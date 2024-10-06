#pragma once

#include "State.h"

namespace SOF
{
    class StateMachine;
    class StateTransition
    {
        public:
        StateTransition(State *from, State *to, std::function<bool(StateMachine *)> transition_condition);

        bool Update(float dt, StateMachine *state_machine);
        bool IsActive() { return m_From->IsActive(); }

        State *GetTo() { return m_To; }
        State *GetFrom() { return m_From; }

        UUID GetID() { return m_ID; }

        private:
        UUID m_ID;
        State *m_From = nullptr;
        State *m_To = nullptr;
        std::function<bool(StateMachine *)> m_Condition = nullptr;
    };
}// namespace SOF