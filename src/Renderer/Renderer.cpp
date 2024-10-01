#include "pch.h"

#include "Renderer.h"
#include "VertexArray.h"
#include "Buffer.h"
#include <glad/gl.h>
#include <glm/gtx/string_cast.hpp>
#include "Core/Game.h"
#include "Core/Window.h"


namespace SOF
{
    struct RendererProps
    {
        Renderer *RendererInstance = nullptr;
        bool ResizeWindow = false;
        std::mutex WriteBufferMutex;
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

    void Renderer::Init(Window *window)
    {
        s_Props.RendererInstance = new Renderer();
        s_Props.RendererInstance->m_Context = Context::Create(window->GetNativeWindow());
        s_Props.RendererInstance->m_Context->Init();

        s_Props.RendererInstance->m_ShaderLibrary.Load("sprite", "assets/shaders/sprite");

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

    void Renderer::BeginFrame()
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
        s_Props.RendererInstance->m_Stats = RendererStats();
        auto read_buffer = Game::Get()->GetRenderingThread().GetReadBuffer();
        if (read_buffer->FrameCamera == nullptr) {
            SOF_WARN("Renderer", "No camera detected this frame");
            return;
        }
        ClearScreen();

        s_Props.RendererInstance->m_CurrentActiveCamera = read_buffer->FrameCamera;
        read_buffer->ValidFrame = true;
        if (s_Props.ResizeWindow) {
            Window &window = Game::Get()->GetWindow();
            read_buffer->FrameCamera->SetWidth(window.GetWidth());
            read_buffer->FrameCamera->SetHeight(window.GetHeight());
            glViewport(0, 0, (int)window.GetWidth(), (int)window.GetHeight());
            s_Props.ResizeWindow = false;
        }
    }

    void Renderer::DrawObjects()
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
        auto read_buffer = Game::Get()->GetRenderingThread().GetReadBuffer();
        if (!read_buffer->ValidFrame) { return; }

        for (const auto &[layer, textureBatchMap] : read_buffer->BatchData) {
            for (const auto &[texture, buffers] : textureBatchMap) {
                if (buffers.QuadBuffer.size() > 0) {
                    auto vertex_array = VertexArray::Create();

                    auto vertex_buffer = VertexBuffer::Create(buffers.QuadBuffer.size() * sizeof(Vertex));
                    vertex_buffer->SetLayout({ { ShaderDataType::Float4, "aPos" },
                      { ShaderDataType::Float4, "aColor" },
                      { ShaderDataType::Float2, "aTex" },
                      { ShaderDataType::Float2, "aSpriteCoords" },
                      { ShaderDataType::Float2, "aSpriteSize" } });
                    vertex_buffer->SetData(buffers.QuadBuffer.data(), buffers.QuadBuffer.size() * sizeof(Vertex));

                    auto index_buffer = IndexBuffer::Create(buffers.QuadIndices.data(), buffers.QuadIndices.size());

                    vertex_array->SetVertexBuffer(vertex_buffer);
                    vertex_array->SetIndexBuffer(index_buffer);

                    vertex_array->Bind();
                    auto program = s_Props.RendererInstance->m_ShaderLibrary.Get("sprite");

                    program->Set("u_ViewMatrix", s_Props.RendererInstance->m_CurrentActiveCamera->GetViewMatrix());
                    program->Set(
                      "u_ProjectionMatrix", s_Props.RendererInstance->m_CurrentActiveCamera->GetProjectionMatrix());
                    program->Set("u_UsingTexture", texture != nullptr);
                    program->Activate();

                    if (texture != nullptr) { texture->Bind(0); }
                    glDrawElements(GL_TRIANGLES, buffers.QuadIndices.size(), GL_UNSIGNED_INT, 0);
                    s_Props.RendererInstance->GetStats().DrawCalls++;

                    vertex_array->Unbind();
                }
            }
        }
    }


    void Renderer::EndFrame() { SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized"); }

    void Renderer::SwapBuffers()
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
        s_Props.RendererInstance->m_Context->SwapBuffers();
    }

    void Renderer::SubmitSquare(glm::vec4 &color,
      Texture *texture,
      glm::mat4 &transform,
      glm::vec2 &sprite_coords,
      glm::vec2 &sprite_size,
      int32_t layer)
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
        std::lock_guard<std::mutex> lock(s_Props.WriteBufferMutex);

        auto write_buffer = Game::Get()->GetRenderingThread().GetWriteBuffer();

        glm::vec2 texture_size(texture->GetWidth(), texture->GetHeight());
        glm::vec2 normalized_sprite_size = sprite_size / texture_size;

        write_buffer->BatchData[layer][texture].QuadBuffer.push_back({ transform * glm::vec4(0.5f, 0.5f, 0.0f, 1.f),
          color,
          glm::vec2(1.0f, 1.0f),
          sprite_coords,
          normalized_sprite_size });
        write_buffer->BatchData[layer][texture].QuadBuffer.push_back({ transform * glm::vec4(0.5f, -0.5f, 0.0f, 1.f),
          color,
          glm::vec2(1.0f, 0.0f),
          sprite_coords,
          normalized_sprite_size });
        write_buffer->BatchData[layer][texture].QuadBuffer.push_back({ transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.f),
          color,
          glm::vec2(0.0f, 0.0f),
          sprite_coords,
          normalized_sprite_size });
        write_buffer->BatchData[layer][texture].QuadBuffer.push_back({ transform * glm::vec4(-0.5f, 0.5f, 0.0f, 1.f),
          color,
          glm::vec2(0.0f, 1.0f),
          sprite_coords,
          normalized_sprite_size });

        write_buffer->BatchData[layer][texture].QuadIndices.push_back(write_buffer->BatchData[layer][texture].IndexPtr);
        write_buffer->BatchData[layer][texture].QuadIndices.push_back(
          write_buffer->BatchData[layer][texture].IndexPtr + 1);
        write_buffer->BatchData[layer][texture].QuadIndices.push_back(
          write_buffer->BatchData[layer][texture].IndexPtr + 3);
        write_buffer->BatchData[layer][texture].QuadIndices.push_back(
          write_buffer->BatchData[layer][texture].IndexPtr + 1);
        write_buffer->BatchData[layer][texture].QuadIndices.push_back(
          write_buffer->BatchData[layer][texture].IndexPtr + 2);
        write_buffer->BatchData[layer][texture].QuadIndices.push_back(
          write_buffer->BatchData[layer][texture].IndexPtr + 3);

        write_buffer->BatchData[layer][texture].IndexPtr += 4;

        s_Props.RendererInstance->m_Stats.QuadsDrawn++;
    }


    void Renderer::SubmitTriangle() { SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized"); }
    void Renderer::DrawFrame()
    {
        BeginFrame();
        DrawObjects();
        EndFrame();
    }

    void Renderer::ChangeBackgroundColor(glm::vec3 &color)
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");

        s_Props.RendererInstance->m_BackgroundColor = color;
    }

    void Renderer::ResizeWindow() { s_Props.ResizeWindow = true; }

    void Renderer::SetVSync(bool vsync)
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
        s_Props.RendererInstance->GetContext()->SetVSync(vsync);
    }

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
    Context *Renderer::GetContext()
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
        return s_Props.RendererInstance->m_Context.get();
    }
}// namespace SOF
