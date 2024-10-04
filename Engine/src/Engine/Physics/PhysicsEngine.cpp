#include "pch.h"
#include "PhysicsWorld.h"
#include "PhysicsEngine.h"

namespace SOF
{

    PhysicsGlobals *s_Props = nullptr;


    void PhysicsEngine::Init() { s_Props = new PhysicsGlobals(); }

    void PhysicsEngine::Shutdown()
    {
        delete s_Props;
        s_Props = nullptr;
    }

    PhysicsGlobals *PhysicsEngine::GetGlobalSettings()
    {
        SOF_ASSERT(s_Props, "Globals not initiated, did you forget to run Init()?");
        return s_Props;
    }

    std::shared_ptr<PhysicsWorld> PhysicsEngine::CreateWorld(Scene *context) { return PhysicsWorld::Create(context); }

}// namespace SOF