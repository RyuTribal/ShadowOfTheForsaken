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
            std::shared_ptr<Animation> IdleLeft = nullptr;
            std::shared_ptr<Animation> IdleRight = nullptr;
            std::shared_ptr<Animation> IdleUp = nullptr;
            std::shared_ptr<Animation> IdleDown = nullptr;

            std::shared_ptr<Animation> WalkLeft = nullptr;
            std::shared_ptr<Animation> WalkRight = nullptr;
            std::shared_ptr<Animation> WalkUp = nullptr;
            std::shared_ptr<Animation> WalkDown = nullptr;
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
