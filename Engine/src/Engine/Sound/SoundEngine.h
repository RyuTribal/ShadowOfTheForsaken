#pragma once
#include "Engine/Core/ThreadPool.h"
#include <miniaudio/miniaudio.h>
#include "Audio.h"

namespace SOF
{
    class Scene;
    class SoundComponent;
    class SoundEngine
    {
        public:
        enum SoundAttenuation { LINEAR, EXPONENTIAL };
        struct AttenuationSettings
        {
            float RolloffFactor = 1.0f, ReferenceDistance = 1.0f, MaxDistance = 100.f;
        };
        static void Init();
        static void Shutdown();

        static UUID PlayAudio(SoundComponent *sound_comp, const glm::vec3 &position = { 0.f, 0.f, 0.f });
        static void StopAudio(UUID instance_id);
        static void StopAllAudio();

        static void SetVolume(float volume);
        static float GetVolume();

        static AttenuationSettings &GetAttenuationSettings() { return Instance()->m_AttenuationSettings; }
        static void SetAttenuationSettings(const AttenuationSettings &settings);

        static void Update(const glm::vec3 &focal_point, Scene *active_scene);

        static SoundAttenuation GetAttenuation();
        static void SetAttenuation(SoundAttenuation model);

        private:
        struct SoundInstance
        {
            ma_decoder *Decoder = nullptr;
            std::shared_ptr<Audio> AudioAsset = nullptr;
            bool Loop = false;
            ma_uint64 FramesRead = 0;
            bool ShouldBeDeleted = false;
            bool Is3D = false;
            glm::vec3 Position = { 0.f, 0.f, 0.f };


            SoundInstance() {}
            SoundInstance(ma_decoder *decoder,
              std::shared_ptr<Audio> audio_asset,
              bool loop,
              bool is_3d,
              const glm::vec3 &position)
              : Decoder(decoder), AudioAsset(audio_asset), Loop(loop), Is3D(is_3d), Position(position){};
        };
        SoundEngine();
        ~SoundEngine();
        static SoundEngine *Instance();
        static void DataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount);
        static float CalculatePan(const glm::vec3 &sourcePosition, const glm::vec3 &listenerPosition);
        static float CalculateAttenuation(float distance,
          SoundAttenuation model,
          float rolloffFactor,
          float referenceDistance,
          float maxDistance);

        private:
        ThreadPool m_ThreadPool;
        ma_device m_Device;
        std::mutex m_SoundMutex;
        // So that the sounds are kept in memory in case it gets deleted somewhere else
        std::unordered_map<UUID, SoundInstance> m_ActiveSounds;
        float m_GlobalVolume = 1.0f;
        glm::vec3 m_FocalPoint = { 0.f, 0.f, 0.f };
        SoundAttenuation m_Attenuation = SoundAttenuation::EXPONENTIAL;
        AttenuationSettings m_AttenuationSettings;
    };
}// namespace SOF