#pragma once

#include "box2d/box2d.h"
#include "PhysicsBody.h"

namespace SOF
{
    class Scene;
    enum VelocityType { Linear, Angular };

    struct VelocityData
    {
        VelocityType Type = VelocityType::Linear;
        glm::vec3 Velocity = { 0.f, 0.f, 0.f };
    };
    class PhysicsWorld
    {
        public:
        static std::shared_ptr<PhysicsWorld> Create(Scene *context) { return std::make_shared<PhysicsWorld>(context); }
        static constexpr glm::vec2 DefaultGravity = { 0.f, -9.82f };
        PhysicsWorld(Scene *context);
        ~PhysicsWorld();

        void AddBody(Entity *entity);
        void AddPolygon(Entity *entity);
        void RemovePolygon(Entity *entity);
        void RemoveBody(Entity *entity);
        // Will first update the rigidbody position based on the transform
        void SetBodyDirty(Entity *entity);

        void SetVelocity(Entity *entity, const glm::vec3 &velocity, VelocityType type);
        glm::vec2 GetVelocity(Entity *entity);
        float GetAngle(Entity *entity);

        void StartSimulation();
        void PauseSimulation();

        void Step();

        void SetGravity(const glm::vec2 &new_gravity);

        float GetAirFriction() { return m_AirFriction; }
        // Useful in games with no real solid friction object beneath you
        void SetAirFriction(float friction);

        private:
        void UpdateDirtyBodies();


        private:
        b2WorldId m_WorldID;
        Scene *m_Context;
        glm::vec2 m_Gravity = DefaultGravity;
        std::unordered_map<UUID, std::unique_ptr<RigidBody>> m_RigidBodies;
        std::unordered_map<UUID, VelocityData> m_Velocities;
        std::vector<UUID> m_DirtyBodies;
        bool m_IsSimulating = false;
        float m_AirFriction = 0.0f;
    };
}// namespace SOF