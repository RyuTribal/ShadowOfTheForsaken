#pragma once
#include <Engine/Engine.h>

namespace SOF
{
    class Animation
    {
        public:
        Animation(const std::vector<glm::vec2> &frames, float frameDuration);

        void Update(float dt);
        glm::vec2 GetCurrentFrame() const;
        bool IsFinished() const;
        float GetTotalDuration() { return m_FrameDuration * m_Frames.size(); }

        void Reset();

        private:
        std::vector<glm::vec2> m_Frames;
        float m_FrameDuration;
        float m_Timer = 0.0f;
        size_t m_CurrentFrame = 0;
    };
}// namespace SOF