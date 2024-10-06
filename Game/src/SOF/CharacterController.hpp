#pragma once
#include <Engine/Engine.h>
namespace SOF
{

    class CharacterController
    {
        public:
        virtual ~CharacterController() = default;
        virtual void UpdateMovement(Entity *context) = 0;
    };

}// namespace SOF
