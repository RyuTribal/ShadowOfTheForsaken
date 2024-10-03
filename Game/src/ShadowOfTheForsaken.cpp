#include "pch.h"
#include <Engine/Engine.h>
#include <Engine/Core/EntryPoint.h>
#include "SOF/Game.h"

SOF::Game *SOF::CreateGame(int argc, char **argv)
{
    SOF::Window::WindowData window_settings{};
    window_settings.Title = "Shadow of the Forsaken";
    window_settings.VSync = false;
    window_settings.Height = 720;
    window_settings.Width = 1280;
    window_settings.Fullscreen = true;
    window_settings.FullScreenType = SOF::FullscreenType::WINDOWED;
    return new SOFGame(window_settings);
}