#pragma once

namespace SOF
{
    enum class AssetType {
        None = 0,
        Texture,
        Audio,
        Custom,
    };

    static std::string AssetTypeToString(AssetType type)
    {
        if (type == AssetType::Texture) {
            return "Texture";
        } else if (type == AssetType::Audio) {
            return "Audio";
        } else if (type == AssetType::Custom) {
            return "Custom";
        } else {
            return "Unknown asset";
        }
    }


    static AssetType StringToAssetType(std::string type_name)
    {
        if (type_name == "Texture") {
            return AssetType::Texture;
        } else if (type_name == "Audio") {
            return AssetType::Audio;
        } else if (type_name == "Custom") {
            return AssetType::Custom;
        } else {
            return AssetType::None;
        }
    }
}// namespace SOF
