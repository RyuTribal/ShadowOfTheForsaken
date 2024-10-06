#include "pch.h"
#include "Player.hpp"
#include "Game.h"

namespace SOF
{
    Player::Player(const std::string &name,
      const std::string &sprite_sheet,
      const glm::vec2 &sprite_size,
      float speed,
      Scene *context)
      : m_PlayerController{ std::make_unique<PlayerController>(speed) }
    {
        SOFGame *game_instance = (SOFGame *)Game::Get();
        auto texture = AssetManager::Load<Texture>(sprite_sheet);
        UUID entity_id = context->CreateEntity(name);
        m_Entity = context->GetEntity(entity_id);
        TransformComponent transform = TransformComponent();
        SpriteComponent sprite;
        sprite.TextureRef = texture;
        sprite.SpriteSize = sprite_size;
        sprite.Layer = Player::CharacterLayer;
        CameraComponent camera = CameraComponent(true);
        camera.ClipToTransform = true;
        camera.CameraRef =
          Camera::Create((float)game_instance->GetWindow().GetWidth(), (float)game_instance->GetWindow().GetHeight());
        Rigidbody2DComponent rigid_body{};
        rigid_body.Type = ColliderType::DYNAMIC;
        rigid_body.FixedRotation = true;
        CapsuleCollider2DComponent capsule_collider{};
        capsule_collider.HalfHeight = sprite.SpriteSize.y * 0.5f;
        m_Entity->AddComponent<TransformComponent>(transform);
        m_Entity->AddComponent<SpriteComponent>(sprite);
        m_Entity->AddComponent<CameraComponent>(camera);
        m_Entity->AddComponent<Rigidbody2DComponent>(rigid_body);
        m_Entity->AddComponent<CapsuleCollider2DComponent>(capsule_collider);
    }

    void Player::Update(float dt)
    {
        m_PlayerController->UpdateMovement(m_Entity);
        m_LocomotionStateMachine->Update(dt);
        const glm::vec2 current_sprite = m_LocomotionStateMachine->GetReleventSpriteIndex();
        m_Entity->GetComponent<SpriteComponent>()->SetCoordinate(0, 0, current_sprite);
    }

    void Player::CreateLocomotionStateMachine(const LocomotionSettings &settings)
    {
        m_LocomotionStateMachine.emplace(
          [this](StateMachine *machine, float dt) { this->OnLocomotionUpdate(machine, dt); });

        UUID idle_left_id = m_LocomotionStateMachine->AddState("IdleLeft", settings.IdleLeft, true, true);
        UUID idle_right_id = m_LocomotionStateMachine->AddState("IdleRight", settings.IdleRight, true, false);
        UUID idle_up_id = m_LocomotionStateMachine->AddState("IdleUp", settings.IdleUp, true, false);
        UUID idle_down_id = m_LocomotionStateMachine->AddState("IdleDown", settings.IdleDown, true, false);

        UUID left_id = m_LocomotionStateMachine->AddState("WalkLeft", settings.WalkLeft, true, false);
        UUID right_id = m_LocomotionStateMachine->AddState("WalkRight", settings.WalkRight, true, false);
        UUID up_id = m_LocomotionStateMachine->AddState("WalkUp", settings.WalkUp, true, false);
        UUID down_id = m_LocomotionStateMachine->AddState("WalkDown", settings.WalkDown, true, false);

        auto is_moving_up = [](StateMachine *machine) {
            return machine->GetVariable<glm::vec2>("velocity").y > 0.0f
                   && machine->GetVariable<glm::vec2>("velocity").x == 0.0f;
        };
        auto is_moving_down = [](StateMachine *machine) {
            return machine->GetVariable<glm::vec2>("velocity").y < 0.0f
                   && machine->GetVariable<glm::vec2>("velocity").x == 0.0f;
        };
        auto is_moving_left = [](
                                StateMachine *machine) { return machine->GetVariable<glm::vec2>("velocity").x < 0.0f; };
        auto is_moving_right = [](StateMachine *machine) {
            return machine->GetVariable<glm::vec2>("velocity").x > 0.0f;
        };

        std::vector<UUID> idle_states = { idle_left_id, idle_right_id, idle_up_id, idle_down_id };
        for (UUID idle_state : idle_states) {
            m_LocomotionStateMachine->AddTransition(idle_state, up_id, is_moving_up);
            m_LocomotionStateMachine->AddTransition(idle_state, down_id, is_moving_down);
            m_LocomotionStateMachine->AddTransition(idle_state, left_id, is_moving_left);
            m_LocomotionStateMachine->AddTransition(idle_state, right_id, is_moving_right);
        }

        m_LocomotionStateMachine->AddTransition(left_id, idle_left_id, [this](StateMachine *machine) {
            return machine->GetVariable<glm::vec2>("velocity").x >= 0.0f;
        });
        m_LocomotionStateMachine->AddTransition(right_id, idle_right_id, [this](StateMachine *machine) {
            return machine->GetVariable<glm::vec2>("velocity").x <= 0.0f;
        });
        m_LocomotionStateMachine->AddTransition(up_id, idle_up_id, [this](StateMachine *machine) {
            return machine->GetVariable<glm::vec2>("velocity").y <= 0.0f;
        });
        m_LocomotionStateMachine->AddTransition(down_id, idle_down_id, [this](StateMachine *machine) {
            return machine->GetVariable<glm::vec2>("velocity").y >= 0.0f;
        });
    }


    UUID Player::GetID()
    {

        SOF_ASSERT(m_Entity, "Entity is a nullptr!");
        return m_Entity->GetHandle();
    }
    void Player::OnLocomotionUpdate(StateMachine *machine, float dt)
    {
        machine->SetVariable<glm::vec2>("velocity", m_Entity->GetScene()->GetPhysicsWorld()->GetVelocity(m_Entity));
    }
}// namespace SOF
