#include "pch.h"
#include "Texture.h"
#include <glad/gl.h>

#define STBI_NO_SIMD
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


namespace SOF{
   
        SOF::Texture::Texture(const char* file_path)
        {
            stbi_set_flip_vertically_on_load(1);
            unsigned char* data = stbi_load(file_path, &m_Width, &m_Height, &m_Channels, 0);
            glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

            glTextureStorage2D(m_ID, 1, m_Channels > 3 ? GL_RGBA8 : GL_RGB8, m_Width, m_Height);
            glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);

            if (data) {
                SetData(data);
            }

            stbi_image_free(data);
        }

        Texture::~Texture()
        {
            glDeleteTextures(1, &m_ID);
        }

        void Texture::SetData(unsigned char* data)
        {
            //SOF_ASSERT(sizeof(*data) == m_Width * m_Height * m_Channels * sizeof(unsigned char), "Texture is not correct!");
            glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, m_Channels > 3 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
        }

        void Texture::Bind(uint8_t slot)
        {
            glBindTextureUnit(slot, m_ID);
        }

}