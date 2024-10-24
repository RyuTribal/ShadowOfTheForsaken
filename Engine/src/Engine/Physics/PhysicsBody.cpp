#include "pch.h"
#include "PhysicsBody.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"

namespace SOF
{
    RigidBody::RigidBody(Entity *entity, b2WorldId world_id)
    {
        SOF_ASSERT(entity && entity->HasComponent<Rigidbody2DComponent>(), "Entity is not a valid rigid body entity");
        auto rigidbody = entity->GetComponent<Rigidbody2DComponent>();
        b2BodyDef bodyDef = b2DefaultBodyDef();
        switch (rigidbody->Type) {
        case ColliderType::STATIC:
            m_BodyType = b2_staticBody;
            bodyDef.type = b2_staticBody;
            break;
        case ColliderType::DYNAMIC:
            m_BodyType = b2_dynamicBody;
            bodyDef.type = b2_dynamicBody;
            break;
        case ColliderType::KINEMATIC:
            m_BodyType = b2_kinematicBody;
            bodyDef.type = b2_kinematicBody;
            break;
        }
        bodyDef.fixedRotation = rigidbody->FixedRotation;

        if (entity->HasComponent<TransformComponent>()) {
            auto transform = entity->GetComponent<TransformComponent>();
            bodyDef.position = b2Vec2(transform->Translation.x, transform->Translation.y);
            bodyDef.rotation = b2MakeRot(transform->Rotation.z);
        } else {
            bodyDef.position = { 0.0f, 0.0f };
        }

        m_RuntimeBodyID = b2CreateBody(world_id, &bodyDef);

        AddPolygon(entity);
    }
    void RigidBody::DestroyBody() { b2DestroyBody(m_RuntimeBodyID); }
    void RigidBody::AddPolygon(Entity *entity)
    {
        if (entity->HasComponent<BoxCollider2DComponent>()) {
            auto box_collider = entity->GetComponent<BoxCollider2DComponent>();
            m_Polygons.push_back(std::make_unique<BoxShape>(m_RuntimeBodyID,
              box_collider->Density,
              box_collider->Friction,
              box_collider->Restitution,
              box_collider->Offset,
              box_collider->HalfSize));
            m_Polygons[m_Polygons.size() - 1]->CreateShape();
        }
        if (entity->HasComponent<CircleCollider2DComponent>()) {
            auto circle_collider = entity->GetComponent<CircleCollider2DComponent>();
            m_Polygons.push_back(std::make_unique<CircleShape>(m_RuntimeBodyID,
              circle_collider->Density,
              circle_collider->Friction,
              circle_collider->Restitution,
              circle_collider->Offset,
              circle_collider->Radius));
            m_Polygons[m_Polygons.size() - 1]->CreateShape();
        }
        if (entity->HasComponent<CapsuleCollider2DComponent>()) {
            auto capsule_collider = entity->GetComponent<CapsuleCollider2DComponent>();
            m_Polygons.push_back(std::make_unique<CapsuleShape>(m_RuntimeBodyID,
              capsule_collider->Density,
              capsule_collider->Friction,
              capsule_collider->Restitution,
              capsule_collider->Offset,
              capsule_collider->Radius,
              capsule_collider->HalfHeight));
            m_Polygons[m_Polygons.size() - 1]->CreateShape();
        }
    }
    void RigidBody::RemovePolygon(Entity *entity)
    {
        bool has_box = entity->HasComponent<BoxCollider2DComponent>();
        bool has_circle = entity->HasComponent<CircleCollider2DComponent>();
        bool has_capsule = entity->HasComponent<CapsuleCollider2DComponent>();

        if (has_box && has_circle && has_capsule) { return; }

        for (size_t i = 0; i < m_Polygons.size(); i++) {
            if (!has_box && m_Polygons[i]->GetType() == PolygonType::Box) { m_Polygons.erase(m_Polygons.begin() + i); }
            if (!has_circle && m_Polygons[i]->GetType() == PolygonType::Circle) {
                m_Polygons.erase(m_Polygons.begin() + i);
            }
            if (!has_capsule && m_Polygons[i]->GetType() == PolygonType::Capsule) {
                m_Polygons.erase(m_Polygons.begin() + i);
            }
        }
    }
}// namespace SOF