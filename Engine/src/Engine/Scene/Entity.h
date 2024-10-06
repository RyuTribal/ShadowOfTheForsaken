#pragma once
#include "Scene.h"

namespace SOF
{

    class Entity
    {
        public:
        Entity() : m_Scene(nullptr) {}
        Entity(Scene *scene_ptr) : m_Handle(UUID()), m_Scene(scene_ptr) {}
        Entity(UUID handle, Scene *scene_ptr) : m_Handle(handle), m_Scene(scene_ptr) {}
        ~Entity() = default;


        template<typename Type> void AddComponent(Type component)
        {
            if (m_Scene != nullptr) { m_Scene->GetRegistry()->Add<Type>(m_Handle, component); }
        }

        template<typename Type> void RemoveComponent()
        {
            if (m_Scene != nullptr) { m_Scene->GetRegistry()->Remove<Type>(m_Handle); }
        }

        template<typename Type> Type *GetComponent()
        {
            if (m_Scene != nullptr) { return m_Scene->GetRegistry()->Get<Type>(m_Handle); }
            return nullptr;
        }

        template<typename Type> bool HasComponent()
        {
            if (m_Scene != nullptr) { return m_Scene->GetRegistry()->Get<Type>(m_Handle) != nullptr ? true : false; }
            return false;
        }

        void ChangeScene(Scene *scene) { m_Scene = scene; }
        Scene *GetScene() { return m_Scene; }

        UUID &GetHandle() { return m_Handle; }

        private:
        UUID m_Handle;

        Scene *m_Scene;
    };
}// namespace SOF