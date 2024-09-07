#pragma once

#include "Shaderprogram.h"

namespace SOF {

    struct Vertex {
        glm::vec3 Position = { 0.f, 0.f, 0.f };
        glm::vec4 Color = { 1.f, 1.f, 1.f, 1.f };
        glm::vec2 TexCords = {1.f, 1.f};
    };

    class Renderer{
    public:
        static void Init();

        static void Shutdown();

        static void BeginFrame();

        static void EndFrame();

        static void DrawSquare(glm::vec4& color, glm::mat4& transform);

        static void DrawTriangle();

        static void ChangeBackgroundColor(glm::vec3& color);

    private:
        glm::vec3 m_BackgroundColor{0.f, 0.f, 0.f};
        ShaderLibrary m_ShaderLibrary{};
    
    };
}