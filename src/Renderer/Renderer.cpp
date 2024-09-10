#include "pch.h"

#include "Renderer.h"
#include "VertexArray.h"
#include "Buffer.h"
#include <glad/gl.h>
#include <glm/gtx/string_cast.hpp>
#include "Core/Game.h"


namespace SOF {
struct RendererProps
{
  Renderer *RendererInstance = nullptr;
  bool FrameBegun = false;
  bool ResizeWindow = false;
  std::shared_ptr<VertexBuffer> QuadBuffer;
  std::vector<uint32_t> QuadIndices{};
  uint32_t IndexPtr = 0;

  // TODO REMOVE
  Texture *test_tex = nullptr;
};

void MessageCallback(unsigned source,
  unsigned type,
  unsigned id,
  unsigned severity,
  int length,
  const char *message,
  const void *userParam)
{
  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    SOF_FATAL("Renderer", message);
    return;
  case GL_DEBUG_SEVERITY_MEDIUM:
    SOF_ERROR("Renderer", message);
    return;
  case GL_DEBUG_SEVERITY_LOW:
    SOF_WARN("Renderer", message);
    return;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    SOF_TRACE("Renderer", message);
    return;
  }

  SOF_ASSERT(false, "Unknown severity level!");
}

static RendererProps s_Props{};

void Renderer::Init()
{
  s_Props.RendererInstance = new Renderer();

  s_Props.RendererInstance->m_ShaderLibrary.Load("sprite", "assets/shaders/sprite");

  RecreateVertexBuffers();

#ifdef DEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(MessageCallback, nullptr);

  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif
}
void Renderer::Shutdown()
{
  delete s_Props.RendererInstance;
  s_Props.RendererInstance = nullptr;
}
void Renderer::ClearScreen()
{
  SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
  glClearColor(s_Props.RendererInstance->m_BackgroundColor.r,
    s_Props.RendererInstance->m_BackgroundColor.g,
    s_Props.RendererInstance->m_BackgroundColor.b,
    1.f);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void Renderer::BeginFrame(Camera *camera)
{
  SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
  s_Props.RendererInstance->m_Stats = RendererStats();
  SOF_ASSERT(camera != nullptr, "Cannot start a frame without a valid camera!");

  s_Props.RendererInstance->m_CurrentActiveCamera = camera;
  s_Props.FrameBegun = true;
  if (s_Props.ResizeWindow) {
    Window &window = Game::Get()->GetWindow();
    camera->SetWidth(window.GetWidth());
    camera->SetHeight(window.GetHeight());
    glViewport(0, 0, (int)window.GetWidth(), (int)window.GetHeight());
    s_Props.ResizeWindow = false;
  }
}

void Renderer::Draw()
{
  SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");

  if (s_Props.QuadBuffer->Size() > 0) {
    auto vertex_array = VertexArray::Create();
    auto indexBuffer = IndexBuffer::Create(s_Props.QuadIndices.data(), s_Props.QuadIndices.size());

    vertex_array->SetVertexBuffer(s_Props.QuadBuffer);
    vertex_array->SetIndexBuffer(indexBuffer);

    vertex_array->Bind();

    auto program = s_Props.RendererInstance->m_ShaderLibrary.Get("sprite");

    program->Set("u_ViewMatrix", s_Props.RendererInstance->m_CurrentActiveCamera->GetViewMatrix());
    program->Set("u_ProjectionMatrix", s_Props.RendererInstance->m_CurrentActiveCamera->GetProjectionMatrix());

    program->Activate();
    if (s_Props.test_tex != nullptr) { s_Props.test_tex->Bind(0); }

    glDrawElements(GL_TRIANGLES, s_Props.QuadIndices.size(), GL_UNSIGNED_INT, 0);

    s_Props.RendererInstance->GetStats().DrawCalls++;

    vertex_array->Unbind();
  }

  s_Props.IndexPtr = 0;
  RecreateVertexBuffers();
  s_Props.QuadIndices.clear();
}


void Renderer::EndFrame()
{
  SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
  Draw();
  s_Props.FrameBegun = false;
}

void Renderer::RecreateVertexBuffers()
{
  s_Props.QuadBuffer = VertexBuffer::Create(s_Props.QuadBuffer ? s_Props.QuadBuffer->MaxSize() : 1000000);
  s_Props.QuadBuffer->SetLayout(
    { { ShaderDataType::Float4, "aPos" }, { ShaderDataType::Float4, "aColor" }, { ShaderDataType::Float2, "aTex" } });
}

void Renderer::DrawSquare(glm::vec4 &color, Texture *texture, glm::mat4 &transform)
{
  SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
  SOF_ASSERT(s_Props.FrameBegun, "Please run BeginFrame() before running any frame specific commands!");

  std::vector<Vertex> vertices = {
    { transform * glm::vec4(0.5f, 0.5f, 0.0f, 1.f), color, glm::vec2(1.0f, 1.0f) },
    { transform * glm::vec4(0.5f, -0.5f, 0.0f, 1.f), color, glm::vec2(1.0f, 0.0f) },
    { transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.f), color, glm::vec2(0.0f, 0.0f) },
    { transform * glm::vec4(-0.5f, 0.5f, 0.0f, 1.f), color, glm::vec2(0.0f, 1.0f) },
  };

  s_Props.test_tex = texture;

  s_Props.QuadIndices.push_back(s_Props.IndexPtr);
  s_Props.QuadIndices.push_back(s_Props.IndexPtr + 1);
  s_Props.QuadIndices.push_back(s_Props.IndexPtr + 3);
  s_Props.QuadIndices.push_back(s_Props.IndexPtr + 1);
  s_Props.QuadIndices.push_back(s_Props.IndexPtr + 2);
  s_Props.QuadIndices.push_back(s_Props.IndexPtr + 3);

  s_Props.IndexPtr += 4;
  s_Props.QuadBuffer->SetData(vertices.data(), vertices.size() * sizeof(Vertex));


  s_Props.RendererInstance->m_Stats.QuadsDrawn++;
}


void Renderer::DrawTriangle()
{
  SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
  SOF_ASSERT(s_Props.FrameBegun, "Please run BeginFrame() before running any frame specific commands!");
}
void Renderer::ChangeBackgroundColor(glm::vec3 &color)
{
  SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");

  s_Props.RendererInstance->m_BackgroundColor = color;
}
void Renderer::ResizeWindow() { s_Props.ResizeWindow = true; }
Camera *Renderer::GetCurrentCamera()
{
  SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
  return s_Props.RendererInstance->m_CurrentActiveCamera;
}
RendererStats &Renderer::GetStats()
{
  SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
  return s_Props.RendererInstance->m_Stats;
}
}// namespace SOF
