#include "pch.h"
#include "Game.h"
#include "Animation/Animation.h"

namespace SOF
{
    void SOFGame::OnGameStart()
    {

        AssetManager::DeregisterAsset("");
        m_Scene = std::make_shared<Scene>("Test scene");
        m_Scene->SetBackground("background");
        Renderer::ChangeBackgroundColor({ 1.f, 1.f, 1.f });

        // Don't need to delete it since it will be turned into a unique pointer when registering
        AnimationSerializer *anim_serializer = new AnimationSerializer();
        AssetManager::RegisterCustomAssetType("Animation", nullptr, anim_serializer);

        // Load textures
        std::string grounds_asset_handle = "grounds";
        auto grounds_texture_promise = m_ThreadPool.AddTask(AssetManager::Load<Texture>, grounds_asset_handle);
        auto grounds_texture = grounds_texture_promise.get();

        // cReating warsay home

        float gridWidth = 10.f;
        float gridHeight = 10.f;

        UUID floor_entity_id = m_Scene->CreateEntity("Floor");
        auto floor_entity = m_Scene->GetEntity(floor_entity_id);
        TransformComponent floor_transform;
        floor_transform.LocalTranslation = glm::vec3(0.0f, 0.0f, 0.0f);
        floor_transform.LocalScale = glm::vec3(gridWidth, gridHeight, 0.0f);
        SpriteComponent floor_sprite;
        floor_sprite.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        floor_sprite.TextureRef = grounds_texture;
        floor_sprite.SetTiles({ gridWidth, gridHeight });
        floor_sprite.SetCoordinate(1, 0, { 0.f, 3.f });
        floor_entity->AddComponent<TransformComponent>(floor_transform);
        floor_entity->AddComponent<SpriteComponent>(floor_sprite);

        // cReating warsay

        // Load in all animations
        std::shared_ptr<Animation> Idle_Up = AssetManager::Load<Animation>("idle_up");
        std::shared_ptr<Animation> Idle_Down = AssetManager::Load<Animation>("idle_down");
        std::shared_ptr<Animation> Idle_Right = AssetManager::Load<Animation>("idle_right");
        std::shared_ptr<Animation> Idle_Left = AssetManager::Load<Animation>("idle_left");

        std::shared_ptr<Animation> Walk_Up = AssetManager::Load<Animation>("walk_up");
        std::shared_ptr<Animation> Walk_Down = AssetManager::Load<Animation>("walk_down");
        std::shared_ptr<Animation> Walk_Right = AssetManager::Load<Animation>("walk_right");
        std::shared_ptr<Animation> Walk_Left = AssetManager::Load<Animation>("walk_left");

        m_Player.emplace("Warsay", "actors", glm::vec2(72.f, 96.f), 3.0f, m_Scene.get());
        m_Player->CreateLocomotionStateMachine(
          { Idle_Left, Idle_Right, Idle_Up, Idle_Down, Walk_Left, Walk_Right, Walk_Up, Walk_Down });

        m_Scene->GetPhysicsWorld()->SetGravity({ 0.f, 0.f });// We won't really have a gravity in a jrpg like world
        m_Scene->GetPhysicsWorld()->SetAirFriction(1.f);// Since we don't have a "ground" so to speak
        m_Scene->SetListenerEntity(m_Player->GetID());
    }
    void SOFGame::OnGameShutdown() {}
    void SOFGame::OnGameUpdate(float delta_time)
    {
        m_Scene->Begin();
        m_Player->Update(delta_time);
        m_Scene->Update();
        m_Scene->End();
    }
    void SOFGame::OnDebugUpdate(float delta_time) { m_DebugWindow.Render(m_Scene.get(), delta_time); }
    void SOFGame::OnGameEvent(Event &event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(SOF::SOFGame::OnKeyPressedEvent));
    }

    bool SOFGame::OnKeyPressedEvent(KeyPressedEvent &event)
    {
        if (m_DebugWindow.IsWindowActive()) { return false; }
        return true;
    }
}// namespace SOF
