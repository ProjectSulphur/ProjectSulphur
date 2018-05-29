#include "engine/audio/audio_bank.h"
#include "engine/audio/audio_utils.h"

#include <fmod_studio.hpp>
#include <fmod_errors.h>

#include <foundation/logging/logger.h>
#include <foundation/containers/string.h>
#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------
    AudioBank::AudioBank(FMOD::Studio::Bank* bank, bool is_master)
    {
      bank_ = bank;

      if (is_master == true)
      {
        return;
      }

      int count = 0;
      AudioUtils::CheckError(bank_->getEventCount(&count));

      FMOD::Studio::EventDescription** event_descriptions
        = reinterpret_cast<FMOD::Studio::EventDescription**>(
          foundation::Memory::Allocate(sizeof(FMOD::Studio::EventDescription*) * count)
          );

      AudioUtils::CheckError(bank_->getEventList(event_descriptions, count, nullptr));

      for (int i = 0; i < count; i++)
      {
        AudioEventDescription description = AudioEventDescription(event_descriptions[i]);
        event_descriptions_.push_back(description);
        
        event_names_.push_back(description.GetPath());
      }

      count = 0;
      AudioUtils::CheckError(bank_->getBusCount(&count));

      FMOD::Studio::Bus** busses = reinterpret_cast<FMOD::Studio::Bus**>(
        foundation::Memory::Allocate(sizeof(FMOD::Studio::Bus*) * count)
        );

      AudioUtils::CheckError(bank_->getBusList(busses, count, nullptr));

      for (int i = 0; i < count; i++)
      {
        AudioBus bus = AudioBus(busses[i]);

        busses_.push_back(bus);
        bus_names_.push_back(bus.GetPath());
      }

      count = 0;
      AudioUtils::CheckError(bank_->getVCACount(&count));

      FMOD::Studio::VCA** vcas = reinterpret_cast<FMOD::Studio::VCA**>(
        foundation::Memory::Allocate(sizeof(FMOD::Studio::VCA*) * count)
        );

      AudioUtils::CheckError(bank_->getVCAList(vcas, count, nullptr));

      for (int i = 0; i < count; i++)
      {
        AudioVCA VCA = AudioVCA(vcas[i]);

        vcas_.push_back(VCA);
        vca_names_.push_back(VCA.GetPath());
      }

      foundation::Memory::Deallocate(event_descriptions);
      foundation::Memory::Deallocate(busses);
      foundation::Memory::Deallocate(vcas);
    }

    //------------------------------------------------------------------------------
    void AudioBank::Unload()
    {
      bank_->unload();
      bank_ = nullptr;
    }

    //------------------------------------------------------------------------------
    int AudioBank::GetBusCount()
    {
      return static_cast<int>(busses_.size());
    }

    //------------------------------------------------------------------------------
    foundation::Vector<AudioBus> AudioBank::GetBusses()
    {
      return busses_;
    }

    //------------------------------------------------------------------------------
    foundation::Vector<foundation::String> AudioBank::GetBusNames()
    {
      return bus_names_;
    }

    //------------------------------------------------------------------------------
    AudioBus* AudioBank::GetBusByName(const foundation::String& name)
    {
      for (int i = 0; i < bus_names_.size(); i++)
      {
        if (bus_names_[i] == name)
        {
          return &busses_[i];
        }
      }

      PS_LOG(Error, "Tried to get an event description that does not exist");
      return &busses_[0];
    }

    //------------------------------------------------------------------------------
    int AudioBank::GetEventCount()
    {
      return static_cast<int>(event_descriptions_.size());
    }

    //------------------------------------------------------------------------------
    foundation::Vector<AudioEventDescription> AudioBank::GetEventDescriptions()
    {
      return event_descriptions_;
    }

    //------------------------------------------------------------------------------
    foundation::Vector<foundation::String> AudioBank::GetEventNames()
    {
      return event_names_;
    }

    //------------------------------------------------------------------------------
    AudioEventDescription* AudioBank::GetEventDescriptionByName(const foundation::String& name)
    {
      for (int i = 0; i < event_names_.size(); i++)
      {
        if (event_names_[i] == name)
        {
          return &event_descriptions_[i];
        }
      }

      PS_LOG(Error, "Tried to get an event description that does not exist");
      return &event_descriptions_[0];
    }

    //------------------------------------------------------------------------------
    foundation::String AudioBank::GetPath()
    {
      char path[512]; // max fmod path length
      AudioUtils::CheckError(bank_->getPath(path, 512, nullptr));
      return foundation::String(path);
    }

    //------------------------------------------------------------------------------
    int AudioBank::GetVCACount()
    {
      return static_cast<int>(vcas_.size());
    }

    //------------------------------------------------------------------------------
    foundation::Vector<AudioVCA> AudioBank::GetVCAs()
    {
      return vcas_;
    }

    //------------------------------------------------------------------------------
    foundation::Vector<foundation::String> AudioBank::GetVCANames()
    {
      return vca_names_;
    }

    //------------------------------------------------------------------------------
    AudioVCA* AudioBank::GetVCAByName(const foundation::String& name)
    {
      for (int i = 0; i < vca_names_.size(); i++)
      {
        if (vca_names_[i] == name)
        {
          return &vcas_[i];
        }
      }

      PS_LOG(Error, "Tried to get an event description that does not exist");
      return &vcas_[0];
    }
  }
}
