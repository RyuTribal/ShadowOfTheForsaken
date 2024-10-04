#pragma once

#include <Engine/Engine.h>
#include "Debug/DebugWindow.h"

namespace SOF
{
    class SOFGame : public Game
    {
        public:
        SOFGame(Window::WindowData props) : Game(props) {}

        ~SOFGame() {}

        virtual void OnGameStart() override;
        virtual void OnGameShutdown() override;
        virtual void OnGameUpdate(float delta_time) override;
        virtual void OnDebugUpdate() override;
        virtual void OnGameEvent(Event &event) override;

        void HandleMovement();

        bool OnKeyPressedEvent(KeyPressedEvent &event);

        private:
        std::shared_ptr<Scene> m_Scene;
        UUID m_WarsayID;
        std::vector<UUID> m_WarsayHome{};
        ThreadPool m_ThreadPool{};
        DebugWindow m_DebugWindow{};
        float m_WarsaySpeed = 100.f;
    };
}// namespace SOF