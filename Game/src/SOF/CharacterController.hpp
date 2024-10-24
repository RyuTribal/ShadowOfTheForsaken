#pragma once
#include <Engine/Engine.h>

namespace SOF
{

    class CharacterController
    {
        public:
        virtual ~CharacterController() = default;
        virtual void UpdateMovement(Entity *context, float dt) = 0;
    };

}// namespace SOF
