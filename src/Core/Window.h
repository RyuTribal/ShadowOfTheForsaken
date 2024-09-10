#pragma once
#include "Renderer/Context.h"
#include <GLFW/glfw3.h>
#include "Events/Event.h"

namespace SOF {

struct WindowProps;

class Window
{
  public:
    Window(const WindowProps &props);
    ~Window();
    void OnUpdate();
    void SetEventCallback(const std::function<void(Event &)> &callback) { m_Data.EventCallback = callback; }
    int GetHeight() { return m_Data.Height; }
    int GetWidth() { return m_Data.Width; }
    GLFWwindow *GetNativeWindow() { return m_Window; }

  private:
    struct WindowData
    {
        std::string Title = "Untitled Window";
        bool VSync = true;
        unsigned int Width = 1280, Height = 720;
        std::function<void(Event &)> EventCallback;
    };

    std::unique_ptr<Context> m_Context;
    GLFWwindow *m_Window;
    WindowData m_Data;
};
}// namespace SOF
