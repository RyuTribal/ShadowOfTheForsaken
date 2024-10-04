#pragma once
#include "box2d/box2d.h"

namespace SOF
{
    class Entity;

    enum PolygonType { Box, Circle, Capsule };

    class PolygonShape
    {
        public:
        virtual void CreateShape() = 0;
        PolygonShape(b2BodyId rigid_body_id,
          PolygonType type,
          float density,
          float friction,
          float restitution,
          const glm::vec2 &offset)
          : m_Type(type), m_Density(density), m_Friction(friction), m_Restitution(restitution), m_Offset(offset),
            m_RigidBodyID(rigid_body_id)
        {}
        virtual ~PolygonShape() = default;

        PolygonType GetType() { return m_Type; }

        protected:
        b2BodyId m_RigidBodyID;
        b2ShapeId m_RuntimeShapeID;
        PolygonType m_Type = PolygonType::Box;
        float m_Density = 1.0f;
        float m_Friction = 0.5f;
        float m_Restitution = 0.0f;
        glm::vec2 m_Offset = { 0.0f, 0.0f };
    };

    class BoxShape : public PolygonShape
    {
        public:
        BoxShape(b2BodyId rigid_body_id,
          float density,
          float friction,
          float restitution,
          const glm::vec2 &offset,
          const glm::vec2 &half_size)
          : PolygonShape(rigid_body_id, PolygonType::Box, density, friction, restitution, offset), m_HalfSize(half_size)
        {}

        ~BoxShape() { b2DestroyShape(m_RuntimeShapeID); }

        virtual void CreateShape() override
        {
            m_Shape = b2MakeBox(m_HalfSize.x, m_HalfSize.y);
            for (int32_t i = 0; i < m_Shape.count; ++i) {
                m_Shape.vertices[i].x += m_Offset.x;
                m_Shape.vertices[i].y += m_Offset.y;
            }
            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = m_Density;
            shapeDef.friction = m_Friction;
            shapeDef.restitution = m_Restitution;
            m_RuntimeShapeID = b2CreatePolygonShape(m_RigidBodyID, &shapeDef, &m_Shape);
        }

        private:
        b2Polygon m_Shape;
        glm::vec2 m_HalfSize = { 0.5f, 0.5f };
    };

    class CircleShape : public PolygonShape
    {
        public:
        CircleShape(b2BodyId rigid_body_id,
          float density,
          float friction,
          float restitution,
          const glm::vec2 &offset,
          float radius)
          : PolygonShape(rigid_body_id, PolygonType::Circle, density, friction, restitution, offset), m_Radius(radius)
        {}

        ~CircleShape() { b2DestroyShape(m_RuntimeShapeID); }

        virtual void CreateShape() override
        {
            m_Shape.center = b2Vec2(m_Offset.x, m_Offset.y);
            m_Shape.radius = m_Radius;

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = m_Density;
            shapeDef.friction = m_Friction;
            shapeDef.restitution = m_Restitution;

            m_RuntimeShapeID = b2CreateCircleShape(m_RigidBodyID, &shapeDef, &m_Shape);
        }

        private:
        b2Circle m_Shape;
        float m_Radius;
    };

    class CapsuleShape : public PolygonShape
    {
        public:
        CapsuleShape(b2BodyId rigid_body_id,
          float density,
          float friction,
          float restitution,
          const glm::vec2 &offset,
          float radius,
          float half_height)
          : PolygonShape(rigid_body_id, PolygonType::Capsule, density, friction, restitution, offset), m_Radius(radius),
            m_HalfHeight(half_height)
        {}

        ~CapsuleShape() { b2DestroyShape(m_RuntimeShapeID); }

        virtual void CreateShape() override
        {
            // Define the two endpoints of the capsule along the y-axis (vertical capsule)
            b2Vec2 point1 = { m_Offset.x, m_Offset.y - m_HalfHeight };
            b2Vec2 point2 = { m_Offset.x, m_Offset.y + m_HalfHeight };

            m_Shape.center1 = point1;
            m_Shape.center2 = point2;
            m_Shape.radius = m_Radius;

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = m_Density;
            shapeDef.friction = m_Friction;
            shapeDef.restitution = m_Restitution;
            m_RuntimeShapeID = b2CreateCapsuleShape(m_RigidBodyID, &shapeDef, &m_Shape);
        }

        private:
        b2Capsule m_Shape;
        float m_Radius;
        float m_HalfHeight;
    };

    class RigidBody
    {
        public:
        RigidBody(Entity *entity, b2WorldId world_id);
        ~RigidBody() { b2DestroyBody(m_RuntimeBodyID); }

        b2BodyId GetBodyID() { return m_RuntimeBodyID; }
        void AddPolygon(Entity *entity);
        void RemovePolygon(Entity *entity);

        private:
        b2ShapeId m_ShapeID;
        b2BodyId m_RuntimeBodyID;
        b2BodyType m_BodyType;
        std::vector<std::unique_ptr<PolygonShape>> m_Polygons;
    };

}// namespace SOF