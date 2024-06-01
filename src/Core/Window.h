#pragma once
#include "Renderer/Context.h"
#include <GLFW/glfw3.h>

namespace SOF{
    struct WindowProps{
        std::string Title = "Untitled Window";
        bool VSync = true;
        unsigned int Width = 1280, Height = 720;
        std::function<void(int, int)> KeyEvent;
        std::function<void(float,float)> MouseEvent;
        std::function<void()> ShutDownEvent;
    };

    class Window{
    public:
        Window(const WindowProps& props);
        ~Window();
        void OnUpdate();
    private:

        std::unique_ptr<Context> m_Context;
        GLFWwindow* m_Window;
        WindowProps m_Data;
    };
}