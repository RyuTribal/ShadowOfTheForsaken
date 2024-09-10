#include "pch.h"
#include "VertexArray.h"
#include <glad/gl.h>

namespace SOF {

static GLenum ShaderDataTypeToOpenGLTypes(ShaderDataType type)
{
  switch (type) {
  case ShaderDataType::Float:
    return GL_FLOAT;
  case ShaderDataType::Float2:
    return GL_FLOAT;
  case ShaderDataType::Float3:
    return GL_FLOAT;
  case ShaderDataType::Float4:
    return GL_FLOAT;
  case ShaderDataType::Mat3:
    return GL_FLOAT;
  case ShaderDataType::Mat4:
    return GL_FLOAT;
  case ShaderDataType::Int:
    return GL_INT;
  case ShaderDataType::Int2:
    return GL_INT;
  case ShaderDataType::Int3:
    return GL_INT;
  case ShaderDataType::Int4:
    return GL_INT;
  case ShaderDataType::Bool:
    return GL_BOOL;
  }
}


VertexArray::VertexArray() { glCreateVertexArrays(1, &m_ID); }

VertexArray::~VertexArray() { glDeleteVertexArrays(1, &m_ID); }

void VertexArray::Bind() { glBindVertexArray(m_ID); }

void VertexArray::Unbind() { glBindVertexArray(0); }

void VertexArray::SetVertexBuffer(const std::shared_ptr<VertexBuffer> &vertex_buffer)
{
  SOF_ASSERT(vertex_buffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout, was it you Elliot?!");

  glBindVertexArray(m_ID);
  vertex_buffer->Bind();

  const BufferLayout &layout = vertex_buffer->GetLayout();
  for (const auto &element : layout) {
    switch (element.Type) {
    case ShaderDataType::Float:
    case ShaderDataType::Float2:
    case ShaderDataType::Float3:
    case ShaderDataType::Float4: {
      glEnableVertexAttribArray(m_VertexBufferIndex);
      glVertexAttribPointer(m_VertexBufferIndex,
        element.GetComponentCount(),
        ShaderDataTypeToOpenGLTypes(element.Type),
        element.Normalized ? GL_TRUE : GL_FALSE,
        layout.GetStride(),
        (const void *)element.Offset);
      m_VertexBufferIndex++;
      break;
    }
    case ShaderDataType::Int:
    case ShaderDataType::Int2:
    case ShaderDataType::Int3:
    case ShaderDataType::Int4:
    case ShaderDataType::Bool: {
      glEnableVertexAttribArray(m_VertexBufferIndex);
      glVertexAttribIPointer(m_VertexBufferIndex,
        element.GetComponentCount(),
        ShaderDataTypeToOpenGLTypes(element.Type),
        layout.GetStride(),
        (const void *)element.Offset);
      m_VertexBufferIndex++;
      break;
    }
    case ShaderDataType::Mat3:
    case ShaderDataType::Mat4: {
      uint8_t count = element.GetComponentCount();
      for (uint8_t i = 0; i < count; i++) {
        glEnableVertexAttribArray(m_VertexBufferIndex);
        glVertexAttribPointer(m_VertexBufferIndex,
          count,
          ShaderDataTypeToOpenGLTypes(element.Type),
          element.Normalized ? GL_TRUE : GL_FALSE,
          layout.GetStride(),
          (const void *)(element.Offset + sizeof(float) * count * i));
        glVertexAttribDivisor(m_VertexBufferIndex, 1);
        m_VertexBufferIndex++;
      }
      break;
    }
    default:
      SOF_ASSERT(false, "Unknown shader data type, was it you Elliot?!");
    }
  }
  m_VertexBuffer = vertex_buffer;
  glBindVertexArray(0);
}

void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer> &index_buffer)
{
  glBindVertexArray(m_ID);
  index_buffer->Bind();

  m_IndexBuffer = index_buffer;
  glBindVertexArray(0);
}
}// namespace SOF
