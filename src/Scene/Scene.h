#pragma once
#include "Registry.h"
#include "box2d/box2d.h"

namespace SOF
{

    class Entity;

    class Scene
    {
        public:
        static constexpr glm::vec2 DefaultGravity = { 0.f, -9.82f };
        static std::shared_ptr<Scene> CreateScene(const std::string &name);

        Scene(const std::string &name);
        ~Scene();

        Registry *GetRegistry() { return &m_ComponentRegistry; }

        UUID CreateEntity(const std::string &name);
        UUID CreateEntity(const std::string &name, UUID handle);

        void Begin();
        void CreatePhysicsWorld();
        void DestroyPhysicsWorld();
        void SetGravity(const glm::vec2 &gravity);

        void Update();

        void End();

        template<typename T> std::unordered_map<UUID, T> GetAllEntitiesByType() { return m_ComponentRegistry.Get<T>(); }

        UUID GetID() { return m_ID; }

        const std::string &GetName() { return m_Name; }

        void DestroyEntity(UUID handle);

        Entity *GetEntity(UUID id);

        uint32_t EntitySize() { return (uint32_t)m_EntityMap.size(); }

        private:
        UUID m_ID = UUID();
        Registry m_ComponentRegistry;
        std::unordered_map<UUID, std::unique_ptr<Entity>> m_EntityMap;
        std::string m_Name = "Untitled Level";

        // Physics stuff, mnight be good to move this into it's own thing
        b2WorldId m_PhysicsWorldID;
        glm::vec2 m_Gravity = DefaultGravity;
        float m_PhysicsTimeStep = 1.0f / 60.0f;
        int8_t m_PhysicsSubStep = 4;
    };
}// namespace SOF