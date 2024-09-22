#include "pch.h"
#include "Game.h"
#include <glad/gl.h>
#include <imgui.h>
#include "Asset/Manager.h"
#include "Events/DebugEvents.h"
#include "Renderer/Renderer.h"
#include "Renderer/Texture.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"


namespace SOF
{

    Game *Game::s_Instance = nullptr;

    Game::Game(const WindowProps &props) : m_RendererThread("Render Thread"), m_Window(props)
    {
        SOF_ASSERT(!s_Instance, "Can only have one instance of a game!");
        m_RendererThread.SetShutdownTask(&Renderer::Shutdown);
        m_ThreadData.MainThread = std::this_thread::get_id();
        s_Instance = this;
        m_Window.SetEventCallback(BIND_EVENT_FN(SOF::Game::OnEvent));
        m_RendererThread.Run(&Renderer::Init, &m_Window);
        m_RendererThread.Run(Renderer::SetVSync, false);
#ifdef DEBUG
        ImGuiLayer::Init();
#endif
        m_RendererThread.WaitForAllTasks();
        AssetManager::Init("Assets.sofp");

        m_Scene = std::make_shared<Scene>("Test scene");

        // cReating warsay
        m_WarsayID = m_Scene->CreateEntity("WarsayBox");
        std::string warsay_asset_handle = "warsay_sprite";
        auto warsay_entity = m_Scene->GetEntity(m_WarsayID);
        auto warsay_texture = m_ThreadPool.AddTask(AssetManager::Load<Texture>, warsay_asset_handle).get();
        TransformComponent warsay_transform = TransformComponent();
        SpriteComponent warsay_sprite = SpriteComponent(glm::vec4(1.f, 0.f, 0.f, 1.f));
        warsay_sprite.Texture = warsay_texture;
        CameraComponent warsay_camera = CameraComponent(true);
        warsay_camera.Camera = Camera::Create(m_Window.GetWidth(), m_Window.GetHeight());
        warsay_entity->AddComponent<TransformComponent>(warsay_transform);
        warsay_entity->AddComponent<SpriteComponent>(warsay_sprite);
        warsay_entity->AddComponent<CameraComponent>(warsay_camera);

        // cReating warsay home
        int gridWidth = 100;
        int gridHeight = 100;
        float spacing = 1.0f;

        for (int x = 0; x < gridWidth; ++x) {
            for (int y = 0; y < gridHeight; ++y) {
                UUID entityID = m_Scene->CreateEntity("GridEntity_" + std::to_string(x) + "_" + std::to_string(y));
                auto entity = m_Scene->GetEntity(entityID);
                m_WarsayHome.push_back(entityID);
                TransformComponent transform;
                transform.Translation = glm::vec3(x * spacing, y * spacing, 0.0f);
                transform.Scale = glm::vec3(0.9f);

                SpriteComponent sprite;
                sprite.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                sprite.Texture = warsay_texture;

                entity->AddComponent<TransformComponent>(transform);
                entity->AddComponent<SpriteComponent>(sprite);
            }
        }
    }

    Game *Game::CreateGame(const WindowProps &props) { return new Game(props); }

    void Game::Start()
    {


        auto last_frame = std::chrono::high_resolution_clock::now();
        while (m_Running) {

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - last_frame).count();
            last_frame = newTime;
            double currentTime = std::chrono::duration<double>(newTime.time_since_epoch()).count();
            m_FrameStats.UpdateFPS(currentTime, frameTime);

            m_RendererThread.WaitForAllTasks();
            m_RendererThread.GetWriteBuffer()->Clear();

            m_RendererThread.Run(&Renderer::DrawFrame);

            m_Scene->Begin();
            m_Scene->Update();
            m_Scene->End();

            m_RendererThread.WaitForAllTasks();

#ifdef DEBUG
            ImGuiLayer::Begin();
            ImGuiUpdateEvent debug_event{};
            OnEvent(debug_event);

            m_DebugWindow.Render();
            ImGuiLayer::End();
#endif

            m_Window.OnUpdate();
            m_RendererThread.Run(&Renderer::SwapBuffers);
            m_RendererThread.SwapBuffers();
        }
    }

    bool Game::OnShutDown(WindowCloseEvent &event)
    {
        SOF_INFO("Game", "Shutting down \n");
        m_Running = false;

        m_Scene = nullptr;// So every entity with their asset is deleted

#ifdef DEBUG
        m_RendererThread.Run(&ImGuiLayer::Shutdown);
#endif
        AssetManager::Shutdown();
        return true;
    }

    bool Game::OnWindowResize(WindowResizeEvent &event)
    {
        m_RendererThread.Run(&Renderer::ResizeWindow);
        return true;
    }

    bool Game::OnKeyPressed(KeyPressedEvent &event)
    {
        glm::vec3 velocity = { 0.f, 0.f, 0.f };
        if (event.GetKeyCode() == GLFW_KEY_W) { velocity.y += 10.f * m_FrameStats.DeltaTime; }
        if (event.GetKeyCode() == GLFW_KEY_S) { velocity.y -= 10.f * m_FrameStats.DeltaTime; }
        if (event.GetKeyCode() == GLFW_KEY_A) { velocity.x -= 10.f * m_FrameStats.DeltaTime; }
        if (event.GetKeyCode() == GLFW_KEY_D) { velocity.x += 10.f * m_FrameStats.DeltaTime; }

        auto write_buffer = m_RendererThread.GetWriteBuffer();

        if (write_buffer->FrameCamera) { write_buffer->FrameCamera->Move(velocity); }
        return true;
    }

    void Game::OnEvent(Event &event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(SOF::Game::OnShutDown));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(SOF::Game::OnWindowResize));
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(SOF::Game::OnKeyPressed));

        for (auto [subscriber, callback] : m_Subscribers) { callback(event); }
    }

    Window &Game::GetWindow() { return m_Window; }

    UUID Game::SubscribeOnEvents(std::function<void(Event &)> callback)
    {
        UUID subscriber = UUID();
        m_Subscribers[subscriber] = callback;
        return subscriber;
    }

    void Game::RevokeSubscription(UUID subscriber) { m_Subscribers.erase(subscriber); }
}// namespace SOF
