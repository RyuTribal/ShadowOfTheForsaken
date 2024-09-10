#pragma once
#include "Registry.h"


namespace SOF {

class Entity;

class Scene
{
  public:
    static std::shared_ptr<Scene> CreateScene(const std::string &name);

    Scene(const std::string &name);

    Registry *GetRegistry() { return &m_ComponentRegistry; }

    UUID CreateEntity(const std::string &name);
    UUID CreateEntity(const std::string &name, UUID handle);

    void Begin();

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
};
}// namespace SOF