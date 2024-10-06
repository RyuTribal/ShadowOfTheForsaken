#pragma once

#include "PlayerController.hpp"
#include "Animation/StateMachine.h"

namespace SOF
{

    class Player
    {
        public:
        struct LocomotionSettings
        {
            glm::vec2 IdleLeft = { 0.0f, 0.0f };
            glm::vec2 IdleRight = { 0.0f, 0.0f };
            glm::vec2 IdleUp = { 0.0f, 0.0f };
            glm::vec2 IdleDown = { 0.0f, 0.0f };

            glm::vec2 WalkLeft = { 0.0f, 0.0f };
            glm::vec2 WalkRight = { 0.0f, 0.0f };
            glm::vec2 WalkUp = { 0.0f, 0.0f };
            glm::vec2 WalkDown = { 0.0f, 0.0f };
        };
        static constexpr uint32_t CharacterLayer = 100;
        Player(const std::string &name,
          const std::string &sprite_sheet,
          const glm::vec2 &sprite_size,
          float speed,
          Scene *context);
        ~Player() = default;

        void Update(float dt);
        void CreateLocomotionStateMachine(const LocomotionSettings &settings);
        UUID GetID();

        private:
        void OnLocomotionUpdate(StateMachine *machine, float dt);

        private:
        std::unique_ptr<CharacterController> m_PlayerController;
        std::optional<StateMachine> m_LocomotionStateMachine;
        Entity *m_Entity;
    };

}// namespace SOF
