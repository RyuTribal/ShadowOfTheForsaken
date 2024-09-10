#include "pch.h"
#include "Buffer.h"
#include <glad/gl.h>

namespace SOF {

/// VERTEX BUFFER

VertexBuffer::VertexBuffer(uint32_t max_size) : m_MaxSize(max_size)
{
    glCreateBuffers(1, &m_ID);
    glBindBuffer(GL_ARRAY_BUFFER, m_ID);
    glBufferData(GL_ARRAY_BUFFER, m_MaxSize, nullptr, GL_DYNAMIC_DRAW);
}

VertexBuffer::~VertexBuffer() { glDeleteBuffers(1, &m_ID); }
std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size) { return std::make_shared<VertexBuffer>(size); }
void VertexBuffer::Bind() { glBindBuffer(GL_ARRAY_BUFFER, m_ID); }
void VertexBuffer::Unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

void VertexBuffer::SetData(const void *data, uint32_t size)
{
    if (m_Size + size > m_MaxSize) {
        Resize(m_MaxSize + size * 2);// Double the size to minimize future resizes
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_ID);
    glBufferSubData(GL_ARRAY_BUFFER, m_Size, size, data);
    m_Index++;
    m_Size += size;
}

void VertexBuffer::Resize(uint32_t new_size)
{
    if (new_size <= m_MaxSize) {
        // No need to resize if the new size is smaller or equal to the current max size
        return;
    }

    GLuint newBufferID;
    glCreateBuffers(1, &newBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, newBufferID);
    glBufferData(GL_ARRAY_BUFFER, new_size, nullptr, GL_DYNAMIC_DRAW);

    if (m_Size > 0) {
        glBindBuffer(GL_COPY_READ_BUFFER, m_ID);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, m_Size);
    }

    glDeleteBuffers(1, &m_ID);

    m_ID = newBufferID;
    m_MaxSize = new_size;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


/// INDEX BUFFER

IndexBuffer::IndexBuffer(uint32_t *indices, uint32_t count) : m_Count(count)
{
    glCreateBuffers(1, &m_ID);
    glBindBuffer(GL_ARRAY_BUFFER, m_ID);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() { glDeleteBuffers(1, &m_ID); }

std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t *indices, uint32_t count)
{
    return std::make_shared<IndexBuffer>(indices, count);
}

void IndexBuffer::Bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID); }

void IndexBuffer::Unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
}// namespace SOF
