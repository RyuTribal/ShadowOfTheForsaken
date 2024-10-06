#include "pch.h"
#include "Game.h"
#include <string>
namespace SOF
{
    void SOFGame::OnGameStart()
    {
        m_Scene = std::make_shared<Scene>("Test scene");
        m_Scene->SetBackground("background");
        Renderer::ChangeBackgroundColor({ 1.f, 1.f, 1.f });

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

        float anim_time = 0.2f;
        std::shared_ptr<Animation> Idle_Up =
          std::make_shared<Animation>(std::vector<glm::vec2>{ { 1.f, 4.f } }, anim_time);
        std::shared_ptr<Animation> Idle_Down =
          std::make_shared<Animation>(std::vector<glm::vec2>{ { 1.f, 7.f } }, anim_time);
        std::shared_ptr<Animation> Idle_Right =
          std::make_shared<Animation>(std::vector<glm::vec2>{ { 1.f, 5.f } }, anim_time);
        std::shared_ptr<Animation> Idle_Left =
          std::make_shared<Animation>(std::vector<glm::vec2>{ { 1.f, 6.f } }, anim_time);

        std::shared_ptr<Animation> Walk_Up =
          std::make_shared<Animation>(std::vector<glm::vec2>{ { 0.f, 4.f }, { 1.f, 4.f }, { 2.f, 4.f } }, anim_time);
        std::shared_ptr<Animation> Walk_Down =
          std::make_shared<Animation>(std::vector<glm::vec2>{ { 0.f, 7.f }, { 1.f, 7.f }, { 2.f, 7.f } }, anim_time);
        std::shared_ptr<Animation> Walk_Right =
          std::make_shared<Animation>(std::vector<glm::vec2>{ { 0.f, 5.f }, { 1.f, 5.f }, { 2.f, 5.f } }, anim_time);
        std::shared_ptr<Animation> Walk_Left =
          std::make_shared<Animation>(std::vector<glm::vec2>{ { 0.f, 6.f }, { 1.f, 6.f }, { 2.f, 6.f } }, anim_time);

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
    void SOFGame::OnDebugUpdate() { m_DebugWindow.Render(m_Scene.get()); }
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
