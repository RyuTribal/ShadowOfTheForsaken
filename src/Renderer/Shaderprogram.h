#include <glad/gl.h>

namespace SOF{
    class Shaderprogram{
    public:

        static std::shared_ptr<Shaderprogram> Create(std::string vert, std::string frags) {
            return std::make_shared<Shaderprogram>(vert, frags);
        }

        Shaderprogram(std::string vert,  std::string frags);
        ~Shaderprogram();

        void Activate();
        void Deactivate();

        GLuint GetProgramID() { return m_ShaderProgram; }

        void Set(const std::string& name, float value);
        void Set(const std::string& name, int value);
        void Set(const std::string& name, uint32_t value);
        void Set(const std::string& name, bool value);
        void Set(const std::string& name, const glm::ivec2& value);
        void Set(const std::string& name, const glm::ivec3& value);
        void Set(const std::string& name, const glm::ivec4& value);
        void Set(const std::string& name, const glm::vec2& value);
        void Set(const std::string& name, const glm::vec3& value);
        void Set(const std::string& name, const glm::vec4& value);
        void Set(const std::string& name, const glm::mat3& value);
        void Set(const std::string& name, const glm::mat4& value);

    private:
        std::string m_VertPath;
        std::string m_FragPath;
        GLuint m_VertexId;
        GLuint m_FragmentId;
        GLuint m_ShaderProgram;
    };



    class ShaderLibrary
    {
    public:
        ShaderLibrary();
        ~ShaderLibrary();

        void Load(std::string_view name, const std::string& path);

        const std::shared_ptr<Shaderprogram>& Get(const std::string& name) const;
        const std::shared_ptr<Shaderprogram>& GetByShaderID(uint32_t id) const;
        size_t GetSize() const { return m_Shaders.size(); }

        std::unordered_map<std::string, std::shared_ptr<Shaderprogram>>& GetShaders() { return m_Shaders; }
        const std::unordered_map<std::string, std::shared_ptr<Shaderprogram>>& GetShaders() const { return m_Shaders; }
    private:
        std::unordered_map<std::string, std::shared_ptr<Shaderprogram>> m_Shaders;
    };
}