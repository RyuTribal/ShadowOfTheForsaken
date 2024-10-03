#pragma once
#include <glm/gtx/quaternion.hpp>
#include "Renderer/Texture.h"
#include "Renderer/Camera.h"
#include "box2d/box2d.h"

namespace SOF
{

    enum ColliderType {
        STATIC,// Something that isn't affected by forces but can be collided with such as walls
        DYNAMIC,// Something that is affected by forces and can be collided with, such as players
        KINEMATIC// Something that is affected by a set velocity but can't be collided with
    };

    enum SoundType {
        DEFAULT,// Will just play at the volume set in the sound engine
        SPATIAL// Will vary in volume depending on the transform component of the entity
    };


    struct TagComponent
    {
        std::string Tag = "";

        TagComponent() = default;
        TagComponent(const TagComponent &) = default;
        TagComponent(const std::string &new_tag) : Tag(new_tag) {}
    };

    struct TransformComponent
    {
        glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

        TransformComponent() = default;
        TransformComponent(const TransformComponent &) = default;
        TransformComponent(const glm::vec3 &new_translation) : Translation(new_translation) {}

        glm::mat4 CreateMat4x4()
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

            return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
        }

        glm::quat RotationVecToQuat() const { return glm::quat(Rotation); }

        glm::vec2 Get2DPosition() const { return glm::vec2(Translation.x, Translation.y); }
    };

    struct SpriteComponent
    {
        glm::vec4 Color = { 1.f, 1.f, 1.f, 1.f };
        std::shared_ptr<Texture> Texture = nullptr;
        glm::vec2 SpriteCoordinates = { 0.f, 0.f };
        glm::vec2 SpriteSize = { 32.f, 32.f };
        int32_t Layer = 0;

        SpriteComponent() = default;
        SpriteComponent(const SpriteComponent &) = default;
        SpriteComponent(const glm::vec4 &new_color) : Color(new_color) {}
        SpriteComponent(const glm::vec4 &new_color, glm::vec2 &sprite_coords, glm::vec2 &sprite_size)
          : Color(new_color), SpriteCoordinates(sprite_coords), SpriteSize(sprite_size)
        {}
    };

    struct CameraComponent
    {
        std::shared_ptr<Camera> Camera = nullptr;
        bool IsActive = false;
        CameraComponent() = default;
        CameraComponent(const CameraComponent &) = default;
        CameraComponent(bool is_active) : IsActive(is_active) {}
    };

    struct Rigidbody2DComponent
    {
        ColliderType Type = ColliderType::STATIC;
        bool FixedRotation = false;
        b2ShapeId ShapeID;
        b2BodyId RuntimeBodyID;

        Rigidbody2DComponent() = default;
        Rigidbody2DComponent(const Rigidbody2DComponent &) = default;
    };

    struct BoxCollider2DComponent
    {
        glm::vec2 Offset = { 0.0f, 0.0f };
        glm::vec2 Size = { 0.5f, 0.5f };
        float Density = 1.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;
        float RestitutionThreshold = 0.5f;
        b2ShapeId RuntimeShapeID;
        b2Polygon Shape;


        BoxCollider2DComponent() = default;
        BoxCollider2DComponent(const BoxCollider2DComponent &) = default;
    };

    struct CapsuleCollider2DComponent
    {
        glm::vec2 Offset = { 0.0f, 0.0f };
        float Radius = 0.5f;
        float Height = 1.0f;
        float Density = 1.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;
        float RestitutionThreshold = 0.5f;
        b2ShapeId RuntimeShapeID;
        b2Capsule Shape;

        CapsuleCollider2DComponent() = default;
        CapsuleCollider2DComponent(const CapsuleCollider2DComponent &) = default;
    };

    struct CircleCollider2DComponent
    {
        glm::vec2 Offset = { 0.0f, 0.0f };
        float Radius = 0.5f;
        float Density = 1.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;
        float RestitutionThreshold = 0.5f;
        b2ShapeId RuntimeShapeID;
        b2Circle Shape;

        CircleCollider2DComponent() = default;
        CircleCollider2DComponent(const CircleCollider2DComponent &) = default;
    };

    struct SoundComponent
    {
        std::string AssetHandle = "";
        UUID InstanceID;
        bool Loop = false;
        SoundType Type = SoundType::DEFAULT;

        SoundComponent() = default;
        SoundComponent(const SoundComponent &) = default;
    };

}// namespace SOF