#pragma once


namespace SOF
{
    class Camera
    {
        public:
        static std::shared_ptr<Camera> Create(float width, float height)
        {
            return std::make_shared<Camera>(width, height);
        }

        Camera(float width, float height);

        glm::vec3 &GetPosition() { return m_Position; }
        glm::mat4 &GetViewMatrix() { return m_ViewMatrix; }
        glm::mat4 &GetProjectionMatrix() { return m_ProjectionMatrix; }

        float GetHeight() { return m_Height; }
        float GetWidth() { return m_Width; }
        float GetZoomLevel() { return m_ZoomLevel; }

        void SetWidth(float width)
        {
            m_Width = width;
            SetOrthographic();
        }
        void SetHeight(float height)
        {
            m_Height = height;
            SetOrthographic();
        }
        void SetZoomLevel(float zoom)
        {
            m_ZoomLevel = zoom;
            SetOrthographic();
        }
        void SetPosition(glm::vec3 &position)
        {
            m_Position = position;
            RecalculateViewMatrix();
        }
        void Move(glm::vec3 &velocity)
        {
            m_Position += velocity;
            RecalculateViewMatrix();
        }

        private:
        void RecalculateViewMatrix();
        void SetOrthographic();

        private:
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix{ 1.f };

        glm::vec3 m_Position = { 0.f, 0.f, 0.f };

        float m_Width = 1920.f;
        float m_Height = 1080.f;
        float m_ZoomLevel = 100.f;
    };
}// namespace SOF
