#include "pch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"

namespace SOF {
	std::shared_ptr<Scene> Scene::CreateScene(const std::string& name)
	{
		return std::make_shared<Scene>(name);
	}
	Scene::Scene(const std::string& name) : m_Name(name)
	{

	}

	UUID Scene::CreateEntity(const std::string& name)
	{
		UUID new_id = UUID();
		m_EntityMap[new_id] = std::make_unique<Entity>(new_id, this);
		m_EntityMap[new_id]->AddComponent<TagComponent>(TagComponent(name));
		return new_id;
	}

	UUID Scene::CreateEntity(const std::string& name, UUID handle)
	{
		m_EntityMap[handle] = std::make_unique<Entity>(handle, this);
		m_EntityMap[handle]->AddComponent<TagComponent>(TagComponent(name));
		return handle;
	}

	void Scene::DestroyEntity(UUID handle)
	{
		m_EntityMap.erase(handle);
		m_ComponentRegistry.RemoveAllFromEntity(handle);
	}

	Entity* Scene::GetEntity(UUID id)
	{
		return m_EntityMap[id].get();
	}
}