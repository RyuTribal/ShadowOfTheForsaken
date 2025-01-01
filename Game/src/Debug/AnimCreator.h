#pragma once

#include <Engine/Engine.h>

namespace SOF
{
    class AnimCreator
    {
        public:
        AnimCreator();
        static void Init();
        static void Shutdown();

        static void Render(float dt);

        private:
        std::shared_ptr<Texture> m_SpriteSheet = nullptr;
        std::string m_SpriteSheetHandle = "";
        std::string m_AnimationHandle = "";
        float m_FullDuration = 0.0f, m_FrameTime = 0.2f, m_CurrFrameTime = 0.0f;
        uint32_t m_SpriteWidth = 32, m_SpriteHeight = 32;
        std::pair<uint32_t, uint32_t> m_SelectedIndex = { 0, 0 };
        size_t m_CurrentAnimatedFrame = 0;
        bool m_IsSaving = false;
        bool m_IsOverwritting = false;
        std::vector<std::pair<uint32_t, uint32_t>> m_AnimationIndices;
        static AnimCreator* s_Instance;
    };
}// namespace SOF