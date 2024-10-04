#pragma once

#include <unordered_map>
#include <memory>
#include <typeindex>
#include <type_traits>
#include "Engine/Core/UUID.h"

namespace SOF
{
    using ComponentAddedCallback = std::function<void(UUID entityId, const std::type_index &type_index)>;
    using ComponentRemovedCallback = std::function<void(UUID entityId, const std::type_index &type_index)>;
    class IComponentContainer
    {
        public:
        virtual ~IComponentContainer() = default;
        virtual void Remove(UUID entityId) = 0;
    };

    template<typename T> class ComponentContainer : public IComponentContainer
    {
        public:
        void Add(UUID entityId, T component) { m_ComponentMap[entityId] = component; }

        T *Get(UUID entityId)
        {
            auto it = m_ComponentMap.find(entityId);
            if (it != m_ComponentMap.end()) { return &it->second; }
            return nullptr;
        }

        void Remove(UUID entityId) override
        {
            auto it = m_ComponentMap.find(entityId);
            if (it != m_ComponentMap.end()) { m_ComponentMap.erase(entityId); }
        }

        std::unordered_map<UUID, T> *map() { return &m_ComponentMap; }

        private:
        std::unordered_map<UUID, T> m_ComponentMap{};
    };

    class Registry
    {
        public:
        void RegisterComponentAddedCallback(const ComponentAddedCallback &callback) { m_AddedCallback = callback; }
        void RegisterComponentRemovedCallback(const ComponentRemovedCallback &callback)
        {
            m_RemovedCallback = callback;
        }

        template<typename T> void Add(UUID entityId, T component)
        {
            const std::type_index typeIndex = std::type_index(typeid(T));
            if (m_Components.find(typeIndex) == m_Components.end()) {
                m_Components[typeIndex] = std::make_shared<ComponentContainer<T>>();
            }
            std::static_pointer_cast<ComponentContainer<T>>(m_Components[typeIndex])
              ->Add(entityId, std::move(component));

            if (m_AddedCallback) { m_AddedCallback(entityId, typeIndex); }
        }

        template<typename T> T *Get(UUID entityId)
        {
            const std::type_index typeIndex = std::type_index(typeid(T));
            auto it = m_Components.find(typeIndex);
            if (it != m_Components.end()) {
                return std::static_pointer_cast<ComponentContainer<T>>(it->second)->Get(entityId);
            }
            return nullptr;
        }

        template<typename T> void Remove(UUID entityId)
        {
            const std::type_index typeIndex = std::type_index(typeid(T));
            auto it = m_Components.find(typeIndex);
            if (it != m_Components.end()) {
                std::static_pointer_cast<ComponentContainer<T>>(it->second)->Remove(entityId);
                if (m_RemovedCallback) { m_RemovedCallback(entityId, typeIndex); }
            }
        }

        void RemoveAllFromEntity(UUID entityId)
        {
            for (auto &[typeIndex, container] : m_Components) {
                container->Remove(entityId);
                if (m_RemovedCallback) { m_RemovedCallback(entityId, typeIndex); }
            }
        }

        template<typename T> std::unordered_map<UUID, T> *GetComponentRegistry()
        {
            const std::type_index typeIndex = std::type_index(typeid(T));
            auto it = m_Components.find(typeIndex);
            if (it != m_Components.end()) { return std::static_pointer_cast<ComponentContainer<T>>(it->second)->map(); }
            return nullptr;
        }

        private:
        std::unordered_map<std::type_index, std::shared_ptr<IComponentContainer>> m_Components;
        ComponentAddedCallback m_AddedCallback;
        ComponentRemovedCallback m_RemovedCallback;
    };
}// namespace SOF
