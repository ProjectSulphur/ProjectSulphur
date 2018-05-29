#include "engine/audio/audio_vca.h"
#include "engine/audio/audio_utils.h"

#include <fmod_studio.hpp>

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------
    AudioVCA::AudioVCA(FMOD::Studio::VCA* fmod_vca)
    {
      fmod_vca_ = fmod_vca;
    }

    //-----------------------------------------------------------------------------
    foundation::String AudioVCA::GetPath()
    {
      char path[512] = "";
      AudioUtils::CheckError(fmod_vca_->getPath(path, 512, nullptr));
      return foundation::String(path);
    }

    //-----------------------------------------------------------------------------
    float AudioVCA::GetVolume()
    {
      float volume = 0;
      AudioUtils::CheckError(fmod_vca_->getVolume(&volume));
      return volume;
    }

    //-----------------------------------------------------------------------------
    void AudioVCA::SetVolume(float volume)
    {
      AudioUtils::CheckError(fmod_vca_->setVolume(volume));
    }
  }
}
