#include "pch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Core/Game.h"
#include "Engine/Sound/SoundEngine.h"
#include "Engine/Physics/PhysicsEngine.h"
#include "Engine/Core/Profiler.h"
#include "Engine/Asset/Manager.h"
#include "Engine/Renderer/Texture.h"

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

    void Scene::SetBackground(const std::string &background_handle)
    {
        m_Background = AssetManager::Load<Texture>(background_handle);
    }

    void Scene::UpdateChildTransforms(UUID parent_id)
    {
        SOF_PROFILE_FUNC();
        Entity *parent_entity = m_EntityMap[parent_id].get();
        auto parent_relationship = parent_entity->GetComponent<RelationshipComponent>();
        auto parent_transform = parent_entity->GetComponent<TransformComponent>();
        auto parent_camera = parent_entity->GetComponent<CameraComponent>();

        if (parent_camera && parent_camera->ClipToTransform) {
            parent_camera->CameraRef->SetPosition(parent_transform->Translation);
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
                    m_Chunks.UpdateEntity(child_id);
                }
            }
            UpdateChildTransforms(child_id);
        }
    }

    void Scene::Begin()
    {
        auto write_buffer = Game::Get()->GetRenderingThread().GetWriteBuffer();
        auto camera_registry = m_ComponentRegistry.GetComponentRegistry<CameraComponent>();
        if (camera_registry) {
            for (auto [id, camera] : *camera_registry) {
                if (camera.IsActive) {
                    write_buffer->FrameCamera = camera.CameraRef.get();
                    break;
                }
            }
        }
        Renderer::SubmitBackgroundTexture(m_Background.get());
    }

    void Scene::Update()
    {
        SOF_PROFILE_FUNC();
        // Run physics once
        m_PhysicsWorld->Step();

        // Update relationship transforms

        {
            SOF_PROFILE_SCOPE("Scene: Relationship update");
            for (auto id : m_DirtyTransforms) {
                auto transform = m_EntityMap[id]->GetComponent<TransformComponent>();
                if (transform) {
                    transform->Translation = transform->LocalTranslation;
                    transform->Rotation = transform->LocalRotation;
                    transform->Scale = transform->LocalScale;
                    m_Chunks.UpdateEntity(id);
                    UpdateChildTransforms(id);
                }
            }
        }

        m_DirtyTransforms.clear();

        // Fix a better way to go through the visible sprites than just going through literally all the sprites
        // Draw all entities
        auto write_buffer = Game::Get()->GetRenderingThread().GetWriteBuffer();
        Camera *curr_camera = write_buffer->FrameCamera;
        write_buffer->FrameView = curr_camera->GetViewMatrix();
        write_buffer->FrameProjection = curr_camera->GetProjectionMatrix();
        auto visible_chunks = m_Chunks.GetVisibleChunks(
          curr_camera->GetPosition(), curr_camera->GetWorldWidth(), curr_camera->GetWorldHeight());

        {
            SOF_PROFILE_SCOPE("Scene: Sprite preparation");
            for (Chunk *chunk : visible_chunks) {
                for (UUID entity_id : chunk->Entities) {

                    auto sprite = m_EntityMap[entity_id]->GetComponent<SpriteComponent>();
                    auto transform = m_EntityMap[entity_id]->GetComponent<TransformComponent>();
                    SOF_ASSERT(sprite, "This object does not have a sprite, this hsould not happen");
                    auto transform_mat = transform->CreateMat4x4();
                    glm::vec2 texture_size = { 1.f, 1.f };
                    if (sprite->TextureRef) {
                        texture_size = glm::vec2(sprite->TextureRef->GetWidth(), sprite->TextureRef->GetHeight());
                    }

                    glm::vec2 normalized_uv_offset = sprite->SpriteUVOffset / texture_size;
                    glm::vec2 normalized_sprite_size = sprite->SpriteSize / texture_size;

                    glm::mat4x2 uv_coords =
                      glm::mat4x2(normalized_uv_offset + glm::vec2(0.0f, normalized_sprite_size.y),
                        normalized_uv_offset + glm::vec2(normalized_sprite_size.x, normalized_sprite_size.y),
                        normalized_uv_offset + glm::vec2(0.0f, 0.0f),
                        normalized_uv_offset + glm::vec2(normalized_sprite_size.x, 0.0f));

                    SpriteData data = { sprite->TextureRef.get(),
                        sprite->ShaderHandle,
                        sprite->Color,
                        uv_coords,
                        transform_mat,
                        sprite->Layer };
                    Renderer::SubmitSquare(&data);
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
        } else if (type_index == typeid(TransformComponent)) {
            m_Chunks.UpdateEntity(entity_id);
        } else if (type_index == typeid(SpriteComponent)) {
            m_Chunks.AddEntity(entity_id);
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
        } else if (type_index == typeid(SpriteComponent)) {
            m_Chunks.RemoveEntity(entity_id);
        }
    }

    void Scene::DestroyEntity(UUID handle)
    {
        m_ComponentRegistry.RemoveAllFromEntity(handle);
        m_EntityMap.erase(handle);
    }

    Entity *Scene::GetEntity(UUID id) { return m_EntityMap[id].get(); }
}// namespace SOF