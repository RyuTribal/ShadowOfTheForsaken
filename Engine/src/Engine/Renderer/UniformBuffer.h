#pragma once

namespace SOF
{
    class UniformBuffer
    {
        public:
        static std::shared_ptr<UniformBuffer> Create(uint64_t size, uint32_t binding)
        {
            return std::make_shared<UniformBuffer>(size, binding);
        }
        UniformBuffer(uint64_t size, uint32_t binding);
        ~UniformBuffer();

        void SetData(const void *data, uint64_t size, uint32_t offset = 0);

        private:
        uint32_t m_RendererID = 0;
    };

    // Similar class but serves slightly different purpose
    class ShaderStorageBuffer
    {
        public:
        ShaderStorageBuffer(uint64_t size, uint32_t binding);

        ~ShaderStorageBuffer();

        void SetData(const void *data, uint64_t size, uint32_t offset = 0);

        void Bind() const;

        void Unbind() const;

        void Unmap() const;

        void *Map() const;

        private:
        uint32_t m_RendererID;
        uint32_t m_Binding;
    };
}// namespace SOF