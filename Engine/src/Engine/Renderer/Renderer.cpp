#include "pch.h"

#include "Renderer.h"
#include "VertexArray.h"
#include "Buffer.h"
#include <glad/gl.h>
#include <glm/gtx/string_cast.hpp>
#include "Engine/Core/Game.h"
#include "Engine/Core/Window.h"
#include "Engine/Scene/Components.h"
#include "UniformBuffer.h"


namespace SOF
{
    struct RendererProps
    {
        Renderer *RendererInstance = nullptr;
        bool ResizeWindow = true;
        std::mutex WriteBufferMutex;
        std::unique_ptr<Texture> DefaultBackground = nullptr;
        std::shared_ptr<VertexArray> FullScreenQuadVA = nullptr;// Used from drawing background and post process frame
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

        int screen_width = window->GetWidth();
        int screen_height = window->GetHeight();

        FramebufferSpecification frame_spec = {};
        frame_spec.Width = screen_width;
        frame_spec.Height = screen_height;
        frame_spec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::DEPTH24STENCIL8 };
        s_Props.RendererInstance->m_SceneBuffer = Framebuffer::Create(frame_spec);

        s_Props.RendererInstance->m_ShaderLibrary.Load("sprite", "assets/shaders/sprite");
        s_Props.RendererInstance->m_ShaderLibrary.Load("background_shader", "assets/shaders/background");
        s_Props.RendererInstance->m_ShaderLibrary.Load("default_shader", "assets/shaders/postprocess/no_effects");
        s_Props.RendererInstance->m_ShaderLibrary.Load(
          "chromatic_shader", "assets/shaders/postprocess/chromatic_aberration");
        s_Props.RendererInstance->m_ShaderLibrary.Load("grayscale_shader", "assets/shaders/postprocess/grayscale");
        s_Props.RendererInstance->m_ShaderLibrary.Load("vignette_shader", "assets/shaders/postprocess/vignette");
        s_Props.RendererInstance->m_ShaderLibrary.Load("pixelation_shader", "assets/shaders/postprocess/pixelation");
        s_Props.RendererInstance->m_ShaderLibrary.Load("sephia_shader", "assets/shaders/postprocess/sephia");
        s_Props.RendererInstance->m_ShaderLibrary.Load("invert_shader", "assets/shaders/postprocess/invert");
        s_Props.RendererInstance->m_ShaderLibrary.Load("blur_shader", "assets/shaders/postprocess/blur");
        s_Props.RendererInstance->m_ShaderLibrary.Load("sharpen_shader", "assets/shaders/postprocess/sharpen");

        s_Props.RendererInstance->m_PostProcessEffects[PostProcessEffect::None] = "default_shader";
        s_Props.RendererInstance->m_PostProcessEffects[PostProcessEffect::Chromatic] = "chromatic_shader";
        s_Props.RendererInstance->m_PostProcessEffects[PostProcessEffect::Grayscale] = "grayscale_shader";
        s_Props.RendererInstance->m_PostProcessEffects[PostProcessEffect::Sephia] = "sephia_shader";
        s_Props.RendererInstance->m_PostProcessEffects[PostProcessEffect::Invert] = "invert_shader";
        s_Props.RendererInstance->m_PostProcessEffects[PostProcessEffect::Vignette] = "vignette_shader";
        s_Props.RendererInstance->m_PostProcessEffects[PostProcessEffect::Pixelation] = "pixelation_shader";
        s_Props.RendererInstance->m_PostProcessEffects[PostProcessEffect::Blur] = "blur_shader";
        s_Props.RendererInstance->m_PostProcessEffects[PostProcessEffect::Sharpen] = "sharpen_shader";


        const uint8_t blackPixel[4] = { 0, 0, 0, 255 };
        s_Props.DefaultBackground = std::make_unique<Texture>(reinterpret_cast<const char *>(blackPixel), 1, 1, 4);

        std::vector<float> fullscreenQuadVertices;
        std::vector<uint32_t> indices;

        CreateQuad(fullscreenQuadVertices, 1.f, indices);
        auto vertex_buffer = VertexBuffer::Create((uint32_t)(fullscreenQuadVertices.size()) * sizeof(float));
        vertex_buffer->SetLayout({ { ShaderDataType::Float2, "aPos" }, { ShaderDataType::Float2, "aTexCoord" } });
        vertex_buffer->SetData(
          fullscreenQuadVertices.data(), (uint32_t)(fullscreenQuadVertices.size()) * sizeof(float));
        auto index_buffer = IndexBuffer::Create(indices.data(), (uint32_t)(indices.size()));

        auto vertex_array = VertexArray::Create();
        vertex_array->SetVertexBuffer(vertex_buffer);
        vertex_array->SetIndexBuffer(index_buffer);
        s_Props.FullScreenQuadVA = vertex_array;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(MessageCallback, nullptr);

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif
    }

    Renderer::Renderer() {}

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
        s_Props.RendererInstance->m_SceneBuffer->Bind();
        glClearColor(s_Props.RendererInstance->m_BackgroundColor.r,
          s_Props.RendererInstance->m_BackgroundColor.g,
          s_Props.RendererInstance->m_BackgroundColor.b,
          1.f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        s_Props.RendererInstance->m_SceneBuffer->Unbind();
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

        s_Props.RendererInstance->m_CurrentActiveCamera = read_buffer->FrameCamera;
        read_buffer->ValidFrame = true;
        if (s_Props.ResizeWindow) {
            Window &window = Game::Get()->GetWindow();
            int screen_width = window.GetWidth();
            int screen_height = window.GetHeight();
            SOF_TRACE("Renderer", "Screen resize detected, current size [{0}, {1}]", screen_width, screen_height);
            read_buffer->FrameCamera->SetWidth((float)screen_width);
            read_buffer->FrameCamera->SetHeight((float)screen_height);
            glViewport(0, 0, (GLsizei)screen_width, (GLsizei)screen_height);
            s_Props.RendererInstance->m_SceneBuffer->Resize(screen_width, screen_height);
            s_Props.ResizeWindow = false;
        }

        ClearScreen();
    }

    void Renderer::DrawBackground()
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
        auto read_buffer = Game::Get()->GetRenderingThread().GetReadBuffer();
        if (!read_buffer->Background || !read_buffer->ValidFrame) { return; }

        auto program = s_Props.RendererInstance->m_ShaderLibrary.Get("background_shader");
        read_buffer->Background->Bind(0);
        s_Props.FullScreenQuadVA->Bind();
        program->Activate();
        s_Props.RendererInstance->m_SceneBuffer->Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        s_Props.FullScreenQuadVA->Unbind();
        s_Props.RendererInstance->m_SceneBuffer->Unbind();
    }

    void Renderer::DrawFinalFrame()
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
        auto read_buffer = Game::Get()->GetRenderingThread().GetReadBuffer();
        if (!read_buffer->ValidFrame) { return; }
        uint32_t scene_texture = s_Props.RendererInstance->m_SceneBuffer->GetColorAttachmentRendererID();
        auto program = s_Props.RendererInstance->m_ShaderLibrary.Get(
          s_Props.RendererInstance->m_PostProcessEffects[s_Props.RendererInstance->m_SelectedPostProcessingEffect]);
        s_Props.FullScreenQuadVA->Bind();
        Texture::Bind(scene_texture, 0);
        program->Activate();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        s_Props.FullScreenQuadVA->Unbind();
    }

    void Renderer::DrawObjects()
    {
        SOF_PROFILE_FUNC();
        SOF_PROFILE_GPU("Draw objects");
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
        auto read_buffer = Game::Get()->GetRenderingThread().GetReadBuffer();
        if (!read_buffer->ValidFrame) { return; }

        for (const auto &[layer, textureBatchMap] : read_buffer->CurrentBatch) {
            for (const auto &[texture, buffers] : textureBatchMap) {
                if (buffers.QuadBuffer.size() > 0) {
                    auto vertex_array = VertexArray::Create();

                    auto vertex_buffer = VertexBuffer::Create((uint32_t)buffers.QuadBuffer.size() * sizeof(Vertex));
                    vertex_buffer->SetLayout({ { ShaderDataType::Float4, "aPos" },
                      { ShaderDataType::Float4, "aColor" },
                      { ShaderDataType::Float2, "aTex" },
                      { ShaderDataType::Float2, "aSpriteSize" },
                      { ShaderDataType::Float2, "aSegments" },
                      { ShaderDataType::Float, "aTileIndexOffset" } });
                    vertex_buffer->SetData(
                      buffers.QuadBuffer.data(), (uint32_t)buffers.QuadBuffer.size() * sizeof(Vertex));

                    auto index_buffer =
                      IndexBuffer::Create(buffers.QuadIndices.data(), (uint32_t)buffers.QuadIndices.size());

                    vertex_array->SetVertexBuffer(vertex_buffer);
                    vertex_array->SetIndexBuffer(index_buffer);

                    ShaderStorageBuffer tile_indices{ buffers.TileIndices.size() * 2 * sizeof(float), 0 };
                    tile_indices.SetData(buffers.TileIndices.data(), buffers.TileIndices.size() * 2 * sizeof(float));

                    vertex_array->Bind();
                    auto program = s_Props.RendererInstance->m_ShaderLibrary.Get("sprite");

                    program->Set("u_ViewMatrix", s_Props.RendererInstance->m_CurrentActiveCamera->GetViewMatrix());
                    program->Set(
                      "u_ProjectionMatrix", s_Props.RendererInstance->m_CurrentActiveCamera->GetProjectionMatrix());
                    program->Set("u_UsingTexture", texture != nullptr);
                    program->Activate();

                    if (texture != nullptr) { texture->Bind(0); }
                    s_Props.RendererInstance->m_SceneBuffer->Bind();
                    glDrawElements(GL_TRIANGLES, (GLsizei)buffers.QuadIndices.size(), GL_UNSIGNED_INT, 0);
                    s_Props.RendererInstance->m_SceneBuffer->Unbind();
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

    void Renderer::SubmitSquare(SpriteComponent *sprite_comp, const glm::mat4 &transform)
    {
        SOF_PROFILE_FUNC();
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");

        auto write_buffer = Game::Get()->GetRenderingThread().GetWriteBuffer();

        glm::vec2 texture_size(sprite_comp->TextureRef->GetWidth(), sprite_comp->TextureRef->GetHeight());
        glm::vec2 normalized_sprite_size = sprite_comp->SpriteSize / texture_size;
        float tile_indices_offset =
          write_buffer->CurrentBatch[sprite_comp->Layer][sprite_comp->TextureRef.get()].TimeIndexPtr;

        write_buffer->CurrentBatch[sprite_comp->Layer][sprite_comp->TextureRef.get()].QuadBuffer.push_back(
          { transform * glm::vec4(0.5f, 0.5f, 0.0f, 1.f),
            sprite_comp->Color,
            glm::vec2(1.0f, 1.0f),
            normalized_sprite_size,
            sprite_comp->GetTiles(),
            tile_indices_offset });
        write_buffer->CurrentBatch[sprite_comp->Layer][sprite_comp->TextureRef.get()].QuadBuffer.push_back(
          { transform * glm::vec4(0.5f, -0.5f, 0.0f, 1.f),
            sprite_comp->Color,
            glm::vec2(1.0f, 0.0f),
            normalized_sprite_size,
            sprite_comp->GetTiles(),
            tile_indices_offset });
        write_buffer->CurrentBatch[sprite_comp->Layer][sprite_comp->TextureRef.get()].QuadBuffer.push_back(
          { transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.f),
            sprite_comp->Color,
            glm::vec2(0.0f, 0.0f),
            normalized_sprite_size,
            sprite_comp->GetTiles(),
            tile_indices_offset });
        write_buffer->CurrentBatch[sprite_comp->Layer][sprite_comp->TextureRef.get()].QuadBuffer.push_back(
          { transform * glm::vec4(-0.5f, 0.5f, 0.0f, 1.f),
            sprite_comp->Color,
            glm::vec2(0.0f, 1.0f),
            normalized_sprite_size,
            sprite_comp->GetTiles(),
            tile_indices_offset });

        write_buffer->CurrentBatch[sprite_comp->Layer][sprite_comp->TextureRef.get()].QuadIndices.push_back(
          write_buffer->CurrentBatch[sprite_comp->Layer][sprite_comp->TextureRef.get()].IndexPtr);
        write_buffer->CurrentBatch[sprite_comp->Layer][sprite_comp->TextureRef.get()].QuadIndices.push_back(
          write_buffer->CurrentBatch[sprite_comp->Layer][sprite_comp->TextureRef.get()].IndexPtr + 1);
        write_buffer->CurrentBatch[sprite_comp->Layer][sprite_comp->TextureRef.get()].QuadIndices.push_back(
          write_buffer->CurrentBatch[sprite_comp->Layer][sprite_comp->TextureRef.get()].IndexPtr + 3);
        write_buffer->CurrentBatch[sprite_comp->Layer][sprite_comp->TextureRef.get()].QuadIndices.push_back(
          write_buffer->CurrentBatch[sprite_comp->Layer][sprite_comp->TextureRef.get()].IndexPtr + 1);
        write_buffer->CurrentBatch[sprite_comp->Layer][sprite_comp->TextureRef.get()].QuadIndices.push_back(
          write_buffer->CurrentBatch[sprite_comp->Layer][sprite_comp->TextureRef.get()].IndexPtr + 2);
        write_buffer->CurrentBatch[sprite_comp->Layer][sprite_comp->TextureRef.get()].QuadIndices.push_back(
          write_buffer->CurrentBatch[sprite_comp->Layer][sprite_comp->TextureRef.get()].IndexPtr + 3);

        write_buffer->CurrentBatch[sprite_comp->Layer][sprite_comp->TextureRef.get()].IndexPtr += 4;
        write_buffer->CurrentBatch[sprite_comp->Layer][sprite_comp->TextureRef.get()].TimeIndexPtr +=
          sprite_comp->GetAllCoordinates().size();
        for (auto index : sprite_comp->GetAllCoordinates()) {
            write_buffer->CurrentBatch[sprite_comp->Layer][sprite_comp->TextureRef.get()].TileIndices.push_back(index);
        }

        s_Props.RendererInstance->m_Stats.QuadsDrawn++;
    }


    void Renderer::SubmitTriangle() { SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized"); }
    void Renderer::DrawFrame()
    {
        BeginFrame();
        DrawBackground();
        DrawObjects();
        DrawFinalFrame();
        EndFrame();
    }

    void Renderer::CreateQuad(std::vector<float> &vertex_buffer, float scale, std::vector<uint32_t> &index_buffer)
    {
        float quadVertices[] = {
            // Positions    // Texture Coords
            -1.0f,
            1.0f,
            0.0f,
            1.0f,// Top-left
            -1.0f,
            -1.0f,
            0.0f,
            0.0f,// Bottom-left
            1.0f,
            -1.0f,
            1.0f,
            0.0f,// Bottom-right
            1.0f,
            1.0f,
            1.0f,
            1.0f// Top-right
        };

        // Define the default indices for the quad (two triangles)
        const uint32_t quadIndices[] = {
            0,
            1,
            2,// First triangle
            0,
            2,
            3// Second triangle
        };
        vertex_buffer.resize(sizeof(quadVertices) / sizeof(float));
        index_buffer.resize(sizeof(quadIndices) / sizeof(uint32_t));
        std::copy(std::begin(quadVertices), std::end(quadVertices), vertex_buffer.begin());
        std::copy(std::begin(quadIndices), std::end(quadIndices), index_buffer.begin());
    }

    void Renderer::ChangeBackgroundColor(const glm::vec3 &color)
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");

        s_Props.RendererInstance->m_BackgroundColor = color;
    }

    void Renderer::SubmitBackgroundTexture(Texture *background)
    {
        auto write_buffer = Game::Get()->GetRenderingThread().GetWriteBuffer();
        write_buffer->Background = background;
    }

    void Renderer::SetPostProcessEffect(PostProcessEffect effect)
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
        auto it = s_Props.RendererInstance->m_PostProcessEffects.find(effect);
        if (it != s_Props.RendererInstance->m_PostProcessEffects.end()) {
            s_Props.RendererInstance->m_SelectedPostProcessingEffect = effect;
            return;
        }
        SOF_WARN("Renderer", "This post process effect does not exist");
    }

    std::unordered_map<PostProcessEffect, std::string> &Renderer::GetAllPostProcessingEffects()
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
        return s_Props.RendererInstance->m_PostProcessEffects;
    }

    std::string &Renderer::GetSelectedPostProcessingEffectValue()
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
        return s_Props.RendererInstance->m_PostProcessEffects[s_Props.RendererInstance->m_SelectedPostProcessingEffect];
    }

    PostProcessEffect &Renderer::GetSelectedPostProcessingEffect()
    {
        SOF_ASSERT(s_Props.RendererInstance, "Renderer not initialized");
        return s_Props.RendererInstance->m_SelectedPostProcessingEffect;
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
