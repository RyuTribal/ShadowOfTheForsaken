#include "pch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "Renderer/Renderer.h"
#include "Core/Game.h"

namespace SOF
{
    std::shared_ptr<Scene> Scene::CreateScene(const std::string &name) { return std::make_shared<Scene>(name); }
    Scene::Scene(const std::string &name) : m_Name(name) {}

    UUID Scene::CreateEntity(const std::string &name)
    {
        UUID new_id = UUID();
        m_EntityMap[new_id] = std::make_unique<Entity>(new_id, this);
        m_EntityMap[new_id]->AddComponent<TagComponent>(TagComponent(name));
        return new_id;
    }

    UUID Scene::CreateEntity(const std::string &name, UUID handle)
    {
        m_EntityMap[handle] = std::make_unique<Entity>(handle, this);
        m_EntityMap[handle]->AddComponent<TagComponent>(TagComponent(name));
        return handle;
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
            for (auto [id, sprite] : *sprite_registry) {
                auto transform = m_ComponentRegistry.Get<TransformComponent>(id);
                glm::vec2 half_extents = glm::vec2(transform->Scale.x * 0.5f, transform->Scale.y * 0.5f);

                float left = transform->Translation.x - half_extents.x;
                float right = transform->Translation.x + half_extents.x;
                float top = transform->Translation.y + half_extents.y;
                float bottom = transform->Translation.y - half_extents.y;

                if (right >= camera_left && left <= camera_right && top >= camera_down && bottom <= camera_up) {
                    auto transform_mat = transform->CreateMat4x4();
                    Renderer::SubmitSquare(sprite.Color, sprite.Texture.get(), transform_mat);
                }
            }
        }
    }


    void Scene::End() {}

    void Scene::DestroyEntity(UUID handle)
    {
        m_EntityMap.erase(handle);
        m_ComponentRegistry.RemoveAllFromEntity(handle);
    }

    Entity *Scene::GetEntity(UUID id) { return m_EntityMap[id].get(); }
}// namespace SOF