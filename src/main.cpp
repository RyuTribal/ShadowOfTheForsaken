#include "pch.h"
#include "Core/Game.h"
#include "Core/Window.h"

int main()
{

    SOF::Log::Init();
    SOF_WARN("Entrypoint", "Log initialized!");

    SOF::Window::WindowData window_settings{};
    window_settings.Title = "Shadow of the Forsaken";
    window_settings.VSync = false;
    window_settings.Height = 720;
    window_settings.Width = 1280;
    window_settings.Fullscreen = true;
    window_settings.FullScreenType = SOF::FullscreenType::WINDOWED;


    SOF::Game *game_instance = SOF::Game::CreateGame(window_settings);

    game_instance->Start();

    delete game_instance;
    SOF::Log::Shutdown();

    return 0;
}
