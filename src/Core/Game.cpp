#include "pch.h"
#include <glad/gl.h>
#include "Game.h"
#include "Events/DebugEvents.h"
#include "Renderer/Renderer.h"
#include <imgui.h>

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

	}

	Game* Game::CreateGame(const WindowProps& props)
	{
		return new Game(props);
	}

	void Game::Start()
	{
		float color[3] = { 0.f,  0.f, 0.f };
		while (m_Running) {
			Renderer::BeginFrame();
			Renderer::DrawSquare();
			Renderer::EndFrame();

#ifdef DEBUG
			ImGuiLayer::Begin();
			ImGuiUpdateEvent debug_event{};
			ImGui::Begin("WArsay");
			
			if(ImGui::ColorEdit3("Warsay color", color)){
				glm::vec3 color_vec(color[0], color[1], color[2]);
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