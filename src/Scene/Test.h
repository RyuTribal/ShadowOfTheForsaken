#pragma once
#include "Events/Subscriber.h"
#include <Events/DebugEvents.h>
#include <imgui.h>

namespace SOF {
	class Test : public Subscriber {
	public:
		Test() {
			Subscribe(BIND_EVENT_FN(SOF::Test::OnEvent));
		}

		void OnEvent(Event& event) {
			EventDispatcher dispatcher(event);
			dispatcher.Dispatch<ImGuiUpdateEvent>(BIND_EVENT_FN(SOF::Test::OnImGuiRender));
		}

		bool OnImGuiRender(ImGuiUpdateEvent& event) {
			ImGui::Begin("Debug window");
			ImGui::Text("Hey");
			ImGui::Button("Click if gay");
			ImGui::End();

			return true;
		}
	};
}