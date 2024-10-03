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
#include "Sound/SoundEngine.h"


namespace SOF
{

    Game *Game::s_Instance = nullptr;

    Game::Game(const Window::WindowData &props) : m_RendererThread("Render Thread"), m_Window(props)
    {
        SOF_ASSERT(!s_Instance, "Can only have one instance of a game!");
        m_RendererThread.SetShutdownTask(&Renderer::Shutdown);
        m_ThreadData.MainThread = std::this_thread::get_id();
        s_Instance = this;
        m_Window.SetEventCallback(BIND_EVENT_FN(SOF::Game::OnEvent));
        m_RendererThread.Run(&Renderer::Init, &m_Window);
        m_RendererThread.Run(Renderer::SetVSync, props.VSync);
#ifdef DEBUG
        ImGuiLayer::Init();
#endif
        m_RendererThread.WaitForAllTasks();
        AssetManager::Init("Assets.sofp");
        SoundEngine::Init();

        m_Scene = std::make_shared<Scene>("Test scene");

        // Load textures
        std::string warsay_asset_handle = "black_idle";
        std::string grounds_asset_handle = "grounds";
        auto warsay_texture_promise = m_ThreadPool.AddTask(AssetManager::Load<Texture>, warsay_asset_handle);
        auto grounds_texture_promise = m_ThreadPool.AddTask(AssetManager::Load<Texture>, grounds_asset_handle);
        m_ThreadPool.Await();
        auto warsay_texture = warsay_texture_promise.get();
        auto grounds_texture = grounds_texture_promise.get();

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

                SpriteComponent sprite;
                sprite.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                sprite.Texture = grounds_texture;
                sprite.SpriteCoordinates = glm::vec2(3.f, 4.f);
                entity->AddComponent<TransformComponent>(transform);
                entity->AddComponent<SpriteComponent>(sprite);

                // if (x == gridWidth - 1 && y == gridHeight - 1) {
                if (x == 70 && y == 70) {
                    SoundComponent sound_comp;
                    sound_comp.Loop = true;
                    sound_comp.AssetHandle = "sound_test";
                    sound_comp.Type = SoundType::SPATIAL;
                    entity->AddComponent<SoundComponent>(sound_comp);
                    // Should not be here, but for testing it works
                    auto sound_ref = entity->GetComponent<SoundComponent>();
                    auto transform_ref = entity->GetComponent<TransformComponent>();
                    sound_ref->InstanceID = SoundEngine::PlayAudio(sound_ref, transform_ref->Translation);
                }
            }
        }

        // cReating warsay
        m_WarsayID = m_Scene->CreateEntity("WarsayBox");

        auto warsay_entity = m_Scene->GetEntity(m_WarsayID);
        TransformComponent warsay_transform = TransformComponent();
        SpriteComponent warsay_sprite = SpriteComponent(glm::vec4(1.f, 0.f, 0.f, 1.f));
        warsay_sprite.Texture = warsay_texture;
        warsay_sprite.SpriteSize = glm::vec2(128.f, 128.f);
        warsay_sprite.Layer = 1;
        CameraComponent warsay_camera = CameraComponent(true);
        warsay_camera.Camera = Camera::Create((float)m_Window.GetWidth(), (float)m_Window.GetHeight());
        Rigidbody2DComponent rigid_body{};
        rigid_body.Type = ColliderType::DYNAMIC;// Currently just falls so keep this commented
        CapsuleCollider2DComponent capsule_collider{};
        capsule_collider.Height = warsay_sprite.SpriteSize.y;
        warsay_entity->AddComponent<TransformComponent>(warsay_transform);
        warsay_entity->AddComponent<SpriteComponent>(warsay_sprite);
        warsay_entity->AddComponent<CameraComponent>(warsay_camera);
        warsay_entity->AddComponent<Rigidbody2DComponent>(rigid_body);
        warsay_entity->AddComponent<CapsuleCollider2DComponent>(capsule_collider);

        m_Scene->CreatePhysicsWorld();

        m_Scene->SetGravity({ 0.f, 0.f });// We won't really have a gravity in a jrpg like world
        m_Scene->SetListenerEntity(m_WarsayID);
    }

    Game *Game::CreateGame(const Window::WindowData &props) { return new Game(props); }

    void Game::Start()
    {
        m_Window.SetFullscreen(m_Window.GetFullScreen(), m_Window.GetFullScreenType());
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
        SoundEngine::Shutdown();
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
        if (m_DebugWindow.IsWindowActive()) { return false; }
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
