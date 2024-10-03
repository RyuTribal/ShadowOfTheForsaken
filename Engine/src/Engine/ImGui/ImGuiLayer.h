#pragma once

#include "Engine/Events/ApplicationEvents.h"
#include "Engine/Events/KeyEvents.h"
#include "Engine/Events/MouseEvents.h"

namespace SOF
{

    class ImGuiLayer
    {
        public:
        static void Init();
        static void Shutdown();

        static void OnEvent(Event &e);

        static void Begin();
        static void End();

        static void BlockEvents(bool block);

        private:
    };

}// namespace SOF
