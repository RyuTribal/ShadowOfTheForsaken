#pragma once

#include "Shaderprogram.h"
#include "Texture.h"
#include "Camera.h"

namespace SOF {

struct Vertex
{
    glm::vec4 Position = { 0.f, 0.f, 0.f, 1.f };
    glm::vec4 Color = { 1.f, 1.f, 1.f, 1.f };
    glm::vec2 TexCords = { 1.f, 1.f };
};

struct RendererStats
{
    uint32_t DrawCalls = 0;
    uint32_t QuadsDrawn = 0;
};

class Renderer
{
  public:
    static void Init();

    static void Shutdown();

    static void ClearScreen();

    static void BeginFrame(Camera *camera);

    static void EndFrame();

    static void DrawSquare(glm::vec4 &color, Texture *texture, glm::mat4 &transform);

    static void DrawTriangle();

    static void ChangeBackgroundColor(glm::vec3 &color);

    static void ResizeWindow();

    static Camera *GetCurrentCamera();

    static RendererStats &GetStats();

  private:
    static void RecreateVertexBuffers();
    static void Draw();

  private:
    glm::vec3 m_BackgroundColor{ 0.f, 0.f, 0.f };
    ShaderLibrary m_ShaderLibrary{};
    Camera *m_CurrentActiveCamera = nullptr;
    RendererStats m_Stats{};
};
}// namespace SOF
