#include "pch.h"
#include "Deserializers.h"
#include "Manager.h"
#include "Renderer/Texture.h"
#include "Core/Game.h"

namespace SOF
{

    struct TextureMetadata
    {
        uint32_t Width, Height, Channels;
    };

    static void DeserializeTextureMetadata(char *metadata_buffer, TextureMetadata &destination)
    {
        std::string metadata_str(metadata_buffer);

        std::stringstream ss(metadata_str);
        std::string token;

        while (std::getline(ss, token, ';')) {
            if (token.empty()) continue;

            size_t pos = token.find('=');
            if (pos != std::string::npos) {
                std::string key = token.substr(0, pos);
                std::string value = token.substr(pos + 1);

                if (key == "Width") {
                    destination.Width = std::stoi(value);
                } else if (key == "Height") {
                    destination.Height = std::stoi(value);
                } else if (key == "Channels") {
                    destination.Channels = std::stoi(value);
                }
            }
        }
    }


    std::shared_ptr<Asset> TextureDeserializer::Load(TOCEntry &toc, std::vector<char> &data) const
    {
        std::promise<std::shared_ptr<Asset>> texture_promise;
        std::future<std::shared_ptr<Asset>> texture_future = texture_promise.get_future();
        Game::Get()->GetRenderingThread().Run([&texture_promise, &toc, &data]() {
            TextureMetadata metadata{};
            DeserializeTextureMetadata(toc.MetaData, metadata);
            auto texture = new Texture(data.data(), metadata.Width, metadata.Height, metadata.Channels);
            texture_promise.set_value(
              std::shared_ptr<Asset>(texture, [toc](Asset *asset) { AssetManager::RefDeleter(asset, toc.Handle); }));
        });
        return texture_future.get();
    }
}// namespace SOF
