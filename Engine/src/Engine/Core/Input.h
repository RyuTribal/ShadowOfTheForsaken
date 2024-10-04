#pragma once

namespace SOF
{
    class Input
    {
        public:
        static bool IsKeyPressed(uint32_t keycode);
        static bool IsMouseButtonPressed(uint32_t button);
        static glm::vec2 GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();

        protected:
        private:
    };
}// namespace SOF