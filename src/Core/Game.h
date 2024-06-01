#pragma once
#include "Events/ApplicationEvents.h"
#include "Window.h"
#include "ImGui/ImGuiLayer.h"

namespace SOF {

	class Window;

	struct WindowProps {
		std::string Title = "Untitled Window";
		bool VSync = true;
		unsigned int Width = 1280, Height = 720;
	};

	class Game {
	public:
		static Game* CreateGame(const WindowProps& props);

		void Start();
		bool OnShutDown(WindowCloseEvent& event);

		void OnEvent(Event& event);

		static Game* Get() { return s_Instance; }

		Window& GetWindow();

		UUID SubscribeOnEvents(std::function<void(Event&)> callback);
		void RevokeSubscription(UUID subscriber);

	private:
		Game(const WindowProps& props);
		bool m_Running = true;
		std::unique_ptr<Window> m_Window;
		static Game* s_Instance;
		std::unordered_map<UUID, std::function<void(Event&)>> m_Subscribers{};
	};
}