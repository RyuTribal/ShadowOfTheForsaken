#include "pch.h"

#include "Renderer.h"
#include "VertexArray.h"
#include "Buffer.h"
#include <glad/gl.h>
#include <glm/gtx/string_cast.hpp>
#include "Core/Game.h"



namespace SOF{
    struct RendererProps{
        Renderer* RendererInstance = nullptr;
        bool FrameBegun = false;
        bool ResizeWindow = false;
    };

    void MessageCallback(
        unsigned source,
        unsigned type,
        unsigned id,
        unsigned severity,
        int length,
        const char* message,
        const void* userParam)
    {
        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH:		 SOF_FATAL("Renderer", message); return;
        case GL_DEBUG_SEVERITY_MEDIUM:       SOF_ERROR("Renderer", message);  return;
        case GL_DEBUG_SEVERITY_LOW:          SOF_WARN("Renderer", message);  return;
        case GL_DEBUG_SEVERITY_NOTIFICATION: SOF_TRACE("Renderer", message);  return;
        }

        SOF_ASSERT(false, "Unknown severity level!");
    }

    static RendererProps s_Props{};

    void Renderer::Init()
    {
        s_Props.RendererInstance = new Renderer();
        
        s_Props.RendererInstance->m_ShaderLibrary.Load("sprite", "assets/shaders/sprite");

#ifdef DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(MessageCallback, nullptr);

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif
    }
    void Renderer::Shutdown()
    {
        s_Props.RendererInstance = nullptr;
    }
    void Renderer::BeginFrame(Camera* camera)
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
        s_Props.RendererInstance->m_Stats = RendererStats();
        SOF_ASSERT(camera != nullptr, "Cannot start a frame without a valid camera!");
        glClearColor(s_Props.RendererInstance->m_BackgroundColor.r, s_Props.RendererInstance->m_BackgroundColor.g, s_Props.RendererInstance->m_BackgroundColor.b, 1.f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

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

    void Renderer::EndFrame()
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
        s_Props.FrameBegun = false;
    }

    void Renderer::DrawSquare(glm::vec4& color, Texture* texture, glm::mat4& transform)
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
        SOF_ASSERT(s_Props.FrameBegun, "Please run BeginFrame() before running any frame specific commands!");
        auto program = s_Props.RendererInstance->m_ShaderLibrary.Get("sprite");

        std::vector<Vertex> vertices = {
            {glm::vec3(0.5f,  0.5f, 0.0f), glm::vec4(1.0f, 1.f, 1.f, 1.f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(0.5f, -0.5f, 0.0f), glm::vec4(1.0f, 1.f, 1.f, 1.f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec4(1.0f, 1.f, 1.f, 1.f), glm::vec2(0.0f, 0.0f)},
            {glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec4(1.0f, 1.f, 1.f, 1.f), glm::vec2(0.0f, 1.0f)},
        };

        std::vector<uint32_t> indices = {
            0, 1, 3,
            1, 2, 3
        };

        auto vertex_array = VertexArray::Create();

        auto vertexBuffer = VertexBuffer::Create(vertices.size() * sizeof(Vertex));
        vertexBuffer->SetLayout({
            {ShaderDataType::Float3, "aPos"},
            {ShaderDataType::Float4, "aColor"},
            {ShaderDataType::Float2, "aTex"}
        });
        vertexBuffer->SetData(vertices.data(), vertices.size() * sizeof(Vertex));

        auto indexBuffer = IndexBuffer::Create(indices.data(), indices.size());

        vertex_array->SetVertexBuffer(vertexBuffer);
        vertex_array->SetIndexBuffer(indexBuffer);

        program->Set("u_Transform", transform);
        program->Set("u_Color", color);
        program->Set("u_ViewMatrix", s_Props.RendererInstance->m_CurrentActiveCamera->GetViewMatrix());
        program->Set("u_ProjectionMatrix", s_Props.RendererInstance->m_CurrentActiveCamera->GetProjectionMatrix());


        program->Activate();
        if (texture != nullptr) {
            texture->Bind(0);
        }
        vertex_array->Bind();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        vertex_array->Unbind();

        s_Props.RendererInstance->m_Stats.DrawCalls += 1;
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
    void Renderer::ResizeWindow()
    {
        s_Props.ResizeWindow = true;
    }
    Camera* Renderer::GetCurrentCamera()
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
        return s_Props.RendererInstance->m_CurrentActiveCamera;
    }
    RendererStats& Renderer::GetStats()
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
        return s_Props.RendererInstance->m_Stats;
    }
}