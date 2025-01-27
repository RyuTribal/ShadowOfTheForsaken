#pragma once

struct GLFWwindow;
namespace SOF
{
    class Context
    {
        public:
        Context(GLFWwindow *window_context);

        void Init();
        GLFWwindow *Handle() { return m_WindowHandle; }
        void SwapBuffers();
        void SetVSync(bool vsync);
        static std::unique_ptr<Context> Create(void *window);

        private:
        GLFWwindow *m_WindowHandle;
    };
}// namespace SOF
