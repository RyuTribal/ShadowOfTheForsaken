#pragma once

namespace SOF {
    class Renderer{
    public:
        static void Init();

        static void Shutdown();

        static void BeginFrame();

        static void EndFrame();

        static void DrawSquare();

        static void DrawTriangle();

        static void ChangeBackgroundColor(glm::vec3& color);

    private:
        glm::vec3 m_BackgroundColor{0.f, 0.f, 0.f};
    
    };
}