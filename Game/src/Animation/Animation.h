#pragma once
#include <Engine/Engine.h>

namespace SOF
{
    class AnimationSerializer : public IAssetDeserializerStrategy
    {
        public:
        struct Animation
        {
            uint32_t SpriteWidth = 32, SpriteHeight = 32;
            size_t FrameCount = 0;
            float FrameTime = 0.0f;
            char SpriteSheetHandle[256];
            std::vector<std::pair<uint32_t, uint32_t>> Frames;
        };
        virtual std::shared_ptr<Asset> Load(TOCEntry &toc, std::vector<char> &data) const override;
        static std::shared_ptr<AssetData> DumpData(AnimationSerializer::Animation &animation_data);
    };

    class Animation : public Asset
    {
        public:
        Animation(const std::vector<std::pair<uint32_t, uint32_t>> &frames, float frameDuration);

        void Update(float dt);
        std::pair<uint32_t, uint32_t> GetCurrentFrame() const;
        bool IsFinished() const;
        float GetTotalDuration() { return m_FrameDuration * m_Frames.size(); }
        float GetFrameDuration() { return m_FrameDuration; }
        std::vector<std::pair<uint32_t, uint32_t>> &GetFrames() { return m_Frames; }

        void Reset();

        virtual AssetType GetType() const override { return AssetType::Custom; };

        private:
        std::vector<std::pair<uint32_t, uint32_t>> m_Frames;
        float m_FrameDuration;
        float m_Timer = 0.0f;
        size_t m_CurrentFrame = 0;
    };
}// namespace SOF