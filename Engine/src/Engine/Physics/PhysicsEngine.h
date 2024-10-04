#pragma once

namespace SOF
{
    struct PhysicsGlobals
    {
        float TimeStep = 1.0f / 60.0f;
        int8_t SubStep = 4;
    };
    class PhysicsEngine
    {
        public:
        static void Init();
        static void Shutdown();
        static PhysicsGlobals *GetGlobalSettings();

        static std::shared_ptr<PhysicsWorld> CreateWorld(Scene *context);

        private:
    };
}// namespace SOF