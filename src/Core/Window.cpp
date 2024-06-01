#include "pch.h"
#include "Window.h"

namespace SOF{
    Window::Window(const WindowProps& props)
    {
        int success = glfwInit();
		assert(success && "Could not initialize GLFW!");
        m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, props.Title.c_str(), nullptr, nullptr);
        m_Context = Context::Create(m_Window);
        m_Context->Init();
		glfwSwapInterval(props.VSync ? 1 : 0);
        m_Data = props;
        glfwSetWindowUserPointer(m_Window, &m_Data);
        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods){
            WindowProps& props = *(WindowProps*)glfwGetWindowUserPointer(window);
            props.KeyEvent(key, action);
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos){
           WindowProps& props = *(WindowProps*)glfwGetWindowUserPointer(window);
           
           props.MouseEvent((float) xpos, (float) ypos);
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods){
            WindowProps& props = *(WindowProps*)glfwGetWindowUserPointer(window);
            props.KeyEvent(button, action);
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window){
            WindowProps& props = *(WindowProps*)glfwGetWindowUserPointer(window);
            props.ShutDownEvent();
        });
    }
    Window::~Window()
    {
        glfwDestroyWindow(m_Window);
    }

    void Window::OnUpdate()
    {
        glfwPollEvents();
        m_Context->SwapBuffers();
    }
}