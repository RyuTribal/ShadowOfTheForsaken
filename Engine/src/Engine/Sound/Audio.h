#pragma once
#include "Engine/Asset/Asset.h"
#include <miniaudio/miniaudio.h>

namespace SOF
{
    class Audio : public Asset
    {
        public:
        static std::shared_ptr<Audio> Create(const char *data, size_t data_size, double duration)
        {
            return std::make_shared<Audio>(data, data_size, duration);
        }
        Audio(const char *data, size_t data_size, double duration);
        ~Audio();

        static std::string FormatToString(ma_format format)
        {
            switch (format) {
            case ma_format_u8:
                return "Unsigned 8-bit PCM";
            case ma_format_s16:
                return "Signed 16-bit PCM";
            case ma_format_s24:
                return "Signed 24-bit PCM";
            case ma_format_s32:
                return "Signed 32-bit PCM";
            case ma_format_f32:
                return "32-bit Floating Point PCM";
            default:
                return "Unknown Format";
            }
        }

        virtual AssetType GetType() const override { return AssetType::Audio; };

        std::shared_ptr<std::vector<char>> &GetData() { return m_Data; }

        private:
        double m_Duration;
        std::shared_ptr<std::vector<char>> m_Data;
    };
}// namespace SOF