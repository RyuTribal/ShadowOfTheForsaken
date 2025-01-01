#include "pch.h"
#include "StateMachine.h"

namespace SOF
{
    void StateMachine::Update(float dt)
    {
        if (m_OnUpdateCallback) { m_OnUpdateCallback(this, dt); }
        if (!m_CurrentActiveState || m_AvailibleTransitions.size() < 1) { return; }

        for (StateTransition *transition : m_AvailibleTransitions) {
            if (transition && transition->Update(dt, this)) {
                TransitionState(transition->GetTo()->GetID());
                break;
            }
        }
    }
    UUID StateMachine::AddState(const std::string &name,
      std::shared_ptr<Animation> anim_instance,
      bool looping,
      bool starting)
    {
        UUID new_state_id;
        m_States[new_state_id] = std::make_unique<State>(new_state_id, name, anim_instance, looping);
        if (starting) { m_CurrentActiveState = new_state_id; }
        return new_state_id;
    }
    void StateMachine::AddTransition(UUID from, UUID to, std::function<bool(StateMachine *)> condition)
    {
        m_Transitions.push_back(std::make_unique<StateTransition>(m_States[from].get(), m_States[to].get(), condition));
        if (from == m_CurrentActiveState) {
            m_AvailibleTransitions.push_back(m_Transitions[m_Transitions.size() - 1].get());
        }
    }

    const std::pair<uint32_t, uint32_t> StateMachine::GetReleventSpriteIndex()
    {
        if (!m_CurrentActiveState || m_States.find(m_CurrentActiveState) == m_States.end()) {
            SOF_ERROR("StateMachine", "No current active state registered!");
            return {0, 0};
        }
        return m_States[m_CurrentActiveState]->GetSpriteIndex();
    }

    void StateMachine::TransitionState(UUID new_state)
    {
        m_CurrentActiveState = 0;
        m_AvailibleTransitions.clear();
        if (m_States[new_state]->IsActive()) {
            m_CurrentActiveState = new_state;
            for (auto &transition : m_Transitions) {
                if (transition->GetFrom()->GetID() == new_state) { m_AvailibleTransitions.push_back(transition.get()); }
            }
        }
    }
}// namespace SOF