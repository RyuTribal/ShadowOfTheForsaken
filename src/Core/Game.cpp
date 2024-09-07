#include "pch.h"
#include <glad/gl.h>
#include "Game.h"
#include "Events/DebugEvents.h"
#include "Scene/Entity.h"
#include "Renderer/Renderer.h"
#include <imgui.h>
#include <Scene/Components.h>

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

		m_Scene = std::make_shared<Scene>("Test scene");

		m_WarsayID = m_Scene->CreateEntity("WarsayBox");
		auto warsay_entity = m_Scene->GetEntity(m_WarsayID);
		TransformComponent warsay_transform = TransformComponent();
		SpriteComponent warsay_sprite = SpriteComponent(glm::vec4(0.f, 0.f, 0.f, 1.f));
		warsay_entity->AddComponent<TransformComponent>(warsay_transform);
		warsay_entity->AddComponent<SpriteComponent>(warsay_sprite);

	}

	Game* Game::CreateGame(const WindowProps& props)
	{
		return new Game(props);
	}

	void Game::Start()
	{
		float bg_color[3] = { 0.f,  0.f, 0.f };
		while (m_Running) {
			m_Scene->Begin();
			m_Scene->Update();
			m_Scene->End();

#ifdef DEBUG
			ImGuiLayer::Begin();
			ImGuiUpdateEvent debug_event{};
			ImGui::Begin("Debug");
			
			if(ImGui::ColorEdit3("Background color", bg_color)){
				glm::vec3 color_vec(bg_color[0], bg_color[1], bg_color[2]);
				Renderer::ChangeBackgroundColor(color_vec);
			}

			ImGui::End();
			OnEvent(debug_event);
			ImGuiLayer::End();
#endif
			m_Window->OnUpdate();
		}
	}

	bool Game::OnShutDown(WindowCloseEvent& event)
	{
		SOF_INFO("Game", "Shutting down \n");
		m_Running = false;
		Renderer::Shutdown();
		ImGuiLayer::Shutdown();
		return true;
	}

    void Game::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(SOF::Game::OnShutDown));

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