#include "pch.h"
#include "Loaders.h"
#include "Manager.h"

#define STBI_NO_SIMD
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <miniaudio/miniaudio.h>


namespace SOF
{


    std::shared_ptr<AssetData> TextureLoader::Load(const std::filesystem::path &path) const
    {
        std::shared_ptr<AssetData> assetData = std::make_shared<AssetData>();

        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
        if (data) {
            assetData->RawData.assign(reinterpret_cast<unsigned char *>(data),
              reinterpret_cast<unsigned char *>(data) + width * height * channels);
            stbi_image_free(data);


            assetData->MetaData["Width"] = std::to_string(width);
            assetData->MetaData["Height"] = std::to_string(height);
            assetData->MetaData["Channels"] = std::to_string(channels);
        }

        return assetData;
    }

    std::shared_ptr<AssetData> AudioLoader::Load(const std::filesystem::path &path) const
    {
        std::shared_ptr<AssetData> assetData = std::make_shared<AssetData>();
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file) {
            SOF_ERROR("AudioLoader", "File could not be opened!");
            return nullptr;
        }

        std::streamsize dataSize = file.tellg();
        file.seekg(0, std::ios::beg);
        assetData->RawData.resize(dataSize);
        SOF_ASSERT(
          file.read(reinterpret_cast<char *>(assetData->RawData.data()), dataSize), "Could not read audio file!");
        ma_decoder_config decoderConfig = ma_decoder_config_init(ma_format_unknown, 0, 0);
        ma_decoder decoder;

        ma_result result = ma_decoder_init_memory(assetData->RawData.data(), dataSize, &decoderConfig, &decoder);
        SOF_ASSERT(result == MA_SUCCESS, "Could not initialize audio decoder!");

        ma_uint64 totalFrameCount = 0;
        result = ma_data_source_get_length_in_pcm_frames(&decoder, &totalFrameCount);
        if (result == MA_SUCCESS && decoder.outputSampleRate != 0) {
            double duration = static_cast<double>(totalFrameCount) / decoder.outputSampleRate;
            assetData->MetaData["Duration"] = std::to_string(duration);
        }
        ma_decoder_uninit(&decoder);

        return assetData;
    }

}// namespace SOF
