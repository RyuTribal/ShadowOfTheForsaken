#pragma once

#include "Shaderprogram.h"
#include "Texture.h"
#include "Camera.h"
#include "Context.h"

namespace SOF
{
    class Window;

    struct Vertex
    {
        glm::vec4 Position = { 0.f, 0.f, 0.f, 1.f };
        glm::vec4 Color = { 1.f, 1.f, 1.f, 1.f };
        glm::vec2 TexCords = { 1.f, 1.f };
    };

    struct BatchData
    {
        std::vector<Vertex> QuadBuffer{};
        std::vector<uint32_t> QuadIndices{};
        uint32_t IndexPtr = 0;
    };

    struct RenderBufferData
    {
        Camera *FrameCamera = nullptr;
        std::unordered_map<Texture *, BatchData> BatchData{};
        bool ValidFrame = false;

        void Clear()
        {
            FrameCamera = nullptr;
            BatchData.clear();
            ValidFrame = false;
        }
    };

    struct RendererStats
    {
        uint32_t DrawCalls = 0;
        uint32_t QuadsDrawn = 0;
    };

    class Renderer
    {
        public:
        static void Init(Window *window);

        static void Shutdown();

        static void ClearScreen();

        static void SwapBuffers();

        static void SubmitSquare(glm::vec4 &color, Texture *texture, glm::mat4 &transform);

        static void SubmitTriangle();

        static void DrawFrame();

        static void ChangeBackgroundColor(glm::vec3 &color);

        static void ResizeWindow();

        static void SetVSync(bool vsync);

        static Camera *GetCurrentCamera();

        static RendererStats &GetStats();

        static Context *GetContext();

        private:
        static void BeginFrame();
        static void EndFrame();
        static void DrawObjects();

        private:
        std::unique_ptr<Context> m_Context;
        glm::vec3 m_BackgroundColor{ 0.f, 0.f, 0.f };
        ShaderLibrary m_ShaderLibrary{};
        Camera *m_CurrentActiveCamera = nullptr;
        RendererStats m_Stats{};
    };
}// namespace SOF
