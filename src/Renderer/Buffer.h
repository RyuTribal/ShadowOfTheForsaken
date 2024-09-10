#pragma once


namespace SOF {

enum class ShaderDataType { None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool };

static uint32_t ShaderDataTypeSizes(ShaderDataType type)
{
    switch (type) {
    case ShaderDataType::Float:
        return 4;
    case ShaderDataType::Float2:
        return 4 * 2;
    case ShaderDataType::Float3:
        return 4 * 3;
    case ShaderDataType::Float4:
        return 4 * 4;
    case ShaderDataType::Mat3:
        return 4 * 3 * 3;
    case ShaderDataType::Mat4:
        return 4 * 4 * 4;
    case ShaderDataType::Int:
        return 4;
    case ShaderDataType::Int2:
        return 4 * 2;
    case ShaderDataType::Int3:
        return 4 * 3;
    case ShaderDataType::Int4:
        return 4 * 4;
    case ShaderDataType::Bool:
        return 1;
    }

    SOF_ASSERT(false, "Unknown shader data type");
    return 0;
}

struct BufferElement
{
    std::string Name;
    ShaderDataType Type;
    uint32_t Size;
    size_t Offset;
    bool Normalized;

    BufferElement() = default;
    BufferElement(ShaderDataType type, const std::string &name, bool normalized = false)
      : Name(name), Type(type), Size(ShaderDataTypeSizes(type)), Normalized(normalized)
    {}

    uint32_t GetComponentCount() const
    {
        switch (Type) {
        case ShaderDataType::Float:
            return 1;
        case ShaderDataType::Float2:
            return 2;
        case ShaderDataType::Float3:
            return 3;
        case ShaderDataType::Float4:
            return 4;
        case ShaderDataType::Mat3:
            return 3;
        case ShaderDataType::Mat4:
            return 4;
        case ShaderDataType::Int:
            return 1;
        case ShaderDataType::Int2:
            return 2;
        case ShaderDataType::Int3:
            return 3;
        case ShaderDataType::Int4:
            return 4;
        case ShaderDataType::Bool:
            return 1;
        }

        SOF_ASSERT(false, "Unknown shader data type");
        return 0;
    }
};

class BufferLayout
{
  public:
    BufferLayout() {}

    BufferLayout(std::initializer_list<BufferElement> elements) : m_Elements(elements) { CalculateOffsetsAndStride(); }

    uint32_t GetStride() const { return m_Stride; }
    const std::vector<BufferElement> &GetElements() const { return m_Elements; }

    std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
    std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
    std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
    std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

  private:
    void CalculateOffsetsAndStride()
    {
        size_t offset = 0;
        m_Stride = 0;
        for (auto &element : m_Elements) {
            element.Offset = offset;
            offset += element.Size;
            m_Stride += element.Size;
        }
    }

  private:
    std::vector<BufferElement> m_Elements;
    uint32_t m_Stride = 0;
};

class VertexBuffer
{
  public:
    VertexBuffer(uint32_t max_size);
    ~VertexBuffer();

    static std::shared_ptr<VertexBuffer> Create(uint32_t size);

    void Bind();
    void Unbind();

    void SetData(const void *data, uint32_t size);

    uint32_t MaxSize() { return m_MaxSize; }
    uint32_t Size() { return m_Size; }
    void Resize(uint32_t new_size);

    const BufferLayout &GetLayout() { return m_Layout; }
    void SetLayout(const BufferLayout &layout) { m_Layout = layout; }

  private:
    uint32_t m_ID;
    BufferLayout m_Layout;
    uint32_t m_MaxSize = 0;
    uint32_t m_Size = 0;
    uint32_t m_Index = 0;
};

class IndexBuffer
{
  public:
    IndexBuffer(uint32_t *indices, uint32_t count);
    ~IndexBuffer();

    static std::shared_ptr<IndexBuffer> Create(uint32_t *indices, uint32_t count);

    void Bind();
    void Unbind();

    uint32_t GetCount() { return m_Count; }

  private:
    uint32_t m_ID;
    uint32_t m_Count;
};
}// namespace SOF
