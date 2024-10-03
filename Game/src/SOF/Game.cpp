#include "pch.h"
#include "Game.h"
#include <string>

namespace SOF
{
    void SOFGame::OnGameStart()
    {
        m_Scene = std::make_shared<Scene>("Test scene");

        // Load textures
        std::string warsay_asset_handle = "black_idle";
        std::string grounds_asset_handle = "grounds";
        auto warsay_texture_promise = m_ThreadPool.AddTask(AssetManager::Load<Texture>, warsay_asset_handle);
        auto grounds_texture_promise = m_ThreadPool.AddTask(AssetManager::Load<Texture>, grounds_asset_handle);
        m_ThreadPool.Await();
        auto warsay_texture = warsay_texture_promise.get();
        auto grounds_texture = grounds_texture_promise.get();

        // cReating warsay home
        int gridWidth = 100;
        int gridHeight = 100;
        float spacing = 1.0f;

        for (int x = 0; x < gridWidth; ++x) {
            for (int y = 0; y < gridHeight; ++y) {
                UUID entityID = m_Scene->CreateEntity("GridEntity_" + std::to_string(x) + "_" + std::to_string(y));
                auto entity = m_Scene->GetEntity(entityID);
                m_WarsayHome.push_back(entityID);
                TransformComponent transform;
                transform.Translation = glm::vec3(x * spacing, y * spacing, 0.0f);

                SpriteComponent sprite;
                sprite.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                sprite.Texture = grounds_texture;
                sprite.SpriteCoordinates = glm::vec2(3.f, 4.f);
                entity->AddComponent<TransformComponent>(transform);
                entity->AddComponent<SpriteComponent>(sprite);

                // if (x == gridWidth - 1 && y == gridHeight - 1) {
                if (x == 70 && y == 70) {
                    SoundComponent sound_comp;
                    sound_comp.Loop = true;
                    sound_comp.AssetHandle = "sound_test";
                    sound_comp.Type = SoundType::SPATIAL;
                    entity->AddComponent<SoundComponent>(sound_comp);
                    // Should not be here, but for testing it works
                    auto sound_ref = entity->GetComponent<SoundComponent>();
                    auto transform_ref = entity->GetComponent<TransformComponent>();
                    sound_ref->InstanceID = SoundEngine::PlayAudio(sound_ref, transform_ref->Translation);
                }
            }
        }

        // cReating warsay
        m_WarsayID = m_Scene->CreateEntity("WarsayBox");

        auto warsay_entity = m_Scene->GetEntity(m_WarsayID);
        TransformComponent warsay_transform = TransformComponent();
        SpriteComponent warsay_sprite = SpriteComponent(glm::vec4(1.f, 0.f, 0.f, 1.f));
        warsay_sprite.Texture = warsay_texture;
        warsay_sprite.SpriteSize = glm::vec2(128.f, 128.f);
        warsay_sprite.Layer = 1;
        CameraComponent warsay_camera = CameraComponent(true);
        warsay_camera.Camera = Camera::Create((float)GetWindow().GetWidth(), (float)GetWindow().GetHeight());
        Rigidbody2DComponent rigid_body{};
        rigid_body.Type = ColliderType::DYNAMIC;// Currently just falls so keep this commented
        CapsuleCollider2DComponent capsule_collider{};
        capsule_collider.Height = warsay_sprite.SpriteSize.y;
        warsay_entity->AddComponent<TransformComponent>(warsay_transform);
        warsay_entity->AddComponent<SpriteComponent>(warsay_sprite);
        warsay_entity->AddComponent<CameraComponent>(warsay_camera);
        warsay_entity->AddComponent<Rigidbody2DComponent>(rigid_body);
        warsay_entity->AddComponent<CapsuleCollider2DComponent>(capsule_collider);

        m_Scene->CreatePhysicsWorld();

        m_Scene->SetGravity({ 0.f, 0.f });// We won't really have a gravity in a jrpg like world
        m_Scene->SetListenerEntity(m_WarsayID);
    }
    void SOFGame::OnGameShutdown() {}
    void SOFGame::OnGameUpdate(float delta_time)
    {
        m_Scene->Begin();
        m_Scene->Update();
        m_Scene->End();
    }
    void SOFGame::OnDebugUpdate() { m_DebugWindow.Render(m_Scene.get()); }
    void SOFGame::OnGameEvent(Event &event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(SOF::SOFGame::OnKeyPressedEvent));
    }
    bool SOFGame::OnKeyPressedEvent(KeyPressedEvent &event) { return false; }
}// namespace SOF