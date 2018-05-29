#include "engine/audio/audio_event.h"

#include "engine/audio/audio_event_description.h"
#include "engine/audio/audio_utils.h"

#include <fmod_studio.hpp>

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------
    AudioEvent::AudioEvent(FMOD::Studio::EventInstance* instance, AudioEventDescription* description)
    {
      instance_ = instance;
      description_ = description;
    }

    //------------------------------------------------------------------------------
    void AudioEvent::Release()
    {
      instance_->release();
    }

    //------------------------------------------------------------------------------
    Audio3DAttributes AudioEvent::Get3DAttributes()
    {
      FMOD_3D_ATTRIBUTES attributes;
      AudioUtils::CheckError(instance_->get3DAttributes(&attributes));
      return AudioUtils::FModAttributesToEngine(attributes);
    }

    //------------------------------------------------------------------------------
    AudioEventDescription* AudioEvent::GetDescription()
    {
      return description_;
    }

    //------------------------------------------------------------------------------
    int AudioEvent::GetParameterCount()
    {
      int count = 0;
      AudioUtils::CheckError(instance_->getParameterCount(&count));
      return count;
    }

    //------------------------------------------------------------------------------
    AudioParameter AudioEvent::GetParameter(const char* name)
    {
      FMOD::Studio::ParameterInstance* param = nullptr;
      AudioUtils::CheckError(instance_->getParameter(name, &param));
      return AudioParameter(param);
    }

    //------------------------------------------------------------------------------
    AudioParameter AudioEvent::GetParameterByIndex(int index)
    {
      FMOD::Studio::ParameterInstance* param = nullptr;
      AudioUtils::CheckError(instance_->getParameterByIndex(index, &param));
      return AudioParameter(param);
    }

    //------------------------------------------------------------------------------
    float AudioEvent::GetParameterValue(const char* name)
    {
      float value = 0.0f;
      AudioUtils::CheckError(instance_->getParameterValue(name, &value));
      return value;
    }

    //------------------------------------------------------------------------------
    float AudioEvent::GetParameterValueByIndex(int index)
    {
      float value = 0.0f;
      AudioUtils::CheckError(instance_->getParameterValueByIndex(index, &value));
      return value;
    }

    //------------------------------------------------------------------------------
    bool AudioEvent::GetPaused()
    {
      bool paused = false;
      AudioUtils::CheckError(instance_->getPaused(&paused));
      return paused;
    }

    //------------------------------------------------------------------------------
    float AudioEvent::GetPitch()
    {
      float pitch = 0;
      AudioUtils::CheckError(instance_->getPitch(&pitch));
      return pitch;
    }

    //------------------------------------------------------------------------------
    PlaybackState AudioEvent::GetPlaybackState()
    {
      FMOD_STUDIO_PLAYBACK_STATE state;
      AudioUtils::CheckError(instance_->getPlaybackState(&state));
      return static_cast<PlaybackState>(state);
    }

    //------------------------------------------------------------------------------
    float AudioEvent::GetReverbLevel(int index)
    {
      float reverb = 0;
      AudioUtils::CheckError(instance_->getReverbLevel(index, &reverb));
      return reverb;
    }

    //------------------------------------------------------------------------------
    int AudioEvent::GetTimelinePosition()
    {
      int position = 0;
      AudioUtils::CheckError(instance_->getTimelinePosition(&position));
      return position;
    }

    //------------------------------------------------------------------------------
    float AudioEvent::GetVolume()
    {
      float volume = 0;
      AudioUtils::CheckError(instance_->getVolume(&volume));
      return volume;
    }

    //------------------------------------------------------------------------------
    bool AudioEvent::IsVirtual()
    {
      bool is_virtual = false;
      AudioUtils::CheckError(instance_->isVirtual(&is_virtual));
      return is_virtual;
    }

    //------------------------------------------------------------------------------
    void AudioEvent::Set3DAttributes(const Audio3DAttributes& attributes)
    {
      FMOD_3D_ATTRIBUTES fmod_attributes = AudioUtils::EngineAttributesToFMod(attributes);
      AudioUtils::CheckError(instance_->set3DAttributes(&fmod_attributes));
    }

    //------------------------------------------------------------------------------
    void AudioEvent::SetParameterValue(const char* name, float value)
    {
      AudioUtils::CheckError(instance_->setParameterValue(name, value));
    }

    //------------------------------------------------------------------------------
    void AudioEvent::SetParameterValueByIndex(int index, float value)
    {
      AudioUtils::CheckError(instance_->setParameterValueByIndex(index, value));
    }

    //------------------------------------------------------------------------------
    void AudioEvent::SetParameterValuesByIndices(
      foundation::Vector<int> indices, 
      foundation::Vector<float> values
    )
    {
      AudioUtils::CheckError(
        instance_->setParameterValuesByIndices(
          &indices[0], 
          &values[0], 
          static_cast<int>(indices.size())
        )
      );
    }

    //------------------------------------------------------------------------------
    void AudioEvent::SetPaused(bool paused)
    {
      AudioUtils::CheckError(instance_->setPaused(paused));
    }

    //------------------------------------------------------------------------------
    void AudioEvent::SetPitch(float pitch)
    {
      AudioUtils::CheckError(instance_->setPitch(pitch));
    }

    //------------------------------------------------------------------------------
    void AudioEvent::SetReverbLevel(int index, float reverb)
    {
      AudioUtils::CheckError(instance_->setReverbLevel(index, reverb));
    }

    //------------------------------------------------------------------------------
    void AudioEvent::SetTimelinePosition(int position)
    {
      AudioUtils::CheckError(instance_->setTimelinePosition(position));
    }

    //------------------------------------------------------------------------------
    void AudioEvent::SetVolume(float volume)
    {
      AudioUtils::CheckError(instance_->setVolume(volume));
    }

    //------------------------------------------------------------------------------
    void AudioEvent::Start()
    {
      AudioUtils::CheckError(instance_->start());
    }

    //------------------------------------------------------------------------------
    void AudioEvent::Stop(bool fadeout)
    {
      FMOD_STUDIO_STOP_MODE mode = FMOD_STUDIO_STOP_IMMEDIATE;
      if (fadeout == true)
        mode = FMOD_STUDIO_STOP_ALLOWFADEOUT;

      AudioUtils::CheckError(instance_->stop(mode));
    }

    //------------------------------------------------------------------------------
    void AudioEvent::TriggerCue()
    {
      AudioUtils::CheckError(instance_->triggerCue());
    }
  }
}