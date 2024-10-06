#include "pch.h"
#include "State.h"

namespace SOF
{
    State::State(UUID id, const std::string &name, const glm::vec2 &sprite_index, float duration, bool looping)
      : m_ID(id), m_Name(name), m_SpriteIndex(sprite_index), m_Duration(duration), m_Looping(looping)
    {}
    void State::Update(float dt)
    {
        if (!m_IsActive) {
            SOF_WARN("State", "This state is not active");
            return;
        }

        if (m_OnUpdate) { m_OnUpdate(dt); }

        m_Timer += dt;
    }

    void State::Enter()
    {
        if (m_OnEnter) { m_OnEnter(); }
        m_IsActive = true;
    }
    void State::Exit()
    {
        if (m_OnExit) { m_OnExit(); }
        m_Timer = 0.0f;
        m_IsActive = false;
    }
}// namespace SOF