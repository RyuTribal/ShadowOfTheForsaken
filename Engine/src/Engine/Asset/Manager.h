#pragma once

#include "AssetTypes.h"
#include "Loaders.h"
#include "Deserializers.h"
#include "Asset.h"
#include "Engine/Core/ThreadPool.h"

namespace SOF
{

    struct AssetData
    {
        std::vector<char> RawData;
        std::map<std::string, std::string> MetaData;
    };

    struct AssetInfo
    {
        uint64_t UUID;
        std::string Handle;
        uint32_t Type;
        uint64_t Offset;
        uint64_t Length;
    };

    struct GlobalHeader
    {
        char Signature[4] = { 'S', 'O', 'F', 'P' };
        uint16_t Version = 1;
        uint32_t NumAssets;
        uint64_t TOCOffset;
    };

    struct TOCEntry
    {
        uint64_t UUID;
        uint8_t HandleLength;
        char Handle[255];
        uint32_t Type;
        uint64_t Offset;
        uint64_t Length;
        char MetaData[256];
    };

    class AssetManager
    {
        public:
        static void Init(std::filesystem::path path_to_assetpack);
        static void Shutdown();
        static AssetManager *Instance();

        static void RegisterAsset(std::filesystem::path path_to_file,
          const std::string &asset_handle,
          AssetType asset_type,
          const std::string &custom_asset_type = "");

        static void RegisterAssetFromMemory(std::shared_ptr<AssetData> data,
          const std::string &asset_handle,
          AssetType asset_type,
          const std::string &custom_asset_type = "");

        static void DeregisterAsset(const std::string &asset_handle, bool force = false);

        static std::string ExtractMetadataVariable(const char *metadata, const std::string &key)
        {
            std::string metadata_str(metadata);
            std::stringstream ss(metadata_str);
            std::string token;

            while (std::getline(ss, token, ';')) {
                if (token.empty()) continue;

                size_t pos = token.find('=');
                if (pos != std::string::npos) {
                    std::string found_key = token.substr(0, pos);
                    std::string value_str = token.substr(pos + 1);

                    if (found_key == key) { return value_str; }
                }
            }
            return "";
        }

        static void RegisterCustomAssetType(const std::string &asset_type_handle,
          IAssetLoaderStrategy *custom_loader,
          IAssetDeserializerStrategy *custom_deserializer);

        template<typename T> static std::shared_ptr<T> Load(const std::string &asset_handle)
        {
            if (Instance()->m_TOCEntries.find(asset_handle) == Instance()->m_TOCEntries.end()) {
                SOF_ERROR("AssetManager", "The handle does not exist!");
                return nullptr;
            }

            if (Instance()->m_Assets.find(asset_handle) != Instance()->m_Assets.end()) {
                return std::static_pointer_cast<T>(Instance()->m_Assets[asset_handle]);
            }

            TOCEntry &toc_entry = Instance()->m_TOCEntries[asset_handle];
            std::vector<char> buffer(toc_entry.Length);
            {
                std::lock_guard<std::mutex> lock(Instance()->m_FileMutex);
                Instance()->m_OutputFile.seekg(toc_entry.Offset, std::ios::beg);
                Instance()->m_OutputFile.read(buffer.data(), buffer.size());
            }

            std::shared_ptr<Asset> asset = nullptr;

            if ((AssetType)toc_entry.Type == AssetType::Custom) {
                std::string metadata_str(toc_entry.MetaData);

                std::stringstream ss(metadata_str);
                std::string token;
                bool found_key = false;
                std::string custom_asset_type;
                while (std::getline(ss, token, ';')) {
                    if (token.empty()) continue;

                    size_t pos = token.find('=');
                    if (pos != std::string::npos) {
                        std::string key = token.substr(0, pos);
                        std::string value = token.substr(pos + 1);
                        if (key == "AssetType") {
                            custom_asset_type = value;
                            found_key = true;
                            break;
                        }
                    }
                }
                SOF_ASSERT(
                  found_key, "No 'AssetType' key found in your metadata, make sure you serialized that into AssetData");
                auto it = Instance()->m_CustomDeserializers.find(custom_asset_type);
                SOF_ASSERT(it != Instance()->m_CustomDeserializers.end(),
                  "This asset type is not registered. Make sure you register it using RegisterCustomAssetType");
                asset = it->second->Load(toc_entry, buffer);
            } else {
                asset = Instance()->m_Deserializers[static_cast<AssetType>(toc_entry.Type)]->Load(toc_entry, buffer);
            }
            if (asset != nullptr) { Instance()->m_Assets[asset_handle] = asset; }

            return std::static_pointer_cast<T>(asset);
        }

        static AssetType FileToAssetType(std::filesystem::path path_to_file);

        static void RefDeleter(Asset *asset, const std::string &asset_handle);

        static GlobalHeader &GetGlobalHeader() { return Instance()->m_GlobalHeader; }
        static std::map<std::string, TOCEntry> &GetTOCEntries() { return Instance()->m_TOCEntries; }

        static bool Exists(const std::string &asset_handle);

        private:
        AssetManager(std::filesystem::path path_to_assetpack) : m_FilePath(path_to_assetpack) {}
        void AssetPackInit(std::filesystem::path path_to_assetpack);
        void UpdateTOCOffsets();
        std::string SerializeMetadata(const std::map<std::string, std::string> &metadata);
        void WriteTOC();

        private:
        std::unordered_map<AssetType, std::unique_ptr<IAssetLoaderStrategy>> m_Loaders;
        std::unordered_map<AssetType, std::unique_ptr<IAssetDeserializerStrategy>> m_Deserializers;
        std::unordered_map<std::string, std::unique_ptr<IAssetLoaderStrategy>> m_CustomLoaders;
        std::unordered_map<std::string, std::unique_ptr<IAssetDeserializerStrategy>> m_CustomDeserializers;
        std::map<std::string, TOCEntry> m_TOCEntries;
        std::unordered_map<std::string, std::shared_ptr<Asset>> m_Assets;
        std::fstream m_OutputFile;
        GlobalHeader m_GlobalHeader{};
        std::filesystem::path m_FilePath;
        std::mutex m_FileMutex;
    };

}// namespace SOF
