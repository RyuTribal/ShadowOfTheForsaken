#pragma once
#include "Renderer/Context.h"
#include <GLFW/glfw3.h>
#include "Events/Event.h"

namespace SOF
{

    enum FullscreenType { FULLSCREEN, BORDERLESS, WINDOWED };

    class Window
    {
        public:
        struct WindowData
        {
            std::string Title = "Untitled Window";
            bool VSync = true;
            unsigned int Width = 1280, Height = 720;
            std::function<void(Event &)> EventCallback;
            bool Fullscreen;
            FullscreenType FullScreenType;
        };
        Window(const WindowData &props);
        ~Window();
        void OnUpdate();
        void SetEventCallback(const std::function<void(Event &)> &callback) { m_Data.EventCallback = callback; }
        int GetHeight() { return m_Data.Height; }
        int GetWidth() { return m_Data.Width; }
        GLFWwindow *GetNativeWindow() { return m_Window; }
        void SetFullscreen(bool fullscreen, FullscreenType type);

        bool GetFullScreen() { return m_Data.Fullscreen; }
        FullscreenType GetFullScreenType() { return m_Data.FullScreenType; }

        private:
        GLFWwindow *m_Window;
        WindowData m_Data;
        int m_XPos = 0, m_YPos = 0, m_PrevWidth, m_PrevHeight;
    };
}// namespace SOF
