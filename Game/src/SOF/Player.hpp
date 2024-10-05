#pragma once

#include "PlayerController.hpp"
namespace SOF
{

    class Player
    {
        public:
        Player() = delete;
        Player(UUID, float);
        ~Player() = default;

        void UpdateMovement(std::shared_ptr<Scene>);

        private:
        std::unique_ptr<CharactherController> m_PlayerController;
        UUID m_id;
    };

}// namespace SOF
