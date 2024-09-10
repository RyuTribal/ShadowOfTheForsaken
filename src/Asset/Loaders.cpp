#include "pch.h"
#include "Loaders.h"
#include "Manager.h"

#define STBI_NO_SIMD
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


namespace SOF
{


    std::shared_ptr<AssetData> TextureLoader::Load(const std::filesystem::path &path) const
    {
        std::shared_ptr<AssetData> assetData = std::make_shared<AssetData>();

        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
        if (data) {
            assetData->RawData.assign(
              reinterpret_cast<char *>(data), reinterpret_cast<char *>(data) + width * height * channels);
            stbi_image_free(data);


            assetData->MetaData["Width"] = std::to_string(width);
            assetData->MetaData["Height"] = std::to_string(height);
            assetData->MetaData["Channels"] = std::to_string(channels);
        }

        return assetData;
    }

}// namespace SOF
