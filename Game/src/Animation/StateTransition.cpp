#include "pch.h"
#include "StateTransition.h"
#include "StateMachine.h"

namespace SOF
{
    StateTransition::StateTransition(State *from, State *to, std::function<bool(StateMachine *)> transition_condition)
      : m_From(from), m_To(to), m_Condition(transition_condition)
    {}
    bool StateTransition::Update(float dt, StateMachine *state_machine)
    {
        if (m_From && m_From->IsActive()) { m_From->Update(dt); }
        if (m_Condition(state_machine)) {
            m_From->Exit();
            m_To->Enter();
            return true;
        }

        if (m_From->IsActive() && m_From->IsFinished()) {
            m_From->Exit();
            if (m_From->IsLooping()) {
                m_From->Enter();
                return false;
            }
            return true;
        }

        return false;
    }
}// namespace SOF