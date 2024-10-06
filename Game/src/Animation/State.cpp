#include "pch.h"
#include "State.h"

namespace SOF
{
    State::State(UUID id, const std::string &name, std::shared_ptr<Animation> anim_instance, bool looping)
      : m_ID(id), m_Name(name), m_Animation(anim_instance), m_Duration(anim_instance->GetTotalDuration()),
        m_Looping(looping)
    {}
    void State::Update(float dt)
    {
        if (!m_IsActive) {
            SOF_WARN("State", "This state is not active");
            return;
        }

        if (m_OnUpdate) { m_OnUpdate(dt); }
        m_Animation->Update(dt);
        if (m_Animation->IsFinished() && m_Looping) { m_Animation->Reset(); }
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