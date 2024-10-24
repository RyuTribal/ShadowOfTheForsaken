#include "pch.h"
#include "Game.h"
#include "Engine/Asset/Manager.h"
#include "Engine/Events/DebugEvents.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Sound/SoundEngine.h"
#include "Engine/Physics/PhysicsEngine.h"
#include "Engine/Core/Profiler.h"
#include "Engine/UI/UI.h"


namespace SOF
{

    Game *Game::s_Instance = nullptr;

    Game::Game(const Window::WindowData &props) : m_RendererThread("Render Thread"), m_Window(props)
    {
        SOF_ASSERT(!s_Instance, "Can only have one instance of a game!");
        SOF_INFO("Game instance", "Game starting...");
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
        SoundEngine::Init();
        PhysicsEngine::Init();
        AssetManager::Init("Assets.sofp");
        UI::Init();
    }

    void Game::Start()
    {
        OnGameStart();
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

            UI::BeginFrame();
            OnGameUpdate(frameTime);
            UI::EndFrame();

            // m_RendererThread.WaitForAllTasks();

#ifdef DEBUG
            ImGuiLayer::Begin();
            ImGuiUpdateEvent debug_event{};
            OnEvent(debug_event);

            OnDebugUpdate(frameTime);
            ImGuiLayer::End();
#endif
            m_Window.OnUpdate();
            m_RendererThread.Run(&Renderer::SwapBuffers);
            /*m_RendererThread.WaitForAllTasks();*/
            m_RendererThread.SwapBuffers();

            SOF_PROFILE_MARK_FRAME;
        }
    }

    bool Game::OnShutDown(WindowCloseEvent &event)
    {
        SOF_INFO("Game", "Shutting down \n");
        m_Running = false;
        OnGameShutdown();

#ifdef DEBUG
        m_RendererThread.Run(&ImGuiLayer::Shutdown);
#endif
        m_RendererThread.WaitForAllTasks();
        SoundEngine::Shutdown();
        AssetManager::Shutdown();
        PhysicsEngine::Shutdown();
        UI::Shutdown();
        return true;
    }

    bool Game::OnWindowResize(WindowResizeEvent &event)
    {
        m_RendererThread.Run(&Renderer::ResizeWindow);
        return true;
    }

    void Game::OnEvent(Event &event)
    {
        EventDispatcher dispatcher(event);
        UI::OnWindowEvent(event);
        OnGameEvent(event);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(SOF::Game::OnShutDown));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(SOF::Game::OnWindowResize));

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
