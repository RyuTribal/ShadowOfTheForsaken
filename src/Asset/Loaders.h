#pragma once

namespace SOF {

struct AssetData;

class IAssetLoaderStrategy
{
public:
  virtual ~IAssetLoaderStrategy() = default;
  virtual std::shared_ptr<AssetData> Load(const std::filesystem::path &path) const = 0;
};


class TextureLoader : public IAssetLoaderStrategy
{
public:
  virtual std::shared_ptr<AssetData> Load(const std::filesystem::path &path) const override;
};
}// namespace SOF
