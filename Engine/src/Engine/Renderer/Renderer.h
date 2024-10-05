#pragma once

#include "Shaderprogram.h"
#include "Texture.h"
#include "Camera.h"
#include "Context.h"


namespace SOF
{
    class Window;
    struct SpriteComponent;

    struct Vertex
    {
        glm::vec4 Position = { 0.f, 0.f, 0.f, 1.f };
        glm::vec4 Color = { 1.f, 1.f, 1.f, 1.f };
        glm::vec2 TexCoords = { 1.f, 1.f };
        glm::vec2 SpriteSize = { 32.f, 32.f };
        glm::vec2 SpriteSegments = { 1.f, 1.f };
        float TileIndexOffset = 0.f;
    };

    struct BatchData
    {
        std::vector<Vertex> QuadBuffer{};
        std::vector<uint32_t> QuadIndices{};
        std::vector<glm::vec2> TileIndices{};
        uint32_t IndexPtr = 0;
        uint32_t TimeIndexPtr = 0;
    };

    struct RenderBufferData
    {
        Camera *FrameCamera = nullptr;
        std::map<int32_t, std::unordered_map<Texture *, BatchData>>
          CurrentBatch{};// Not the best, but we will solve this problem if it ever comes to it
        bool ValidFrame = false;

        void Clear()
        {
            FrameCamera = nullptr;
            CurrentBatch.clear();
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

        static void SubmitSquare(SpriteComponent *sprite_comp, const glm::mat4 &transform);

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
