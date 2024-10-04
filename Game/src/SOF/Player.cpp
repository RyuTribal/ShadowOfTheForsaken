#include "pch.h"
#include "Player.hpp"

namespace SOF
{
    Player::Player(UUID uid, float velo) : m_id{ uid }, m_PlayerController{ std::make_unique<PlayerController>(velo) }
    {}

    void Player::UpdateMovement(std::shared_ptr<Scene> scene) { m_PlayerController->UpdateMovement(m_id, scene); }
}// namespace SOF
