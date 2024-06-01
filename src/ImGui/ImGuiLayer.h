#pragma once

#include "Events/ApplicationEvents.h"
#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"

namespace SOF
{

	class ImGuiLayer
	{
	public:
		static void Init();
		static void Shutdown();

		static void OnEvent(Event& e);

		static void Begin();
		static void End();

		static void BlockEvents(bool block);
	};

}
