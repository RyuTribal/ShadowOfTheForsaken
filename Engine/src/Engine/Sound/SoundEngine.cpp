#include "pch.h"
#include "SoundEngine.h"
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio/miniaudio.h>
#include "Engine/Asset/Manager.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"

namespace SOF
{
    SoundEngine *SoundEngine::s_Instance = nullptr;

    void SoundEngine::Init()
    {
        SOF_ASSERT(!s_Instance, "Can only have one sound engine instance!");
        s_Instance = new SoundEngine();
        SOF_INFO("Sound Engine", "Initialized");
    }
    void SoundEngine::Shutdown()
    {
        delete s_Instance;
        s_Instance = nullptr;
    }

    UUID SoundEngine::PlayAudio(SoundComponent *sound_comp, const glm::vec3 &position)
    {
        auto instance = Instance();
        UUID audio_instance_id;
        instance->m_ThreadPool.AddTask(
          [instance, audio_instance_id, sound_comp](const glm::vec3 &position) {
              std::shared_ptr<Audio> audio_asset = AssetManager::Load<Audio>(sound_comp->AssetHandle);
              if (audio_asset == nullptr) {
                  SOF_ERROR("SoundEngine", "This audio asset does not exist");
                  return;
              }
              const auto &data = audio_asset->GetData();

              ma_decoder_config decoderConfig = ma_decoder_config_init(instance->m_Device.playback.format,
                instance->m_Device.playback.channels,
                instance->m_Device.sampleRate);

              ma_decoder *decoder = new ma_decoder;
              ma_result result = ma_decoder_init_memory(data->data(), data->size(), &decoderConfig, decoder);
              if (result != MA_SUCCESS) {
                  delete decoder;
                  return;
              }

              ma_data_source_set_looping(decoder, sound_comp->Loop);

              {
                  std::lock_guard<std::mutex> lock(instance->m_SoundMutex);
                  instance->m_ActiveSounds[audio_instance_id] = { decoder,
                      audio_asset,
                      sound_comp->Loop,
                      sound_comp->Type == SoundType::SPATIAL,
                      sound_comp->Volume,
                      position };
              }
          },
          position);
        return audio_instance_id;
    }

    void SoundEngine::StopAudio(UUID instance_id)
    {
        SoundEngine *instance = Instance();
        std::lock_guard<std::mutex> lock(instance->m_SoundMutex);
        auto it = instance->m_ActiveSounds.find(instance_id);
        if (it != instance->m_ActiveSounds.end()) { it->second.ShouldBeDeleted = true; }
    }

    void SoundEngine::StopAllAudio()
    {
        SoundEngine *instance = Instance();
        std::lock_guard<std::mutex> lock(instance->m_SoundMutex);
        for (auto &[id, audio_instance] : instance->m_ActiveSounds) { audio_instance.ShouldBeDeleted = true; }
    }

    void SoundEngine::SetVolume(float volume)
    {
        SoundEngine *instance = Instance();
        std::lock_guard<std::mutex> lock(instance->m_SoundMutex);
        instance->m_GlobalVolume = std::max(0.0f, std::min(volume, 1.0f));
    }

    float SoundEngine::GetVolume()
    {
        SoundEngine *instance = Instance();
        return instance->m_GlobalVolume;
    }

    void SoundEngine::SetAttenuationSettings(const AttenuationSettings &settings)
    {
        SoundEngine *instance = Instance();
        std::lock_guard<std::mutex> lock(instance->m_SoundMutex);
        instance->m_AttenuationSettings = settings;
    }

    void SoundEngine::Update(const glm::vec3 &focal_point, Scene *active_scene)
    {
        auto instance = Instance();
        instance->m_FocalPoint = focal_point;
        if (!active_scene) { return; }
        auto sound_registry = active_scene->GetRegistry()->GetComponentRegistry<SoundComponent>();
        std::lock_guard<std::mutex> lock(instance->m_SoundMutex);
        if (sound_registry) {
            for (auto &[id, sound_comp] : *sound_registry) {
                auto it = instance->m_ActiveSounds.find(sound_comp.InstanceID);
                if (it != instance->m_ActiveSounds.end()) {
                    if (sound_comp.Type == SoundType::SPATIAL) {
                        auto entity = active_scene->GetEntity(id);
                        if (entity->HasComponent<TransformComponent>()) {
                            it->second.Position = entity->GetComponent<TransformComponent>()->Translation;
                        }
                    }
                    it->second.Is3D = sound_comp.Type == SoundType::SPATIAL;
                    it->second.Loop = sound_comp.Loop;
                }
            }
        }
    }

    SoundEngine::SoundAttenuation SoundEngine::GetAttenuation()
    {
        SoundEngine *instance = Instance();
        return instance->m_Attenuation;
    }

    void SoundEngine::SetAttenuation(SoundAttenuation model)
    {
        SoundEngine *instance = Instance();
        std::lock_guard<std::mutex> lock(instance->m_SoundMutex);
        instance->m_Attenuation = model;
    }

    SoundEngine::SoundEngine()
    {
        ma_device_config config = ma_device_config_init(ma_device_type_playback);
        config.playback.format = ma_format_unknown;
        config.playback.channels = 0;
        config.sampleRate = 0;
        config.dataCallback = SoundEngine::DataCallback;// miniaudio, as I understand it, runs this in a seperate
                                                        // thread. So we should be all good

        SOF_ASSERT(ma_device_init(NULL, &config, &m_Device) == MA_SUCCESS, "Sound device could not be initialized!");

        ma_device_start(&m_Device);
    }
    SoundEngine::~SoundEngine() { 
        // if(ma_device__is_initialized(&m_Device)){
        //     ma_device_uninit(&m_Device); 
        // }
    }
    SoundEngine *SoundEngine::Instance()
    {
        SOF_ASSERT(s_Instance, "No sound instance created, did you forget to run Init()?");
        return s_Instance;
    }
    void SoundEngine::DataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
    {
        SoundEngine *instance = s_Instance;
        if(!s_Instance){
            return;
        }
        float *pOutputF32 = (float *)pOutput;
        memset(pOutput, 0, frameCount * pDevice->playback.channels * sizeof(float));

        std::lock_guard<std::mutex> lock(instance->m_SoundMutex);
        std::vector<UUID> soundsToRemove;
        for (auto &[id, audio_instance] : instance->m_ActiveSounds) {
            if (audio_instance.ShouldBeDeleted) {
                soundsToRemove.push_back(id);
                continue;
            }
            std::vector<float> tempBuffer(frameCount * pDevice->playback.channels);
            ma_decoder_read_pcm_frames(
              audio_instance.Decoder, tempBuffer.data(), frameCount, &audio_instance.FramesRead);
            if (audio_instance.FramesRead > 0) {
                for (ma_uint64 frameIndex = 0; frameIndex < audio_instance.FramesRead; ++frameIndex) {
                    for (ma_uint32 channelIndex = 0; channelIndex < pDevice->playback.channels; ++channelIndex) {
                        float sample = tempBuffer[frameIndex * pDevice->playback.channels + channelIndex];
                        float attenuation = 1.f;
                        if (audio_instance.Is3D) {
                            glm::vec3 direction = audio_instance.Position - instance->m_FocalPoint;
                            float distance = glm::length(direction);
                            attenuation = CalculateAttenuation(distance,
                              instance->m_Attenuation,
                              instance->m_AttenuationSettings.RolloffFactor,
                              instance->m_AttenuationSettings.ReferenceDistance,
                              instance->m_AttenuationSettings.MaxDistance);
                            float pan = CalculatePan(audio_instance.Position, instance->m_FocalPoint);
                        }
                        float finalVolume = attenuation * audio_instance.Volume * instance->m_GlobalVolume;
                        pOutputF32[frameIndex * pDevice->playback.channels + channelIndex] += sample * finalVolume;
                    }
                }
            } else {
                if (audio_instance.Loop) {
                    ma_decoder_seek_to_pcm_frame(audio_instance.Decoder, 0);
                } else {
                    ma_decoder_uninit(audio_instance.Decoder);
                    delete audio_instance.Decoder;
                    soundsToRemove.push_back(id);
                }
            }
        }
        for (UUID id : soundsToRemove) { instance->m_ActiveSounds.erase(id); }

        (void)pInput;
    }

    float SoundEngine::CalculatePan(const glm::vec3 &sourcePosition, const glm::vec3 &listenerPosition)
    {
        glm::vec3 direction = sourcePosition - listenerPosition;
        float angle = atan2f(direction.x, direction.z);
        float pan = sinf(angle);
        pan = glm::clamp(pan, -1.0f, 1.0f);

        return pan;
    }

    float SoundEngine::CalculateAttenuation(float distance,
      SoundAttenuation model,
      float rolloffFactor,
      float referenceDistance,
      float maxDistance)
    {
        float attenuation = 1.0f;

        switch (model) {
        case SoundAttenuation::LINEAR: {
            // Linear attenuation: volume decreases linearly with distance
            if (distance <= referenceDistance) {
                attenuation = 1.0f;
            } else if (distance >= maxDistance) {
                attenuation = 0.0f;
            } else {
                attenuation = 1.0f - rolloffFactor * (distance - referenceDistance) / (maxDistance - referenceDistance);
                attenuation = glm::clamp(attenuation, 0.0f, 1.0f);
            }
            break;
        }

        case SoundAttenuation::EXPONENTIAL: {
            // Exponential attenuation: volume decreases exponentially with distance
            if (distance <= referenceDistance) {
                attenuation = 1.0f;
            } else if (distance >= maxDistance) {
                attenuation = 0.0f;
            } else {
                attenuation = powf(distance / referenceDistance, -rolloffFactor);
                attenuation = glm::clamp(attenuation, 0.0f, 1.0f);
            }
            break;
        }

        default:
            attenuation = 1.0f;
            break;
        }

        return attenuation;
    }


}// namespace SOF