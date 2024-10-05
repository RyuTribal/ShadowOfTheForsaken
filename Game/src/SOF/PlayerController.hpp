#pragma ONCE
#include <Engine/Engine.h>
#include "CharacterController.hpp"
namespace SOF
{
    class PlayerController : public CharacterController
    {
        public:
        PlayerController(float m_speed);
        ~PlayerController() override = default;
        void UpdateMovement(UUID, std::shared_ptr<Scene> scene) override;

        private:
        float m_velocity;
    };
}// namespace SOF
