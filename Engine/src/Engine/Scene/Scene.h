#pragma once
#include "Registry.h"
#include "Engine/Physics/PhysicsWorld.h"
#include "Engine/Core/ThreadPool.h"
#include "Chunk.h"

namespace SOF
{

    class Entity;
    class Texture;
    class Scene
    {
        public:
        static std::shared_ptr<Scene> CreateScene(const std::string &name);

        Scene(const std::string &name);
        ~Scene();

        Registry *GetRegistry() { return &m_ComponentRegistry; }

        UUID CreateEntity(const std::string &name);
        UUID CreateEntity(const std::string &name, UUID handle);
        void AddChild(UUID parentID, UUID childID);
        void RemoveEntity(UUID entity_id);
        void ReparentEntity(UUID entity_id, UUID new_parent_id);

        void SetBackground(const std::string &background_handle);

        void Begin();
        void Update();
        void End();

        void OnComponentAdded(UUID entity_id, const std::type_index &type_index);
        void OnComponentRemoved(UUID entity_id, const std::type_index &type_index);

        PhysicsWorld *GetPhysicsWorld() { return m_PhysicsWorld.get(); }

        template<typename T> std::unordered_map<UUID, T> GetAllEntitiesByType() { return m_ComponentRegistry.Get<T>(); }

        UUID GetID() { return m_ID; }

        const std::string &GetName() { return m_Name; }

        void DestroyEntity(UUID handle);

        Entity *GetEntity(UUID id);

        uint32_t EntitySize() { return (uint32_t)m_EntityMap.size(); }

        UUID GetListenerEntity() { return m_Listener; }
        void SetListenerEntity(UUID id) { m_Listener = id; }

        void SetDirtyTransform(UUID entity_id) { m_DirtyTransforms.push_back(entity_id); }

        ChunkManager &GetChunks() { return m_Chunks; }

        private:
        void UpdateChildTransforms(UUID parent_id);

        private:
        UUID m_ID = UUID();
        Registry m_ComponentRegistry;
        std::unordered_map<UUID, std::unique_ptr<Entity>> m_EntityMap;
        std::vector<UUID> m_DirtyTransforms;
        std::string m_Name = "Untitled Level";
        std::shared_ptr<PhysicsWorld> m_PhysicsWorld;
        ThreadPool m_Threads{ 16 };
        std::shared_ptr<Texture> m_Background;
        ChunkManager m_Chunks{ 8.f, this };
        // Some sound settings
        UUID m_Listener;
    };
}// namespace SOF