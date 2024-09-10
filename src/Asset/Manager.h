#pragma once

#include "AssetTypes.h"
#include "Loaders.h"
#include "Deserializers.h"
#include "Asset.h"

namespace SOF {

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

  static void RegisterAsset(std::filesystem::path path_to_file, std::string &asset_handle, AssetType asset_type);

  static void DeregisterAsset(std::string &asset_handle);

  template<typename T> static std::shared_ptr<T> Load(std::string &asset_handle)
  {
    if (Instance()->m_TOCEntries.find(asset_handle) == Instance()->m_TOCEntries.end()) {
      SOF_ERROR("AssetManager", "The handle does not exist!");
      return nullptr;
    }

    if (Instance()->m_Assets.find(asset_handle) != Instance()->m_Assets.end()) {
      return std::static_pointer_cast<T>(Instance()->m_Assets[asset_handle]);
    }

    TOCEntry &toc_entry = Instance()->m_TOCEntries[asset_handle];
    Instance()->m_OutputFile.seekg(toc_entry.Offset, std::ios::beg);

    std::vector<char> buffer(toc_entry.Length);
    Instance()->m_OutputFile.read(buffer.data(), buffer.size());

    std::shared_ptr<Asset> asset =
      Instance()->m_Deserializers[static_cast<AssetType>(toc_entry.Type)]->Load(toc_entry, buffer);

    if (asset != nullptr) { Instance()->m_Assets[asset_handle] = asset; }

    return std::static_pointer_cast<T>(asset);
  }

  static AssetType FileToAssetType(std::filesystem::path path_to_file);

  static void RefDeleter(Asset *asset, const std::string &asset_handle);

private:
  AssetManager(std::filesystem::path path_to_assetpack) : m_FilePath(path_to_assetpack) {}
  void AssetPackInit(std::filesystem::path path_to_assetpack);
  void UpdateTOCOffsets(bool add = true);
  std::string SerializeMetadata(const std::map<std::string, std::string> &metadata);
  void WriteTOC(uint64_t deleted_offset = 0, uint64_t deleted_length = 0);// if both are 0 means no entry was deleted


private:
  std::unordered_map<AssetType, std::unique_ptr<IAssetLoaderStrategy>> m_Loaders;
  std::unordered_map<AssetType, std::unique_ptr<IAssetDeserializerStrategy>> m_Deserializers;
  std::map<std::string, TOCEntry> m_TOCEntries;
  std::unordered_map<std::string, std::shared_ptr<Asset>> m_Assets;
  std::fstream m_OutputFile;
  GlobalHeader m_GlobalHeader{};
  std::filesystem::path m_FilePath;
};

}// namespace SOF
