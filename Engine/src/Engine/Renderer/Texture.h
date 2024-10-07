#pragma once
#include "Engine/Asset/Asset.h"

namespace SOF
{
    class Texture : public Asset
    {
        public:
        static std::shared_ptr<Texture> Create(const char *data, uint32_t width, uint32_t height, uint32_t channels)
        {
            return std::make_shared<Texture>(data, width, height, channels);
        }

        Texture(const char *data, uint32_t width, uint32_t height, uint32_t channels);
        ~Texture();
        void UploadTextureToGPU(const char *data);
        void SetData(const char *data);
        void Bind(uint8_t slot);

        uint32_t GetRendererID() { return m_ID; }

        static void Bind(uint32_t id, uint8_t slot);

        virtual AssetType GetType() const override { return AssetType::Texture; };

        uint32_t GetWidth() { return m_Width; }
        uint32_t GetHeight() { return m_Height; }

        private:
        uint32_t m_ID;
        uint32_t m_Width, m_Height;
        uint32_t m_Channels;
    };
}// namespace SOF
