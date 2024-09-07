namespace SOF{
    class Texture {
        public:
            void CreateTexture();
            void Bind();
            void UnBind();

            void ApplyTexture();

        private:
            unsigned int texture;

    };
}