#include "pch.h"
#include "PhysicsWorld.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#include "PhysicsEngine.h"

namespace SOF
{
    PhysicsWorld::PhysicsWorld(Scene *context) : m_Context(context)
    {
        SOF_ASSERT(context, "Provided context isn't valid");
        b2WorldDef world_def = b2DefaultWorldDef();
        world_def.gravity = b2Vec2(m_Gravity.x, m_Gravity.y);
        m_WorldID = b2CreateWorld(&world_def);
        SOF_ASSERT(b2World_IsValid(m_WorldID), "Physics World failed to create");
        auto rigid_body_collider = context->GetRegistry()->GetComponentRegistry<Rigidbody2DComponent>();
        if (rigid_body_collider) {
            for (auto &[id, rigidbody] : *rigid_body_collider) { AddBody(context->GetEntity(id)); }
        }
    }

    void PhysicsWorld::SetAirFriction(float friction)
    {
        m_AirFriction = friction;
        for (auto &[id, rigid_bodies] : m_RigidBodies) {
            b2Body_SetLinearDamping(rigid_bodies->GetBodyID(), m_AirFriction);
        }
    }

    void PhysicsWorld::UpdateDirtyBodies()
    {
        if (m_DirtyBodies.size() < 1) { return; }
        for (UUID entityID : m_DirtyBodies) {
            auto entity = m_Context->GetEntity(entityID);
            auto transform = entity->GetComponent<TransformComponent>();
            auto it = m_RigidBodies.find(entityID);
            if (it != m_RigidBodies.end()) {
                auto &rigid_body = it->second;
                b2BodyId body_id = rigid_body->GetBodyID();
                b2Vec2 newPosition = { transform->Translation.x, transform->Translation.y };
                float newAngle = transform->Rotation.z;
                b2Body_SetTransform(body_id, newPosition, b2MakeRot(transform->Rotation.z));
                b2Body_SetAwake(body_id, true);
            }
        }
        m_DirtyBodies.clear();
    }


    PhysicsWorld::~PhysicsWorld() { b2DestroyWorld(m_WorldID); }
    void PhysicsWorld::AddBody(Entity *entity)
    {
        if (entity->HasComponent<Rigidbody2DComponent>()) {
            m_RigidBodies[entity->GetHandle()] = std::make_unique<RigidBody>(entity, m_WorldID);
            b2Body_SetLinearDamping(m_RigidBodies[entity->GetHandle()]->GetBodyID(), m_AirFriction);
        }
    }
    void PhysicsWorld::AddPolygon(Entity *entity)
    {
        auto entity_handle = entity->GetHandle();
        auto it = m_RigidBodies.find(entity_handle);
        if (it != m_RigidBodies.end()) { m_RigidBodies[entity_handle]->AddPolygon(entity); }
    }
    void PhysicsWorld::RemovePolygon(Entity *entity)
    {
        auto entity_handle = entity->GetHandle();
        auto it = m_RigidBodies.find(entity_handle);
        if (it != m_RigidBodies.end()) { m_RigidBodies[entity_handle]->RemovePolygon(entity); }
    }
    void PhysicsWorld::StartSimulation() { m_IsSimulating = true; }
    void PhysicsWorld::PauseSimulation() { m_IsSimulating = false; }

    void PhysicsWorld::Step()
    {
        if (!m_IsSimulating) {
            SOF_ERROR("PhysicsWorld", "Tried stepping without starting the simulation");
            return;
        }
        PhysicsGlobals *props = PhysicsEngine::GetGlobalSettings();

        UpdateDirtyBodies();

        for (auto &[id, velocity] : m_Velocities) {
            if (velocity.Type == VelocityType::Linear) {
                b2Body_SetLinearVelocity(m_RigidBodies[id]->GetBodyID(), { velocity.Velocity.x, velocity.Velocity.y });
            } else if (velocity.Type == VelocityType::Angular) {
                b2Body_SetAngularVelocity(m_RigidBodies[id]->GetBodyID(), velocity.Velocity.z);
            }
        }
        m_Velocities.clear();

        b2World_Step(m_WorldID, props->TimeStep, props->SubStep);
        // Update all transforms
        auto rigid_body_collider = m_Context->GetRegistry()->GetComponentRegistry<Rigidbody2DComponent>();
        for (auto &[id, rigid_body] : m_RigidBodies) {
            auto entity = m_Context->GetEntity(id);
            if (entity->HasComponent<TransformComponent>()) {
                auto transform = entity->GetComponent<TransformComponent>();
                b2Vec2 position = b2Body_GetPosition(rigid_body->GetBodyID());
                b2Rot rotation = b2Body_GetRotation(rigid_body->GetBodyID());
                transform->LocalTranslation = { position.x, position.y, transform->LocalTranslation.z };
                transform->LocalRotation.z = b2Rot_GetAngle(rotation);
            }
        }
    }

    void PhysicsWorld::SetGravity(const glm::vec2 &new_gravity)
    {
        m_Gravity = new_gravity;
        b2World_SetGravity(m_WorldID, { m_Gravity.x, m_Gravity.y });
    }

    void PhysicsWorld::RemoveBody(Entity *entity)
    {
        auto entity_handle = entity->GetHandle();
        auto it = m_RigidBodies.find(entity_handle);
        if (it != m_RigidBodies.end()) {
            auto body_id = m_RigidBodies[entity->GetHandle()]->GetBodyID();
            m_RigidBodies.erase(entity->GetHandle());
        }
    }

    void PhysicsWorld::SetBodyDirty(Entity *entity)
    {
        auto entity_handle = entity->GetHandle();
        auto it = m_RigidBodies.find(entity_handle);
        if (it != m_RigidBodies.end()) { m_DirtyBodies.push_back(entity_handle); }
    }
    void PhysicsWorld::SetVelocity(Entity *entity, const glm::vec3 &velocity, VelocityType type)
    {
        auto entity_handle = entity->GetHandle();
        VelocityData data;
        data.Type = type;
        data.Velocity = velocity;
        m_Velocities[entity_handle] = data;
    }
}// namespace SOF