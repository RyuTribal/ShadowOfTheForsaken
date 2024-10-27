#include "pch.h"
#include "Camera.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace SOF
{
    Camera::Camera(float width, float height)
    {
        m_Width = width;
        m_Height = height;
        SetOrthographic();
    }

    void Camera::RecalculateViewMatrix()
    {
        m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position);
        m_ViewMatrix = glm::inverse(m_ViewMatrix);
    }

    void Camera::SetOrthographic()
    {
        float half_width = m_Width * 0.5f / m_ZoomLevel;
        float half_height = m_Height * 0.5f / m_ZoomLevel;

        m_ProjectionMatrix = glm::ortho(-half_width, half_width, -half_height, half_height, -1.0f, 1.0f);

        float left = -1.0f / m_ProjectionMatrix[0][0];
        float right = 1.0f / m_ProjectionMatrix[0][0];
        float bottom = -1.0f / m_ProjectionMatrix[1][1];
        float top = 1.0f / m_ProjectionMatrix[1][1];

        m_WorldWidth = right - left;
        m_WorldHeight = top - bottom;
    }
}// namespace SOF