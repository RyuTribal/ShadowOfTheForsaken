#pragma once
#include "CharacterController.hpp"
namespace SOF
{
    class PlayerController : public CharacterController
    {
        public:
        PlayerController(float m_speed);
        ~PlayerController() override = default;
        void UpdateMovement(Entity *context, float dt) override;

        private:
        float m_Velocity;
    };
}// namespace SOF
