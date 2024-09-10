#pragma once

namespace SOF {
enum class AssetType {
    None = 0,
    Texture,
};

static std::string AssetTypeToString(AssetType type)
{
    if (type == AssetType::Texture) {
        return "Texture";
    } else {
        return "Unknown asset";
    }
}


static AssetType StringToAssetType(std::string type_name)
{
    if (type_name == "Texture") {
        return AssetType::Texture;
    } else {
        return AssetType::None;
    }
}
}// namespace SOF
