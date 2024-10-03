#include "pch.h"
#include "Window.h"
#include "Game.h"
#include "Engine/Events/KeyEvents.h"
#include "Engine/Events/MouseEvents.h"
#include "Engine/Events/ApplicationEvents.h"

namespace SOF
{
    Window::Window(const WindowData &props) : m_Data(props)
    {
        int success = glfwInit();
        SOF_ASSERT(success, "Could not initialize GLFW!");
        m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, props.Title.c_str(), nullptr, nullptr);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
#ifdef DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
        glfwSetWindowUserPointer(m_Window, &m_Data);
        glfwSetErrorCallback(
          [](int error, const char *msg) { SOF_ERROR("Window", "GLFW error {0}: {1}", error, msg); });

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *window, int width, int height) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            data.Width = width;
            data.Height = height;
            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });
        glfwSetWindowMaximizeCallback(m_Window, [](GLFWwindow *window, int maximized) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            data.Fullscreen = (bool)maximized;
            data.FullScreenType = FullscreenType::WINDOWED;
        });
        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            WindowCloseEvent event;
            data.EventCallback(event);
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            switch (action) {
            case GLFW_PRESS: {
                KeyPressedEvent event(key, 0);
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                KeyReleasedEvent event(key);
                data.EventCallback(event);
                break;
            }
            case GLFW_REPEAT: {
                KeyPressedEvent event(key, 1);
                data.EventCallback(event);
                break;
            }
            }
        });

        glfwSetCharCallback(m_Window, [](GLFWwindow *window, unsigned int keycode) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            KeyTypedEvent event(keycode);
            data.EventCallback(event);
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button, int action, int mods) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

            switch (action) {
            case GLFW_PRESS: {
                MouseButtonPressedEvent event(button);
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                MouseButtonReleasedEvent event(button);
                data.EventCallback(event);
                break;
            }
            }
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double x_offset, double y_offset) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            MouseScrolledEvent event((float)x_offset, (float)y_offset);
            data.EventCallback(event);
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double xPos, double yPos) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            MouseMovedEvent event((float)xPos, (float)yPos);
            data.EventCallback(event);
        });
    }
    Window::~Window() { glfwDestroyWindow(m_Window); }

    void Window::OnUpdate() { glfwPollEvents(); }

    void Window::SetFullscreen(bool fullscreen, FullscreenType type)
    {
        m_Data.Fullscreen = fullscreen;
        m_Data.FullScreenType = type;
        if (!fullscreen || type == FullscreenType::WINDOWED) {
            glfwSetWindowAttrib(m_Window, GLFW_DECORATED, GLFW_TRUE);
            glfwSetWindowAttrib(m_Window, GLFW_RESIZABLE, GLFW_TRUE);
        }
        if (!fullscreen) {
            glfwSetWindowMonitor(m_Window, nullptr, m_XPos, m_YPos, m_PrevWidth, m_PrevHeight, 0);
        } else {
            GLFWmonitor *monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);

            switch (type) {
            case FullscreenType::FULLSCREEN:

                glfwGetWindowPos(m_Window, &m_XPos, &m_YPos);
                glfwGetWindowSize(m_Window, &m_PrevWidth, &m_PrevHeight);
                glfwSetWindowMonitor(m_Window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                break;
            case FullscreenType::BORDERLESS:
                glfwGetWindowPos(m_Window, &m_XPos, &m_YPos);
                glfwGetWindowSize(m_Window, &m_PrevWidth, &m_PrevHeight);
                monitor = glfwGetPrimaryMonitor();
                mode = glfwGetVideoMode(monitor);
                if (!mode) return;
                glfwSetWindowAttrib(m_Window, GLFW_DECORATED, GLFW_FALSE);
                glfwSetWindowAttrib(m_Window, GLFW_RESIZABLE, GLFW_FALSE);
                glfwSetWindowMonitor(m_Window, NULL, 0, 0, mode->width, mode->height, 0);
                break;
            case FullscreenType::WINDOWED:
                if (fullscreen) { glfwMaximizeWindow(m_Window); }
                break;
            }
        }
    }
}// namespace SOF
