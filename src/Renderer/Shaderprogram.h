#include <glad/gl.h>

namespace SOF{
    class Shaderprogram{
        public:
            Shaderprogram(std::string vert,  std::string frags);
            ~Shaderprogram();

            void Activate();
            void Deactivate();

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
}