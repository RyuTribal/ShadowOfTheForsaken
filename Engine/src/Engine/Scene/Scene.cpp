#include "pch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Core/Game.h"
#include "Engine/Sound/SoundEngine.h"
#include "Engine/Physics/PhysicsEngine.h"
#include "Engine/Core/Profiler.h"

namespace SOF
{
    std::shared_ptr<Scene> Scene::CreateScene(const std::string &name) { return std::make_shared<Scene>(name); }
    Scene::Scene(const std::string &name) : m_Name(name)
    {
        m_PhysicsWorld = PhysicsEngine::CreateWorld(this);
        m_ComponentRegistry.RegisterComponentAddedCallback(
          [this](UUID entity_id, const std::type_index &type_index) { this->OnComponentAdded(entity_id, type_index); });
        m_ComponentRegistry.RegisterComponentRemovedCallback([this](UUID entity_id, const std::type_index &type_index) {
            this->OnComponentRemoved(entity_id, type_index);
        });
        m_PhysicsWorld->StartSimulation();
    }

    Scene::~Scene() {}

    UUID Scene::CreateEntity(const std::string &name)
    {
        UUID new_id = UUID();
        m_EntityMap[new_id] = std::make_unique<Entity>(new_id, this);
        m_EntityMap[new_id]->AddComponent<TagComponent>(TagComponent(name));
        m_EntityMap[new_id]->AddComponent<RelationshipComponent>({});
        return new_id;
    }

    UUID Scene::CreateEntity(const std::string &name, UUID handle)
    {
        m_EntityMap[handle] = std::make_unique<Entity>(handle, this);
        m_EntityMap[handle]->AddComponent<TagComponent>(TagComponent(name));
        m_EntityMap[handle]->AddComponent<RelationshipComponent>({});
        return handle;
    }

    void Scene::AddChild(UUID parentID, UUID childID)
    {
        auto parentRelationship = m_ComponentRegistry.Get<RelationshipComponent>(parentID);
        auto childRelationship = m_ComponentRegistry.Get<RelationshipComponent>(childID);
        if (!childRelationship) { return; }
        if (parentID != 0 && !parentRelationship) { return; }// Not the root node or a valid entity

        // Means that you are trying to detach the child to the root node, aka parentID = 0
        if (parentRelationship) { parentRelationship->Children.push_back(childID); }
        childRelationship->ParentID = parentID;
    }

    void Scene::RemoveEntity(UUID entity_id)
    {
        Entity *entity = m_EntityMap[entity_id].get();
        UUID parent_id = entity->GetComponent<RelationshipComponent>()->ParentID;
        if (parent_id != 0) {
            Entity *parent_entity = m_EntityMap[parent_id].get();
            auto &children_vector = parent_entity->GetComponent<RelationshipComponent>()->Children;
            for (size_t i = 0; i < children_vector.size(); i++) {
                if (children_vector[i] == entity_id) {
                    children_vector.erase(children_vector.begin() + i);
                    break;
                }
            }
        }
        auto &children_vector = entity->GetComponent<RelationshipComponent>()->Children;
        for (size_t i = 0; i < children_vector.size(); i++) { RemoveEntity(children_vector[i]); }
        m_EntityMap.erase(entity_id);
    }

    void Scene::ReparentEntity(UUID entity_id, UUID new_parent_id)
    {
        Entity *child = m_EntityMap[entity_id].get();
        Entity *parent = m_EntityMap[new_parent_id].get();
        if ((new_parent_id != 0 && !parent) || !child) { return; }
        if (new_parent_id != 0) { parent->GetComponent<RelationshipComponent>()->Children.push_back(entity_id); }
        child->GetComponent<RelationshipComponent>()->ParentID = new_parent_id;
    }

    void Scene::UpdateChildTransforms(UUID parent_id)
    {
        Entity *parent_entity = m_EntityMap[parent_id].get();
        auto parent_relationship = parent_entity->GetComponent<RelationshipComponent>();
        auto parent_transform = parent_entity->GetComponent<TransformComponent>();
        auto parent_camera = parent_entity->GetComponent<CameraComponent>();

        if (parent_camera && parent_camera->ClipToTransform) {
            parent_camera->Camera->SetPosition(parent_transform->Translation);
        }

        for (UUID child_id : parent_relationship->Children) {
            auto child_transform = m_ComponentRegistry.Get<TransformComponent>(child_id);
            if (child_transform) {
                if (parent_transform) {
                    glm::quat parent_quat = glm::quat(parent_transform->Rotation);
                    glm::vec3 parent_scale = parent_transform->Scale;
                    glm::vec3 scaled_local_translation = parent_scale * child_transform->LocalTranslation;
                    glm::vec3 rotated_translation = parent_quat * scaled_local_translation;
                    child_transform->Translation = parent_transform->Translation + rotated_translation;
                    glm::quat child_local_quat = glm::quat(child_transform->LocalRotation);
                    glm::quat child_world_quat = parent_quat * child_local_quat;
                    glm::vec3 child_world_euler = glm::eulerAngles(child_world_quat);
                    child_transform->Rotation = child_world_euler;
                    child_transform->Scale = parent_scale * child_transform->LocalScale;
                }
            }
            m_Threads.AddTask([this, child_id]() { this->UpdateChildTransforms(child_id); });
        }
    }

    void Scene::Begin()
    {
        auto write_buffer = Game::Get()->GetRenderingThread().GetWriteBuffer();
        auto camera_registry = m_ComponentRegistry.GetComponentRegistry<CameraComponent>();
        if (camera_registry) {
            for (auto [id, camera] : *camera_registry) {
                if (camera.IsActive) {
                    write_buffer->FrameCamera = camera.Camera.get();
                    break;
                }
            }
        }
    }

    void Scene::Update()
    {
        SOF_PROFILE_FUNC("Scene update");
        // Run physics once
        m_PhysicsWorld->Step();

        // Update relationship transforms
        auto relationship_registry = m_ComponentRegistry.GetComponentRegistry<RelationshipComponent>();
        if (relationship_registry) {
            for (auto &[id, relationship] : *relationship_registry) {
                if (relationship.ParentID == 0) { UpdateChildTransforms(id); }
            }
        }

        m_Threads.Await();

        // Draw all entities
        auto sprite_registry = m_ComponentRegistry.GetComponentRegistry<SpriteComponent>();
        auto write_buffer = Game::Get()->GetRenderingThread().GetWriteBuffer();
        Camera *curr_camera = write_buffer->FrameCamera;
        if (sprite_registry) {
            glm::vec3 &camera_pos = curr_camera->GetPosition();
            float half_width = curr_camera->GetWidth() * 0.5f / curr_camera->GetZoomLevel();
            float half_height = curr_camera->GetHeight() * 0.5f / curr_camera->GetZoomLevel();

            float camera_left = camera_pos.x - half_width;
            float camera_right = camera_pos.x + half_width;
            float camera_up = camera_pos.y + half_height;
            float camera_down = camera_pos.y - half_height;
            for (auto &[id, sprite] : *sprite_registry) {
                auto transform = m_ComponentRegistry.Get<TransformComponent>(id);
                glm::vec2 half_extents = glm::vec2(transform->Scale.x * 0.5f, transform->Scale.y * 0.5f);

                float left = transform->Translation.x - half_extents.x;
                float right = transform->Translation.x + half_extents.x;
                float top = transform->Translation.y + half_extents.y;
                float bottom = transform->Translation.y - half_extents.y;

                if (right >= camera_left && left <= camera_right && top >= camera_down && bottom <= camera_up) {
                    auto transform_mat = transform->CreateMat4x4();
                    Renderer::SubmitSquare(sprite.Color,
                      sprite.Texture.get(),
                      transform_mat,
                      sprite.SpriteCoordinates,
                      sprite.SpriteSize,
                      sprite.Layer);
                }
            }
        }

        // Update sound engine
        auto listener_transform = m_ComponentRegistry.Get<TransformComponent>(m_Listener);
        if (listener_transform) { SoundEngine::Update(listener_transform->Translation, this); }
    }


    void Scene::End() {}

    void Scene::OnComponentAdded(UUID entity_id, const std::type_index &type_index)
    {
        if (type_index == typeid(Rigidbody2DComponent)) {
            m_PhysicsWorld->AddBody(m_EntityMap[entity_id].get());
        } else if (type_index == typeid(BoxCollider2DComponent) || type_index == typeid(CircleCollider2DComponent)
                   || type_index == typeid(CapsuleCollider2DComponent)) {
            if (m_EntityMap[entity_id].get()->HasComponent<Rigidbody2DComponent>()) {
                m_PhysicsWorld->AddPolygon(m_EntityMap[entity_id].get());
            }
        }
    }

    void Scene::OnComponentRemoved(UUID entity_id, const std::type_index &type_index)
    {
        if (type_index == typeid(Rigidbody2DComponent)) {
            m_PhysicsWorld->RemoveBody(m_EntityMap[entity_id].get());
        } else if (type_index == typeid(BoxCollider2DComponent) || type_index == typeid(CircleCollider2DComponent)
                   || type_index == typeid(CapsuleCollider2DComponent)) {
            if (m_EntityMap[entity_id].get()->HasComponent<Rigidbody2DComponent>()) {
                m_PhysicsWorld->RemovePolygon(m_EntityMap[entity_id].get());
            }
        }
    }

    void Scene::DestroyEntity(UUID handle)
    {
        m_EntityMap.erase(handle);
        m_ComponentRegistry.RemoveAllFromEntity(handle);
    }

    Entity *Scene::GetEntity(UUID id) { return m_EntityMap[id].get(); }
}// namespace SOF