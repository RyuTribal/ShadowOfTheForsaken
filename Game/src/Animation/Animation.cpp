#include "pch.h"
#include "Animation.h"

namespace SOF
{
    Animation::Animation(const std::vector<glm::vec2> &frames, float frameDuration)
      : m_Frames(frames), m_FrameDuration(frameDuration), m_CurrentFrame(0), m_Timer(0.0f)
    {}

    void Animation::Update(float dt)
    {
        if (m_CurrentFrame >= m_Frames.size()) { return; }// Animation is finished
        m_Timer += dt;
        if (m_Timer >= m_FrameDuration) {
            m_Timer = 0.0f;
            m_CurrentFrame += 1;
        }
    }

    glm::vec2 Animation::GetCurrentFrame() const { return m_Frames[m_CurrentFrame]; }

    void Animation::Reset()
    {
        m_CurrentFrame = 0;
        m_Timer = 0.0f;
    }

    bool Animation::IsFinished() const { return m_Frames.size() - 1 == m_CurrentFrame; }
}// namespace SOF