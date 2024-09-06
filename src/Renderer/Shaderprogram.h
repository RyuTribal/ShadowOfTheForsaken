#include <glad/gl.h>

namespace SOF{
    class Shaderprogram{
        public:
            Shaderprogram(std::string vert,  std::string frags);
            ~Shaderprogram();

            void Use();

        private:
            std::string m_VertPath;
            std::string m_FragPath;
            GLuint m_VertexId;
            GLuint m_FragmentId;
            GLuint m_ShaderProgram;
    };
}