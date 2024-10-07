#include "pch.h"
#include "Manager.h"
#include "Asset.h"

namespace SOF
{

    struct ManagerData
    {
        AssetManager *Manager = nullptr;
        std::unordered_map<std::filesystem::path, AssetType> FileExtensions = {
            { ".png", AssetType::Texture },
            { ".jpg", AssetType::Texture },
            { ".gif", AssetType::Texture },
            { ".jpeg", AssetType::Texture },
            { ".mp3", AssetType::Audio },
            { ".ogg", AssetType::Audio },
            { ".wav", AssetType::Audio },
        };
    };

    static ManagerData *s_Data = nullptr;


    void AssetManager::Init(std::filesystem::path path_to_assetpack)
    {
        s_Data = new ManagerData();
        s_Data->Manager = new AssetManager(path_to_assetpack);

        s_Data->Manager->AssetPackInit(path_to_assetpack);

        s_Data->Manager->m_Loaders[AssetType::Texture] = std::make_unique<TextureLoader>();
        s_Data->Manager->m_Loaders[AssetType::Audio] = std::make_unique<AudioLoader>();

        s_Data->Manager->m_Deserializers[AssetType::Texture] = std::make_unique<TextureDeserializer>();
        s_Data->Manager->m_Deserializers[AssetType::Audio] = std::make_unique<AudioDeserializer>();
    }

    void AssetManager::Shutdown()
    {
        SOF_ASSERT(s_Data && s_Data->Manager, "AssetManager has not been initialized, was it you Elliot?!");
        s_Data->Manager->m_OutputFile.close();

        auto temp_manager = s_Data->Manager;

        s_Data = nullptr;

        temp_manager->m_Assets.clear();

        delete temp_manager;
    }

    AssetManager *AssetManager::Instance()
    {
        SOF_ASSERT(s_Data && s_Data->Manager, "AssetManager has not been initialized, was it you Elliot?!");
        return s_Data->Manager;
    }

    void AssetManager::RegisterAsset(std::filesystem::path path_to_file,
      const std::string &asset_handle,
      AssetType asset_type,
      const std::string &custom_asset_type)
    {
        SOF_ASSERT(s_Data && s_Data->Manager, "AssetManager has not been initialized, was it you Elliot?!");

        if (!std::filesystem::exists(path_to_file)) {
            SOF_ERROR("AssetManager", "File does not exist: {0}", path_to_file);
            return;
        }

        if (s_Data->Manager->m_TOCEntries.find(asset_handle) != s_Data->Manager->m_TOCEntries.end()) {
            SOF_ERROR("AssetManager", "The handle {0} already exists!", asset_handle);
            return;
        }

        SOF_TRACE("AssetManager", "Registering file {0}...", path_to_file);

        IAssetLoaderStrategy *loader = nullptr;

        if (asset_type == AssetType::Custom) {
            auto it = s_Data->Manager->m_CustomLoaders.find(custom_asset_type);
            if (it == s_Data->Manager->m_CustomLoaders.end()) {
                SOF_ERROR("AssetManager", "No loader available for asset type {0}", custom_asset_type);
                return;
            }
            loader = it->second.get();

        } else {
            auto it = s_Data->Manager->m_Loaders.find(asset_type);
            if (it == s_Data->Manager->m_Loaders.end()) {
                SOF_ERROR("AssetManager", "No loader available for asset type {0}", AssetTypeToString(asset_type));
                return;
            }
            loader = it->second.get();
        }

        std::shared_ptr<AssetData> data = loader->Load(path_to_file);

        RegisterAssetFromMemory(data, asset_handle, asset_type);

        SOF_INFO("AssetManager", "Registered file: {0}", path_to_file);
    }

    void AssetManager::RegisterAssetFromMemory(std::shared_ptr<AssetData> data,
      const std::string &asset_handle,
      AssetType asset_type,
      const std::string &custom_asset_type)
    {
        // Seek to the end of the file for writing the new asset data
        s_Data->Manager->m_OutputFile.seekp(0, std::ios::end);
        uint64_t dataOffset = s_Data->Manager->m_OutputFile.tellp();
        s_Data->Manager->m_OutputFile.write(data->RawData.data(), data->RawData.size());
        s_Data->Manager->m_OutputFile.flush();// Ensure the data is written to the file

        TOCEntry newEntry;
        newEntry.UUID = UUID();
        strcpy(newEntry.Handle, asset_handle.c_str());
        newEntry.Type = static_cast<uint32_t>(asset_type);
        newEntry.Offset = dataOffset;
        newEntry.Length = data->RawData.size();
        std::string metadataSerialized = s_Data->Manager->SerializeMetadata(data->MetaData);
        strncpy(newEntry.MetaData, metadataSerialized.c_str(), sizeof(newEntry.MetaData) - 1);
        newEntry.MetaData[sizeof(newEntry.MetaData) - 1] = '\0';
        s_Data->Manager->m_TOCEntries[asset_handle] = newEntry;

        // Update TOC and write it
        s_Data->Manager->UpdateTOCOffsets();
        s_Data->Manager->WriteTOC();

        SOF_INFO("AssetManager", "Registered asset: {0}", asset_handle);
    }

    void AssetManager::DeregisterAsset(const std::string &asset_handle, bool force)
    {
        SOF_ASSERT(s_Data && s_Data->Manager, "AssetManager has not been initialized, was it you Elliot?!");

        SOF_TRACE("AssetManager", "Deleting asset {0}...", asset_handle);

        if (s_Data->Manager->m_TOCEntries.find(asset_handle) == s_Data->Manager->m_TOCEntries.end()) {
            SOF_ERROR("AssetManager", "Asset {0} does not exist!", asset_handle);
            return;
        }

        if (s_Data->Manager->m_Assets[asset_handle].use_count() > 1 && !force) {
            SOF_ERROR("AssetManager",
              "Asset {0} is currently in use, please remove it first from the scene before deregistering",
              asset_handle);
            return;
        }

        uint64_t deleted_offset = s_Data->Manager->m_TOCEntries[asset_handle].Offset;
        uint64_t deleted_length = s_Data->Manager->m_TOCEntries[asset_handle].Length;

        s_Data->Manager->m_TOCEntries.erase(asset_handle);
        s_Data->Manager->UpdateTOCOffsets();

        s_Data->Manager->WriteTOC();
        SOF_INFO("AssetManager", "Deleted asset: {0}", asset_handle);
    }

    void AssetManager::RegisterCustomAssetType(const std::string &asset_type_handle,
      IAssetLoaderStrategy *custom_loader,
      IAssetDeserializerStrategy *custom_deserializer)
    {
        SOF_ASSERT(s_Data && s_Data->Manager, "AssetManager has not been initialized, was it you Elliot?!");
        if (custom_loader) {
            s_Data->Manager->m_CustomLoaders[asset_type_handle] = std::unique_ptr<IAssetLoaderStrategy>(custom_loader);
        }
        if (custom_deserializer) {
            s_Data->Manager->m_CustomDeserializers[asset_type_handle] =
              std::unique_ptr<IAssetDeserializerStrategy>(custom_deserializer);
        }

        SOF_INFO("AssetManager", "Registered asset type {0}", asset_type_handle);
    }

    void AssetManager::UpdateTOCOffsets()
    {
        m_GlobalHeader.NumAssets = (uint32_t)m_TOCEntries.size();
        m_GlobalHeader.TOCOffset = sizeof(GlobalHeader);// TOC starts after the global header
    }

    std::string AssetManager::SerializeMetadata(const std::map<std::string, std::string> &metadata)
    {
        std::stringstream ss;
        for (const auto &[key, value] : metadata) { ss << key << "=" << value << ";"; }
        return ss.str();
    }

    void AssetManager::WriteTOC()
    {
        m_OutputFile.seekp(0, std::ios::beg);
        m_OutputFile.write(reinterpret_cast<char *>(&m_GlobalHeader), sizeof(GlobalHeader));

        for (const auto &[handle, entry] : m_TOCEntries) {
            m_OutputFile.write(reinterpret_cast<const char *>(&entry), sizeof(TOCEntry));
        }

        m_OutputFile.flush();
    }

    AssetType AssetManager::FileToAssetType(std::filesystem::path path_to_file)
    {
        return s_Data->FileExtensions[path_to_file.extension()];
    }

    void AssetManager::RefDeleter(Asset *asset, const std::string &asset_handle)
    {
        if (!s_Data || !s_Data->Manager) { return; }
        if (asset) {
            std::shared_ptr<Asset> temp = s_Data->Manager->m_Assets[asset_handle];
            if (temp.use_count() == 1) { s_Data->Manager->m_Assets.erase(asset_handle); }
            delete asset;
        }
    }

    bool AssetManager::Exists(const std::string &asset_handle)
    {
        SOF_ASSERT(s_Data && s_Data->Manager, "AssetManager has not been initialized, was it you Elliot?!");
        return s_Data->Manager->m_TOCEntries.find(asset_handle) != s_Data->Manager->m_TOCEntries.end();
    }

    void AssetManager::AssetPackInit(std::filesystem::path path_to_assetpack)
    {
        SOF_ASSERT(s_Data && s_Data->Manager, "AssetManager has not been initialized, was it you Elliot?!");
        SOF_TRACE("AssetManager", "Opening asset pack...");
        bool fileExists = std::filesystem::exists(path_to_assetpack);

        GlobalHeader header = {};

        if (!fileExists) {
            s_Data->Manager->m_OutputFile.open(path_to_assetpack, std::ios::binary | std::ios::out);
            SOF_ASSERT(s_Data->Manager->m_OutputFile.is_open(), "Failed to open the asset pack!");

            header.Signature[0] = 'S';
            header.Signature[1] = 'O';
            header.Signature[2] = 'F';
            header.Signature[3] = 'P';
            header.Version = 1;
            header.NumAssets = 0;
            header.TOCOffset = sizeof(GlobalHeader);

            s_Data->Manager->m_OutputFile.write(reinterpret_cast<char *>(&header), sizeof(GlobalHeader));
            s_Data->Manager->m_OutputFile.close();
        }

        s_Data->Manager->m_OutputFile.open(path_to_assetpack, std::ios::binary | std::ios::in | std::ios::out);
        SOF_ASSERT(s_Data->Manager->m_OutputFile.is_open(), "Failed to open the asset pack!");

        s_Data->Manager->m_OutputFile.read(reinterpret_cast<char *>(&m_GlobalHeader), sizeof(GlobalHeader));
        SOF_ASSERT(s_Data->Manager->m_OutputFile.gcount() == sizeof(GlobalHeader),
          "Failed to read the global header from file: {0}",
          path_to_assetpack.string());

        SOF_ASSERT(std::strncmp(m_GlobalHeader.Signature, "SOFP", 4) == 0,
          "Invalid file format: {0}",
          path_to_assetpack.string());

        SOF_TRACE("AssetManager", "Global Header Info:");
        SOF_TRACE("AssetManager", "	Signature: {0}", std::string(m_GlobalHeader.Signature, 4));
        SOF_TRACE("AssetManager", "	Version: {0}", m_GlobalHeader.Version);
        SOF_TRACE("AssetManager", "	Number of Assets: {0}", m_GlobalHeader.NumAssets);
        SOF_TRACE("AssetManager", "	TOC Offset: {0}", m_GlobalHeader.TOCOffset);

        // Load in TOC
        const size_t toc_buffer_size = m_GlobalHeader.NumAssets * sizeof(TOCEntry);
        std::vector<char> toc_buffer(toc_buffer_size);
        m_OutputFile.seekg(sizeof(GlobalHeader), std::ios::beg);
        m_OutputFile.read(toc_buffer.data(), toc_buffer.size());
        const char *buffer_ptr = toc_buffer.data();
        for (size_t i = 0; i < m_GlobalHeader.NumAssets; ++i) {
            TOCEntry entry;
            std::memcpy(&entry, buffer_ptr, sizeof(TOCEntry));
            buffer_ptr += sizeof(TOCEntry);

            m_TOCEntries[std::string(entry.Handle)] = entry;
        }
    }

}// namespace SOF
