#pragma once
#include "Engine/Asset/Asset.h"

namespace SOF
{
    enum class ImageFormat { None = 0, R8, RG8, RGB8, RGBA8, RGB16F, RGBA16F, RG32F, RGB32F, RGBA32F, DEPTH_COMPONENT };

    struct TextureSpecification
    {
        uint32_t Width = 1;
        uint32_t Height = 1;
        uint32_t Size = 0;
        ImageFormat Format = ImageFormat::RGBA8;
        bool GenerateMips = true;
    };

    class Texture : public Asset
    {
        public:
        static std::shared_ptr<Texture> Create(const TextureSpecification &specification, const void *data)
        {
            return std::make_shared<Texture>(specification, data);
        }

        Texture(const TextureSpecification &specification, const void *data);
        ~Texture();
        void SetData(const void *data);
        void Bind(uint8_t slot);

        const TextureSpecification &GetSpecification() const { return m_Specification; }

        uint32_t GetRendererID() { return m_ID; }

        static void Bind(uint32_t id, uint8_t slot);

        virtual AssetType GetType() const override { return AssetType::Texture; };

        uint32_t GetWidth() { return m_Specification.Width; }
        uint32_t GetHeight() { return m_Specification.Height; }

        private:
        TextureSpecification m_Specification;
        uint32_t m_ID;
        bool m_IsFloat = false;
        GLenum m_InternalFormat, m_DataFormat;
    };
}// namespace SOF
