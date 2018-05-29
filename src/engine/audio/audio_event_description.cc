#include "engine/audio/audio_event_description.h"
#include "engine/audio/audio_event.h"
#include "engine/audio/audio_utils.h"

#include <fmod_studio.hpp>
#include <foundation/containers/string.h>
#include <foundation/memory/memory.h>
#include <foundation/logging/logger.h>

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------
    AudioEventDescription::AudioEventDescription(FMOD::Studio::EventDescription* description)
    {
      description_ = description;
    }

    //------------------------------------------------------------------------------
    foundation::SharedPointer<AudioEvent> AudioEventDescription::CreateInstance()
    {
      FMOD::Studio::EventInstance* instance;
      AudioUtils::CheckError(description_->createInstance(&instance));
      return foundation::Memory::ConstructShared<AudioEvent>(instance, this);
    }

    //------------------------------------------------------------------------------
    int AudioEventDescription::GetInstanceCount()
    {
      int count = 0;
      AudioUtils::CheckError(description_->getInstanceCount(&count));
      return count;
    }

    //------------------------------------------------------------------------------
    int AudioEventDescription::GetLength()
    {
      int length = 0;
      AudioUtils::CheckError(description_->getLength(&length));
      return length;
    }

    //------------------------------------------------------------------------------
    float AudioEventDescription::GetMaximumDistance()
    {
      float distance = 0.0f;
      AudioUtils::CheckError(description_->getMaximumDistance(&distance));
      return distance;
    }

    //------------------------------------------------------------------------------
    float AudioEventDescription::GetMinimumDistance()
    {
      float distance = 0.0f;
      AudioUtils::CheckError(description_->getMinimumDistance(&distance));
      return distance;
    }

    //------------------------------------------------------------------------------
    foundation::String AudioEventDescription::GetPath()
    {
      char path[512];
      AudioUtils::CheckError(description_->getPath(path, 512, nullptr));
      return foundation::String(path);
    }

    //------------------------------------------------------------------------------
    float AudioEventDescription::GetSoundSize()
    {
      float size = 0;
      AudioUtils::CheckError(description_->getSoundSize(&size));
      return size;
    }

    //------------------------------------------------------------------------------
    bool AudioEventDescription::HasCue()
    {
      bool has_cue = false;
      AudioUtils::CheckError(description_->hasCue(&has_cue));
      return has_cue;
    }

    //------------------------------------------------------------------------------
    bool AudioEventDescription::Is3D()
    {
      bool is_3d = false;
      AudioUtils::CheckError(description_->is3D(&is_3d));
      return is_3d;
    }

    //------------------------------------------------------------------------------
    bool AudioEventDescription::IsOneshot()
    {
      bool is_oneshot = false;
      AudioUtils::CheckError(description_->isOneshot(&is_oneshot));
      return is_oneshot;
    }

    //------------------------------------------------------------------------------
    bool AudioEventDescription::IsSnapshot()
    {
      bool is_snapshot = false;
      AudioUtils::CheckError(description_->isSnapshot(&is_snapshot));
      return is_snapshot;
    }

    //------------------------------------------------------------------------------
    bool AudioEventDescription::IsStream()
    {
      bool is_stream = false;
      AudioUtils::CheckError(description_->isSnapshot(&is_stream));
      return is_stream;
    }

    //------------------------------------------------------------------------------
    void AudioEventDescription::ReleaseAllInstances()
    {
      description_->releaseAllInstances(); // this is generally considered a bad idea
    }
  }
}