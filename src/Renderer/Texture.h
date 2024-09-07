#pragma once

namespace SOF{
    class Texture {
        public:
            static std::shared_ptr<Texture> Create(const char* file_path) {
                return std::make_shared<Texture>(file_path);
            }


            Texture(const char* file_path);
            ~Texture();
            void SetData(unsigned char* data);
            void Bind(uint8_t slot);

        private:
            uint32_t m_ID;
            int m_Width, m_Height;
            int m_Channels;

    };
}