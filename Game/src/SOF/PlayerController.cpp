#include "pch.h"
#include "PlayerController.hpp"
namespace SOF
{
    PlayerController::PlayerController(float speed) : m_velocity{ speed } {}
    void PlayerController::UpdateMovement(UUID uid, std::shared_ptr<Scene> scene)
    {
        bool any_movement = Input::IsKeyPressed(GLFW_KEY_W) || Input::IsKeyPressed(GLFW_KEY_S)
                            || Input::IsKeyPressed(GLFW_KEY_A) || Input::IsKeyPressed(GLFW_KEY_D);
        if (any_movement) {
            glm::vec3 velocity = { 0.f, 0.f, 0.f };
            if (Input::IsKeyPressed(GLFW_KEY_W)) { velocity.y += m_velocity; }
            if (Input::IsKeyPressed(GLFW_KEY_S)) { velocity.y -= m_velocity; }
            if (Input::IsKeyPressed(GLFW_KEY_A)) { velocity.x -= m_velocity; }
            if (Input::IsKeyPressed(GLFW_KEY_D)) { velocity.x += m_velocity; }
            if (glm::length(velocity) > 0.0f) { velocity = glm::normalize(velocity); }
            scene->GetPhysicsWorld()->SetVelocity(scene->GetEntity(uid), velocity, VelocityType::Linear);
        }
    }
}// namespace SOF
