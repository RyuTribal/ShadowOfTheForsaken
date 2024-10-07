#pragma once

#include "StateTransition.h"

namespace SOF
{
    class StateMachine
    {
        public:
        StateMachine(std::function<void(StateMachine *, float)> on_update_callback)
          : m_OnUpdateCallback(on_update_callback)
        {}
        template<typename T> void SetVariable(const std::string &name, T value) { m_Variables[name] = value; }
        template<typename T> T GetVariable(const std::string &name)
        {
            auto it = m_Variables.find(name);
            if (it != m_Variables.end()) { return std::any_cast<T>(m_Variables.at(name)); }
        }

        void Update(float dt);

        UUID AddState(const std::string &name, std::shared_ptr<Animation> anim_instance, bool looping, bool starting);
        void AddTransition(UUID from, UUID to, std::function<bool(StateMachine *)> condition);

        const std::pair<uint32_t, uint32_t> &GetReleventSpriteIndex();


        private:
        void TransitionState(UUID new_state);

        private:
        std::function<void(StateMachine *, float)> m_OnUpdateCallback = nullptr;
        std::unordered_map<std::string, std::any> m_Variables;
        std::unordered_map<UUID, std::unique_ptr<State>> m_States;
        std::vector<std::unique_ptr<StateTransition>> m_Transitions;
        std::vector<StateTransition *> m_AvailibleTransitions;
        UUID m_CurrentActiveState = 0;
    };
}// namespace SOF