#include "engine/audio/audio_bus.h"
#include "engine/audio/audio_utils.h"

#include <fmod_studio.hpp>

namespace sulphur
{
  namespace engine
  {
    AudioBus::AudioBus(FMOD::Studio::Bus* fmod_bus)
    {
      fmod_bus_ = fmod_bus;
    }

    //-----------------------------------------------------------------------------
    bool AudioBus::GetMute()
    {
      bool mute = false;
      AudioUtils::CheckError(fmod_bus_->getMute(&mute));
      return mute;
    }

    //-----------------------------------------------------------------------------
    foundation::String AudioBus::GetPath()
    {
      char path[512] = "";
      AudioUtils::CheckError(fmod_bus_->getPath(path, 512, nullptr));
      return foundation::String(path);
    }

    //-----------------------------------------------------------------------------
    bool AudioBus::GetPaused()
    {
      bool paused = false;
      AudioUtils::CheckError(fmod_bus_->getPaused(&paused));
      return paused;
    }

    //-----------------------------------------------------------------------------
    float AudioBus::GetVolume()
    {
      float volume = 0.0f;
      AudioUtils::CheckError(fmod_bus_->getVolume(&volume));
      return volume;
    }

    //-----------------------------------------------------------------------------
    void AudioBus::SetMute(bool mute)
    {
      AudioUtils::CheckError(fmod_bus_->setMute(mute));
    }

    //-----------------------------------------------------------------------------
    void AudioBus::SetPaused(bool paused)
    {
      AudioUtils::CheckError(fmod_bus_->setPaused(paused));
    }

    //-----------------------------------------------------------------------------
    void AudioBus::SetVolume(float volume)
    {
      AudioUtils::CheckError(fmod_bus_->setVolume(volume));
    }

    //-----------------------------------------------------------------------------
    void AudioBus::StopAllEvents(bool fade)
    {
      FMOD_STUDIO_STOP_MODE mode = fade ? FMOD_STUDIO_STOP_ALLOWFADEOUT : FMOD_STUDIO_STOP_IMMEDIATE;
      AudioUtils::CheckError(fmod_bus_->stopAllEvents(mode));
    }
  }
}