#include "pch.h"

#include "Renderer.h"
#include "Shaderprogram.h"
#include <glad/gl.h>

namespace SOF{
    struct RendererProps{
        Renderer* RendererInstance = nullptr;
    };

    static RendererProps s_Props{};

    void Renderer::Init()
    {
        s_Props.RendererInstance = new Renderer();
    }
    void Renderer::Shutdown()
    {
        s_Props.RendererInstance = nullptr;
    }
    void Renderer::BeginFrame()
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
        glClearColor(s_Props.RendererInstance->m_BackgroundColor.r, s_Props.RendererInstance->m_BackgroundColor.g, s_Props.RendererInstance->m_BackgroundColor.b, 1.f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }

    void Renderer::EndFrame()
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
    }

    void Renderer::DrawSquare()
    {
        DrawTriangle();
    }
    void Renderer::DrawTriangle()
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
        Shaderprogram program = Shaderprogram("assets/shaders/triangle.vert", "assets/shaders/triangle.frag");

        float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f
        };

        unsigned int VBO;
        glCreateBuffers(1, &VBO);
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);  

        // 0. copy our vertices array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // 1. then set the vertex attributes pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);  

        unsigned int VAO;
        glGenVertexArrays(1, &VAO);  

        glBindVertexArray(VAO);
        // 2. copy our vertices array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // 3. then set our vertex attributes pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);  

        // 2. use our shader program when we want to render an object
        program.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    void Renderer::ChangeBackgroundColor(glm::vec3 &color)
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");

        s_Props.RendererInstance->m_BackgroundColor = color;
    }
}