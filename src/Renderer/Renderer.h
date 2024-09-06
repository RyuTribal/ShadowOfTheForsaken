#pragma once

namespace SOF {

    struct Vertex {
        glm::vec3 Position = { 0.f, 0.f, 0.f };
        glm::vec4 Color = { 1.f, 1.f, 1.f, 1.f };
    };

    class Renderer{
    public:
        static void Init();

        static void Shutdown();

        static void BeginFrame();

        static void EndFrame();

        static void DrawSquare();

        static void DrawTriangle();

        static void ChangeBackgroundColor(glm::vec3& color);

        static glm::vec4 color_test;

    private:
        glm::vec3 m_BackgroundColor{0.f, 0.f, 0.f};
    
    };
}