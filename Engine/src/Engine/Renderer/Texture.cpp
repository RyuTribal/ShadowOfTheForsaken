#include "pch.h"
#include "Texture.h"
#include <glad/gl.h>
#include "Engine/Core/Game.h"


namespace SOF
{

    SOF::Texture::Texture(const char *data, uint32_t width, uint32_t height, uint32_t channels)
      : m_Width(width), m_Height(height), m_Channels(channels)
    {

        glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

        glTextureStorage2D(m_ID, 1, m_Channels > 3 ? GL_RGBA8 : GL_RGB8, m_Width, m_Height);
        glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        if (data) { SetData(data); }
    }

    Texture::~Texture() { glDeleteTextures(1, &m_ID); }

    void Texture::SetData(const char *data)
    {
        // SOF_ASSERT(sizeof(*data) == m_Width * m_Height * m_Channels * sizeof(unsigned char), "Texture is not
        // correct!");
        glTextureSubImage2D(
          m_ID, 0, 0, 0, m_Width, m_Height, m_Channels > 3 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    }

    void Texture::Bind(uint8_t slot) { glBindTextureUnit(slot, m_ID); }

}// namespace SOF
