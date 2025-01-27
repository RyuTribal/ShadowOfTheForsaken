#pragma once
#include "Engine/Events/ApplicationEvents.h"
#include "Window.h"
#include "Engine/ImGui/ImGuiLayer.h"
#include "Thread.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Scene/Scene.h"
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
            DeltaTime = (float)frameTime;
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
        // Overwrite these:
        virtual void OnGameStart(){};
        virtual void OnGameShutdown(){};
        virtual void OnGameUpdate(float delta_time){};
        virtual void OnDebugUpdate(float delta_time){};
        virtual void OnGameEvent(Event &event){};

        public:
        Game(const Window::WindowData &props);
        void Start();
        bool OnShutDown(WindowCloseEvent &event);
        bool OnWindowResize(WindowResizeEvent &event);

        void OnEvent(Event &event);

        static Game *Get() { return s_Instance; }

        Window &GetWindow();
        Thread<RenderBufferData> &GetRenderingThread() { return m_RendererThread; }

        const ThreadData &GetThreadData() { return m_ThreadData; }

        UUID SubscribeOnEvents(std::function<void(Event &)> callback);
        void RevokeSubscription(UUID subscriber);

        FrameStats &GetFrameStats() { return m_FrameStats; }

        private:
        private:
        bool m_Running = true;
        ThreadData m_ThreadData;
        Thread<RenderBufferData> m_RendererThread;
        Window m_Window;
        static Game *s_Instance;
        std::unordered_map<UUID, std::function<void(Event &)>> m_Subscribers{};
        FrameStats m_FrameStats{};
        ThreadPool m_ThreadPool{};
    };
    Game *CreateGame(int argc, char **argv);
}// namespace SOF
