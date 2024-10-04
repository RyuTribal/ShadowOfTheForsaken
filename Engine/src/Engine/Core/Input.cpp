#include "pch.h"
#include "Input.h"
#include "Engine/Core/Game.h"

namespace SOF
{
    bool Input::IsKeyPressed(uint32_t keycode)
    {
        auto *window = static_cast<GLFWwindow *>(Game::Get()->GetWindow().GetNativeWindow());
        auto state = glfwGetKey(window, static_cast<int32_t>(keycode));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }
    bool Input::IsMouseButtonPressed(uint32_t button)
    {
        auto window = static_cast<GLFWwindow *>(Game::Get()->GetWindow().GetNativeWindow());
        auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }
    glm::vec2 Input::GetMousePosition()
    {
        auto *window = static_cast<GLFWwindow *>(Game::Get()->GetWindow().GetNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        return { (float)xpos, (float)ypos };
    }
    float Input::GetMouseX() { return GetMousePosition().x; }
    float Input::GetMouseY() { return GetMousePosition().y; }
}// namespace SOF