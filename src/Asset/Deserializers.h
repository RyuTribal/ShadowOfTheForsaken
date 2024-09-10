#pragma once

namespace SOF {

struct TOCEntry;
class Asset;

class IAssetDeserializerStrategy
{
public:
  virtual ~IAssetDeserializerStrategy() = default;
  virtual std::shared_ptr<Asset> Load(TOCEntry &toc, std::vector<char> &data) const = 0;
};

class TextureDeserializer : public IAssetDeserializerStrategy
{
public:
  virtual std::shared_ptr<Asset> Load(TOCEntry &toc, std::vector<char> &data) const override;
};
}// namespace SOF
