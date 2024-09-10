#include "pch.h"
#include "Context.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace SOF
{
    Context::Context(GLFWwindow *window_context) : m_WindowHandle(window_context) {}

    void Context::Init()
    {
        glfwMakeContextCurrent(m_WindowHandle);
        int status = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
        SOF_ASSERT(status, "GLad did not load");

        SOF_TRACE("Renderer", "Vendor: {}", glGetString(GL_VENDOR));
        SOF_TRACE("Renderer", "Renderer: {}", glGetString(GL_RENDERER));
        SOF_TRACE("Renderer", "Version: {}", glGetString(GL_VERSION));
    }
    void Context::SwapBuffers() { glfwSwapBuffers(m_WindowHandle); }
    void Context::SetVSync(bool vsync) { glfwSwapInterval(vsync ? 1 : 0); }
    std::unique_ptr<Context> Context::Create(void *window)
    {
        return std::make_unique<Context>(std::forward<GLFWwindow *>(static_cast<GLFWwindow *>(window)));
    }
}// namespace SOF
