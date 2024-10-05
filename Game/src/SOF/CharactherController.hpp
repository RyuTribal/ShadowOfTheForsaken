#pragma ONCE
#include <Engine/Scene/Scene.h>
namespace SOF
{

    class CharactherController
    {
        public:
        virtual ~CharactherController() = default;
        virtual void UpdateMovement(UUID, std::shared_ptr<Scene> scene) = 0;
    };

}// namespace SOF
