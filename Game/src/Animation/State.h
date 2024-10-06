#pragma once

#include <Engine/Engine.h>

namespace SOF
{
    class State
    {
        public:
        State(UUID id, const std::string &name, const glm::vec2 &sprite_index, float duration, bool looping);

        void SetOnEnterCallback(std::function<void()> callback) { m_OnEnter = callback; }
        void SetOnUpdateCallback(std::function<void(float dt)> callback) { m_OnUpdate = callback; }
        void SetOnExitCallback(std::function<void()> callback) { m_OnExit = callback; }

        bool IsActive() { return m_IsActive; }
        bool IsFinished() { return m_Timer >= m_Duration; }
        bool IsLooping() { return m_Looping; }
        UUID GetID() { return m_ID; }

        const glm::vec2 &GetSpriteIndex() { return m_SpriteIndex; }

        void Update(float dt);

        void Enter();
        void Exit();

        private:
        UUID m_ID = 0;
        glm::vec2 m_SpriteIndex = { 0.0f, 0.0f };
        float m_Duration = 0.0f;
        float m_Timer = 0.0f;
        bool m_Looping = false;
        bool m_IsActive = false;
        std::function<void()> m_OnEnter = nullptr;
        std::function<void(float dt)> m_OnUpdate = nullptr;
        std::function<void()> m_OnExit = nullptr;
        std::string m_Name = "State";
    };
}// namespace SOF