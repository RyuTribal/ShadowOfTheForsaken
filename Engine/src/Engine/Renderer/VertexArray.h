#pragma once

#include "Buffer.h"

namespace SOF
{
    class VertexArray
    {
        public:
        static std::shared_ptr<VertexArray> Create() { return std::make_shared<VertexArray>(); }

        VertexArray();
        ~VertexArray();

        void Bind();
        void Unbind();

        void SetVertexBuffer(const std::shared_ptr<VertexBuffer> &vertex_buffer);
        void SetIndexBuffer(const std::shared_ptr<IndexBuffer> &index_buffer);

        std::shared_ptr<VertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
        std::shared_ptr<IndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }

        private:
        uint32_t m_ID;
        uint32_t m_VertexBufferIndex = 0;
        std::shared_ptr<VertexBuffer> m_VertexBuffer;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;
    };
}// namespace SOF
