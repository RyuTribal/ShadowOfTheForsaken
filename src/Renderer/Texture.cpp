#include <pch.h>
#include <glad/gl.h>
#include <stb_image.h>

namespace SOF{
    
        void CreateTexture(uint32_t texture){
            int width, height, nrChannels;
            unsigned int texture;
            unsigned char *data =stbi_load("assets/images/shadowheart.jpg", &width, &height, &nrChannels, 0);
            glGenTextures(1, &texture);
            Bind(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }

        void Bind(uint32_t slot, uint32_t texture){
            glBindTextureUnit(slot, texture);
        }
    



}