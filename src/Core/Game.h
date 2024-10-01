#pragma once
#include "Events/ApplicationEvents.h"
#include "Window.h"
#include "ImGui/ImGuiLayer.h"
#include "Thread.h"
#include "Renderer/Renderer.h"
#include <Scene/Scene.h>
#include "ImGui/DebugWindow.h"
#include "ThreadPool.h"

namespace SOF
{

    class Window;

    struct FrameStats
    {
        float DeltaTime = 0.0f;
        double FPS = 0.0;
        double FrameTimeAccumulator = 0.0;
        int FrameCount = 0;
        double LastFPSCalculationTime = 0.0;

        void UpdateFPS(double currentTime, double frameTime)
        {
            DeltaTime = frameTime;
            FrameTimeAccumulator += frameTime;
            FrameCount++;

            if (currentTime - LastFPSCalculationTime >= 1.0) {
                double avgFrameTime = FrameTimeAccumulator / FrameCount;
                FPS = 1.0 / avgFrameTime;

                FrameTimeAccumulator = 0.0;
                FrameCount = 0;
                LastFPSCalculationTime = currentTime;
            }
        }
    };

    class Game
    {
        public:
        static Game *CreateGame(const Window::WindowData &props);

        void Start();
        bool OnShutDown(WindowCloseEvent &event);
        bool OnWindowResize(WindowResizeEvent &event);
        bool OnKeyPressed(KeyPressedEvent &event);

        void OnEvent(Event &event);

        static Game *Get() { return s_Instance; }

        Window &GetWindow();
        Thread<RenderBufferData> &GetRenderingThread() { return m_RendererThread; }

        UUID SubscribeOnEvents(std::function<void(Event &)> callback);
        void RevokeSubscription(UUID subscriber);

        FrameStats &GetFrameStats() { return m_FrameStats; }

        Scene *GetCurrentScene() { return m_Scene.get(); }

        private:
        Game(const Window::WindowData &props);

        private:
        bool m_Running = true;
        ThreadData m_ThreadData;
        Thread<RenderBufferData> m_RendererThread;
        Window m_Window;
        static Game *s_Instance;
        std::unordered_map<UUID, std::function<void(Event &)>> m_Subscribers{};
        std::shared_ptr<Scene> m_Scene;
        FrameStats m_FrameStats{};
        ThreadPool m_ThreadPool{};

        UUID m_WarsayID;
        std::vector<UUID> m_WarsayHome{};
        DebugWindow m_DebugWindow{};
    };
}// namespace SOF
