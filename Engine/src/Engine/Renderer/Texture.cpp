#include "pch.h"
#include "Texture.h"
#include <glad/gl.h>
#include "Engine/Core/Game.h"


namespace SOF
{
    namespace Utils
    {

        static GLenum SOFImageFormatToGLDataFormat(ImageFormat format)
        {
            switch (format) {
            case ImageFormat::R8:
                return GL_RED;
            case ImageFormat::RG8:
                return GL_RG;
            case ImageFormat::RGB8:
                return GL_RGB;
            case ImageFormat::RGBA8:
                return GL_RGBA;
            case ImageFormat::RGB16F:
                return GL_RGB;
            case ImageFormat::RGBA16F:
                return GL_RGBA;
            case ImageFormat::RG32F:
                return GL_RG;
            case ImageFormat::RGB32F:
                return GL_RGB;
            case ImageFormat::RGBA32F:
                return GL_RGBA;
            }

            SOF_ASSERT(false);
            return 0;
        }

        static GLenum SOFImageFormatToGLInternalFormat(ImageFormat format)
        {
            switch (format) {
            case ImageFormat::R8:
                return GL_R8;
            case ImageFormat::RG8:
                return GL_RG8;
            case ImageFormat::RGB8:
                return GL_RGB8;
            case ImageFormat::RGBA8:
                return GL_RGBA8;
            case ImageFormat::RGB16F:
                return GL_RGB16F;
            case ImageFormat::RGBA16F:
                return GL_RGBA16F;
            case ImageFormat::RG32F:
                return GL_RG32F;
            case ImageFormat::RGB32F:
                return GL_RGB32F;
            case ImageFormat::RGBA32F:
                return GL_RGBA32F;
            }

            SOF_ASSERT(false);
            return 0;
        }

        static uint32_t ImageFormatToChannels(ImageFormat format)
        {
            switch (format) {
            case ImageFormat::R8:
                return 1;
            case ImageFormat::RG8:
                return 2;
            case ImageFormat::RGB8:
                return 3;
            case ImageFormat::RGBA8:
                return 4;
            case ImageFormat::RGB16F:
                return 3;
            case ImageFormat::RGBA16F:
                return 4;
            case ImageFormat::RG32F:
                return 2;
            case ImageFormat::RGB32F:
                return 3;
            case ImageFormat::RGBA32F:
                return 4;
            }

            SOF_ASSERT(false);
            return 0;
        }
    }// namespace Utils

    Texture::Texture(const TextureSpecification &specification, const void *data) : m_Specification(specification)
    {

        m_InternalFormat = Utils::SOFImageFormatToGLInternalFormat(m_Specification.Format);
        m_DataFormat = Utils::SOFImageFormatToGLDataFormat(m_Specification.Format);
        m_IsFloat = m_InternalFormat == GL_RGB16F || m_InternalFormat == GL_RGBA16F || m_InternalFormat == GL_RGBA32F
                    || m_InternalFormat == GL_RGB32F || m_InternalFormat == GL_RG32F;

        glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
        if (m_Specification.Format == ImageFormat::DEPTH_COMPONENT) {
            glTextureStorage2D(m_ID, 1, GL_DEPTH_COMPONENT24, m_Specification.Width, m_Specification.Height);
            glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTextureParameterfv(m_ID, GL_TEXTURE_BORDER_COLOR, borderColor);
        } else {
            glTextureStorage2D(m_ID, 1, m_InternalFormat, m_Specification.Width, m_Specification.Height);

            glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        if (data) { SetData(data); }
    }

    Texture::~Texture() { glDeleteTextures(1, &m_ID); }

    void Texture::SetData(const void *data)
    {
        // SOF_ASSERT(sizeof(*data) == m_Width * m_Height * m_Channels * sizeof(unsigned char), "Texture is not
        // correct!");
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTextureSubImage2D(m_ID,
          0,
          0,
          0,
          m_Specification.Width,
          m_Specification.Height,
          m_DataFormat,
          m_IsFloat ? GL_FLOAT : GL_UNSIGNED_BYTE,
          data);
    }

    void Texture::Bind(uint8_t slot) { glBindTextureUnit(slot, m_ID); }

    void Texture::Bind(uint32_t id, uint8_t slot) { glBindTextureUnit(slot, id); }

}// namespace SOF
