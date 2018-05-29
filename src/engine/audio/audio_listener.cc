#include "engine/audio/audio_listener.h"
#include "engine/audio/audio_system.h"
#include "engine/audio/audio_utils.h"

#include <fmod_studio.hpp>

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------
    AudioListener::AudioListener(AudioSystem* audio_system, int listener_id)
    {
      audio_system_ = audio_system;
      listener_id_ = listener_id;
    }

    //------------------------------------------------------------------------------
    float AudioListener::GetWeight()
    {
      float weight = 0;
      AudioUtils::CheckError(
        audio_system_->GetFModSystem()->getListenerWeight(listener_id_, &weight)
      );
      return weight;
    }

    //------------------------------------------------------------------------------
    void AudioListener::SetWeight(float weight)
    {
      AudioUtils::CheckError(
        audio_system_->GetFModSystem()->setListenerWeight(listener_id_, weight)
      );
    }

    //------------------------------------------------------------------------------
    Audio3DAttributes AudioListener::Get3DAttributes()
    {
      FMOD_3D_ATTRIBUTES attributes;
      AudioUtils::CheckError(
        audio_system_->GetFModSystem()->getListenerAttributes(listener_id_, &attributes)
      );
      return AudioUtils::FModAttributesToEngine(attributes);
    }

    //------------------------------------------------------------------------------
    void AudioListener::Set3DAttributes(const Audio3DAttributes& attributes)
    {
      FMOD_3D_ATTRIBUTES fmod_attributes = AudioUtils::EngineAttributesToFMod(attributes);
      AudioUtils::CheckError(
        audio_system_->GetFModSystem()->setListenerAttributes(listener_id_, &fmod_attributes)
      );
    }
  }
}