#include "pch.h"
#include <glad/gl.h>
#include "Game.h"
#include "Events/DebugEvents.h"
#include "Scene/Entity.h"
#include "Renderer/Renderer.h"
#include "Renderer/Texture.h"
#include <imgui.h>
#include "Scene/Components.h"
#include "Asset/Manager.h"

namespace SOF {

	Game* Game::s_Instance = nullptr;

	Game::Game(const WindowProps& props)
	{
		SOF_ASSERT(!s_Instance, "Can only have one instance of a game!");
		m_Window = std::make_unique<Window>(props);
		s_Instance = this;
		m_Window->SetEventCallback(BIND_EVENT_FN(SOF::Game::OnEvent));
		Renderer::Init();
		ImGuiLayer::Init();
		AssetManager::Init("Assets.sofp");

		m_Scene = std::make_shared<Scene>("Test scene");

		// cReating warsay
		m_WarsayID = m_Scene->CreateEntity("WarsayBox");
		std::string warsay_asset_handle = "warsay_sprite";
		auto warsay_entity = m_Scene->GetEntity(m_WarsayID);
		TransformComponent warsay_transform = TransformComponent();
		SpriteComponent warsay_sprite = SpriteComponent(glm::vec4(0.f, 0.f, 0.f, 1.f));
		warsay_sprite.Texture = AssetManager::Load<Texture>(warsay_asset_handle);
		CameraComponent warsay_camera = CameraComponent(true);
		warsay_camera.Camera = Camera::Create(m_Window->GetWidth(), m_Window->GetHeight());
		warsay_entity->AddComponent<TransformComponent>(warsay_transform);
		warsay_entity->AddComponent<SpriteComponent>(warsay_sprite);
		warsay_entity->AddComponent<CameraComponent>(warsay_camera);


		// cReating warsay home
		int gridWidth = 10;
		int gridHeight = 10;
		float spacing = 1.0f;

		for (int x = 0; x < gridWidth; ++x) {
			for (int y = 0; y < gridHeight; ++y) {
				UUID entityID = m_Scene->CreateEntity("GridEntity_"+std::to_string(x) + "_" + std::to_string(y));
				auto entity = m_Scene->GetEntity(entityID);
				m_WarsayHome.push_back(entityID);
				TransformComponent transform;
				transform.Translation = glm::vec3(x * spacing, y * spacing, 0.0f);
				transform.Scale = glm::vec3(0.9f);

				SpriteComponent sprite;
				sprite.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				sprite.Texture = AssetManager::Load<Texture>(warsay_asset_handle);

				entity->AddComponent<TransformComponent>(transform);
				entity->AddComponent<SpriteComponent>(sprite);
			}
		}
		
		
	}

	Game* Game::CreateGame(const WindowProps& props)
	{
		return new Game(props);
	}

	void Game::Start()
	{
		float bg_color[3] = { 0.f,  0.f, 0.f };
		char asset_file_path[256];
		asset_file_path[0] = 0;
		char asset_handle[256];
		asset_handle[0] = 0;
		char deleted_asset_handle[256];
		deleted_asset_handle[0] = 0;

		auto last_frame = std::chrono::high_resolution_clock::now();

		while (m_Running) {

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - last_frame).count();
			last_frame = newTime;
			double currentTime = std::chrono::duration<double>(newTime.time_since_epoch()).count();
			m_FrameStats.UpdateFPS(currentTime, frameTime);

			m_Scene->Begin();
			m_Scene->Update();
			m_Scene->End();

#ifdef DEBUG
			ImGuiLayer::Begin();
			ImGuiUpdateEvent debug_event{};
			ImGui::Begin("Debug");

			if (ImGui::ColorEdit3("Background color", bg_color)) {
				glm::vec3 color_vec(bg_color[0], bg_color[1], bg_color[2]);
				Renderer::ChangeBackgroundColor(color_vec);
			}



			ImGui::Text("Register asset into the asset pack:");
			ImGui::InputText("File path", asset_file_path, 255);
			ImGui::InputText("Asset handle##register", asset_handle, 255);
			if (ImGui::Button("Register asset") && asset_handle[0] != 0 && asset_file_path[0] != 0) {
				std::string handle = std::string(asset_handle);
				AssetManager::RegisterAsset(asset_file_path, handle, AssetManager::FileToAssetType(asset_file_path));
				memset(asset_file_path, 0, sizeof(asset_file_path));
				memset(asset_handle, 0, sizeof(asset_handle));
			}

			ImGui::Text("Deregister asset from the asset pack:");
			ImGui::InputText("Asset handle##deregister", deleted_asset_handle, 255);
			if (ImGui::Button("Deregister asset") && deleted_asset_handle[0] != 0) {
				std::string handle = std::string(deleted_asset_handle);
				AssetManager::DeregisterAsset(handle);
				memset(deleted_asset_handle, 0, sizeof(deleted_asset_handle));
			}

			ImGui::End();

			ImGui::Begin("Stat");
			ImGui::Text("Scene stats:");
			ImGui::Text("	- Entity count: %i", m_Scene->EntitySize());

			ImGui::Text("Renderer stats:");
			ImGui::Text("	- FPS: %f", m_FrameStats.FPS);
			ImGui::Text("	- Quads drawn: %i", Renderer::GetStats().QuadsDrawn);
			ImGui::Text("	- Draw calls: %i", Renderer::GetStats().DrawCalls);
			ImGui::End();
#endif
			OnEvent(debug_event);
#ifdef DEBUG
			ImGuiLayer::End();
#endif
			m_Window->OnUpdate();
		}
	}

	bool Game::OnShutDown(WindowCloseEvent& event)
	{
		SOF_INFO("Game", "Shutting down \n");
		m_Running = false;

		m_Scene = nullptr; // So every entity with their asset is deleted

		Renderer::Shutdown();
		ImGuiLayer::Shutdown();
		AssetManager::Shutdown();
		return true;
	}

	bool Game::OnWindowResize(WindowResizeEvent& event)
	{
		Renderer::ResizeWindow();
		return true;
	}

	bool Game::OnKeyPressed(KeyPressedEvent& event)
	{
		glm::vec3 velocity = { 0.f, 0.f, 0.f };
		if (event.GetKeyCode() == GLFW_KEY_W) {
			velocity.y += 10.f * m_FrameStats.DeltaTime;
		}
		if (event.GetKeyCode() == GLFW_KEY_S) {
			velocity.y -= 10.f * m_FrameStats.DeltaTime;
		}
		if (event.GetKeyCode() == GLFW_KEY_A) {
			velocity.x -= 10.f * m_FrameStats.DeltaTime;
		}
		if (event.GetKeyCode() == GLFW_KEY_D) {
			velocity.x += 10.f * m_FrameStats.DeltaTime;
		}

		Renderer::GetCurrentCamera()->Move(velocity);
		return true;
	}

    void Game::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(SOF::Game::OnShutDown));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(SOF::Game::OnWindowResize));
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(SOF::Game::OnKeyPressed));

		for (auto [subscriber, callback] : m_Subscribers) {
			callback(event);
		}
	}

	Window& Game::GetWindow()
	{
		return *m_Window;
	}

	UUID Game::SubscribeOnEvents(std::function<void(Event&)> callback)
	{
		UUID subscriber = UUID();
		m_Subscribers[subscriber] = callback;
		return subscriber;
	}

	void Game::RevokeSubscription(UUID subscriber)
	{
		m_Subscribers.erase(subscriber);
	}
}