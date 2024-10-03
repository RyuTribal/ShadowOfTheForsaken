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

    Scene::~Scene() { DestroyPhysicsWorld(); }

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

    void Scene::CreatePhysicsWorld()
    {
        b2WorldDef world_def = b2DefaultWorldDef();
        world_def.gravity = b2Vec2(m_Gravity.x, m_Gravity.y);
        m_PhysicsWorldID = b2CreateWorld(&world_def);
        SOF_ASSERT(b2World_IsValid(m_PhysicsWorldID), "World failed to create");
        auto rigid_body_collider = m_ComponentRegistry.GetComponentRegistry<Rigidbody2DComponent>();
        if (rigid_body_collider) {
            for (auto &[id, rigidbody] : *rigid_body_collider) {
                auto &entity = m_EntityMap[id];
                b2BodyDef bodyDef = b2DefaultBodyDef();
                switch (rigidbody.Type) {
                case ColliderType::STATIC:
                    bodyDef.type = b2_staticBody;
                    break;
                case ColliderType::DYNAMIC:
                    bodyDef.type = b2_dynamicBody;
                    break;
                case ColliderType::KINEMATIC:
                    bodyDef.type = b2_kinematicBody;
                    break;
                }
                bodyDef.fixedRotation = rigidbody.FixedRotation;

                if (entity->HasComponent<TransformComponent>()) {
                    auto transform = entity->GetComponent<TransformComponent>();
                    bodyDef.position = b2Vec2(transform->Translation.x, transform->Translation.y);
                    bodyDef.rotation = b2MakeRot(transform->Rotation.z);
                } else {
                    bodyDef.position = { 0.0f, 0.0f };
                }

                rigidbody.RuntimeBodyID = b2CreateBody(m_PhysicsWorldID, &bodyDef);

                if (entity->HasComponent<BoxCollider2DComponent>()) {
                    auto boxCollider = entity->GetComponent<BoxCollider2DComponent>();
                    boxCollider->Shape = b2MakeBox(boxCollider->Size.x * 0.5f, boxCollider->Size.y * 0.5f);
                    for (int32_t i = 0; i < boxCollider->Shape.count; ++i) {
                        boxCollider->Shape.vertices[i].x += boxCollider->Offset.x;
                        boxCollider->Shape.vertices[i].y += boxCollider->Offset.y;
                    }
                    b2ShapeDef shapeDef = b2DefaultShapeDef();
                    shapeDef.density = boxCollider->Density;
                    shapeDef.friction = boxCollider->Friction;
                    shapeDef.restitution = boxCollider->Restitution;
                    boxCollider->RuntimeShapeID =
                      b2CreatePolygonShape(rigidbody.RuntimeBodyID, &shapeDef, &boxCollider->Shape);
                }

                if (entity->HasComponent<CircleCollider2DComponent>()) {
                    auto circleCollider = entity->GetComponent<CircleCollider2DComponent>();
                    circleCollider->Shape.center = b2Vec2(circleCollider->Offset.x, circleCollider->Offset.y);
                    circleCollider->Shape.radius = circleCollider->Radius;

                    b2ShapeDef shapeDef = b2DefaultShapeDef();
                    shapeDef.density = circleCollider->Density;
                    shapeDef.friction = circleCollider->Friction;
                    shapeDef.restitution = circleCollider->Restitution;

                    circleCollider->RuntimeShapeID =
                      b2CreateCircleShape(rigidbody.RuntimeBodyID, &shapeDef, &circleCollider->Shape);
                }

                if (entity->HasComponent<CapsuleCollider2DComponent>()) {
                    auto capsuleCollider = entity->GetComponent<CapsuleCollider2DComponent>();
                    float halfHeight = capsuleCollider->Height * 0.5f;

                    // Define the two endpoints of the capsule along the y-axis (vertical capsule)
                    b2Vec2 point1 = { capsuleCollider->Offset.x, capsuleCollider->Offset.y - halfHeight };
                    b2Vec2 point2 = { capsuleCollider->Offset.x, capsuleCollider->Offset.y + halfHeight };

                    capsuleCollider->Shape.center1 = point1;
                    capsuleCollider->Shape.center2 = point2;
                    capsuleCollider->Shape.radius = capsuleCollider->Radius;

                    b2ShapeDef shapeDef = b2DefaultShapeDef();
                    shapeDef.density = capsuleCollider->Density;
                    shapeDef.friction = capsuleCollider->Friction;
                    shapeDef.restitution = capsuleCollider->Restitution;
                    capsuleCollider->RuntimeShapeID =
                      b2CreateCapsuleShape(rigidbody.RuntimeBodyID, &shapeDef, &capsuleCollider->Shape);
                }
            }
        }
    }

    void Scene::DestroyPhysicsWorld() { b2DestroyWorld(m_PhysicsWorldID); }

    void Scene::SetGravity(const glm::vec2 &gravity)
    {
        m_Gravity = gravity;
        b2World_SetGravity(m_PhysicsWorldID, { m_Gravity.x, m_Gravity.y });
    }

    void Scene::Update()
    {
        // Run physics once
        b2World_Step(m_PhysicsWorldID, m_PhysicsTimeStep, m_PhysicsSubStep);
        // Update all transforms
        auto rigid_body_collider = m_ComponentRegistry.GetComponentRegistry<Rigidbody2DComponent>();
        for (auto [id, rigid_body] : *rigid_body_collider) {
            auto &entity = m_EntityMap[id];
            if (entity->HasComponent<TransformComponent>()) {
                auto transform = entity->GetComponent<TransformComponent>();
                b2Vec2 position = b2Body_GetPosition(rigid_body.RuntimeBodyID);
                b2Rot rotation = b2Body_GetRotation(rigid_body.RuntimeBodyID);
                transform->Translation = { position.x, position.y, transform->Translation.z };
                transform->Rotation.z = b2Rot_GetAngle(rotation);
            }
        }


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
    }


    void Scene::End() {}

    void Scene::DestroyEntity(UUID handle)
    {
        m_EntityMap.erase(handle);
        m_ComponentRegistry.RemoveAllFromEntity(handle);
    }

    Entity *Scene::GetEntity(UUID id) { return m_EntityMap[id].get(); }
}// namespace SOF