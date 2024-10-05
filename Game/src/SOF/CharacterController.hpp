#pragma ONCE
#include <Engine/Scene/Scene.h>
namespace SOF
{

    class CharacterController
    {
        public:
        virtual ~CharacterController() = default;
        virtual void UpdateMovement(UUID, std::shared_ptr<Scene> scene) = 0;
    };

}// namespace SOF
