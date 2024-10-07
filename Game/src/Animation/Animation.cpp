#include "pch.h"
#include "Animation.h"
#include <nlohmann/json.hpp>

namespace SOF
{
    Animation::Animation(const std::vector<std::pair<uint32_t, uint32_t>> &frames, float frameDuration)
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

    std::pair<uint32_t, uint32_t> Animation::GetCurrentFrame() const { return m_Frames[m_CurrentFrame]; }

    void Animation::Reset()
    {
        m_CurrentFrame = 0;
        m_Timer = 0.0f;
    }

    bool Animation::IsFinished() const { return m_Frames.size() - 1 == m_CurrentFrame; }

    std::shared_ptr<Asset> AnimationSerializer::Load(TOCEntry &toc, std::vector<char> &data) const
    {
        auto bson_data = std::vector<uint8_t>(data.begin(), data.end());
        nlohmann::json json_data = nlohmann::json::from_bson(bson_data);
        std::vector<std::pair<uint32_t, uint32_t>> frames =
          json_data["Frames"].get<std::vector<std::pair<uint32_t, uint32_t>>>();

        float frame_time = json_data["FrameTime"].get<float>();

        std::shared_ptr<SOF::Animation> animation_asset = std::make_shared<SOF::Animation>(frames, frame_time);

        return std::static_pointer_cast<Asset>(animation_asset);
    }


    std::shared_ptr<AssetData> AnimationSerializer::DumpData(AnimationSerializer::Animation &animation_data)
    {
        auto asset_data = std::make_shared<AssetData>();

        nlohmann::json anim_data;
        anim_data["FrameCount"] = animation_data.FrameCount;
        anim_data["Frames"] = animation_data.Frames;
        anim_data["FrameTime"] = animation_data.FrameTime;
        anim_data["SpriteWidth"] = animation_data.SpriteWidth;
        anim_data["SpriteHeight"] = animation_data.SpriteHeight;
        anim_data["SpriteSheetHandle"] = animation_data.SpriteSheetHandle;

        std::vector<uint8_t> bson_data = nlohmann::json::to_bson(anim_data);
        asset_data->RawData = std::vector<char>(bson_data.begin(), bson_data.end());

        asset_data->MetaData["AssetType"] = "Animation";
        asset_data->MetaData["SpriteSheetHandle"] = animation_data.SpriteSheetHandle;
        asset_data->MetaData["SpriteHeight"] = std::to_string(animation_data.SpriteHeight);
        asset_data->MetaData["SpriteWidth"] = std::to_string(animation_data.SpriteWidth);

        return asset_data;
    }
}// namespace SOF