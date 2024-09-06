#include "pch.h"
#include "Shaderprogram.h"

namespace SOF{
    Shaderprogram::Shaderprogram(std::string vert, std::string frags) : m_VertPath(vert), m_FragPath(frags)
    {
        m_VertexId = glCreateShader(GL_VERTEX_SHADER);
        m_FragmentId = glCreateShader(GL_FRAGMENT_SHADER);

        std::ifstream vertShaderFile(m_VertPath);
        std::stringstream vert_buffer;

        SOF_ASSERT(vertShaderFile.is_open(), "Failed to open vert Shader file");
        vert_buffer << vertShaderFile.rdbuf();
        std::string vertShaderCode = vert_buffer.str();
        vertShaderFile.close();

        const GLchar* vertSourceCStr = vertShaderCode.c_str();

        std::ifstream fragShaderFile(m_FragPath);
        std::stringstream frag_buffer;

        SOF_ASSERT(fragShaderFile.is_open(), "Failed to open frag Shader file");
        frag_buffer << fragShaderFile.rdbuf();
        std::string fragShaderCode = frag_buffer.str();
        fragShaderFile.close();

        const GLchar* fragSourceCStr = fragShaderCode.c_str();

        glShaderSource(m_VertexId, 1, &vertSourceCStr, 0);
        glShaderSource(m_FragmentId, 1, &fragSourceCStr, 0);

        glCompileShader(m_VertexId);
        glCompileShader(m_FragmentId);

        GLint success = 0;
        glGetShaderiv(m_VertexId, GL_COMPILE_STATUS, &success);
        if(success == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(m_VertexId, GL_INFO_LOG_LENGTH, &maxLength);
            GLchar infoLog[1024];
            glGetShaderInfoLog(m_VertexId, maxLength, &maxLength, infoLog);
            glDeleteShader(m_VertexId);
            SOF_ASSERT(false, "something failed (definetly not the Vertexshader (TOO EASY)): {0}", infoLog);
            return;
        }

        
        glGetShaderiv(m_FragmentId, GL_COMPILE_STATUS, &success);
        if(success == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(m_FragmentId, GL_INFO_LOG_LENGTH, &maxLength);
            GLchar infoLog[1024];
            glGetShaderInfoLog(m_FragmentId, maxLength, &maxLength, infoLog);
            glDeleteShader(m_FragmentId);
            SOF_ASSERT(false, "something failed (definetly not the Fragmentshader (TOO EASY)): {0}", infoLog);
            return;
        }


        m_ShaderProgram = glCreateProgram();
        glAttachShader(m_ShaderProgram, m_VertexId);
        glAttachShader(m_ShaderProgram, m_FragmentId);

        glLinkProgram(m_ShaderProgram);
        GLint isLinked = 0;
        glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, (int *)&isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(m_ShaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

            GLchar infoLog[1024];
            glGetShaderInfoLog(m_ShaderProgram, maxLength, &maxLength, infoLog);

            glDeleteProgram(m_ShaderProgram);
            glDeleteShader(m_VertexId);
            glDeleteShader(m_FragmentId);

            SOF_ASSERT(false, "something failed (definetly not the Shaderprogram (TOO EASY)): {0}", infoLog);
            return;
        }

    }
    Shaderprogram::~Shaderprogram()
    {
        glDeleteProgram(m_ShaderProgram);
        glDeleteShader(m_VertexId);
        glDeleteShader(m_FragmentId);
    }
    void Shaderprogram::Use()
    {
        glUseProgram(m_ShaderProgram);
    }
}