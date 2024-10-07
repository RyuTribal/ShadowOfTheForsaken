#include "pch.h"
#include "PlayerController.hpp"
#include "Game.h"
namespace SOF
{
    PlayerController::PlayerController(float speed) : m_Velocity{ speed } {}
    void PlayerController::UpdateMovement(Entity *context)
    {

        if (!DebugWindow::ShouldCaptureInput()) { return; }

        if (!context) { return; }
        bool any_movement = Input::IsKeyPressed(GLFW_KEY_W) || Input::IsKeyPressed(GLFW_KEY_S)
                            || Input::IsKeyPressed(GLFW_KEY_A) || Input::IsKeyPressed(GLFW_KEY_D);
        if (any_movement) {
            glm::vec3 velocity = { 0.f, 0.f, 0.f };
            if (Input::IsKeyPressed(GLFW_KEY_W)) { velocity.y += m_Velocity; }
            if (Input::IsKeyPressed(GLFW_KEY_S)) { velocity.y -= m_Velocity; }
            if (Input::IsKeyPressed(GLFW_KEY_A)) { velocity.x -= m_Velocity; }
            if (Input::IsKeyPressed(GLFW_KEY_D)) { velocity.x += m_Velocity; }
            if (glm::length(velocity) > 0.0f) { velocity = glm::normalize(velocity); }
            context->GetScene()->GetPhysicsWorld()->SetVelocity(context, velocity, VelocityType::Linear);
        }
    }
}// namespace SOF
