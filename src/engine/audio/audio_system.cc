#include "engine/audio/audio_system.h"
#include "engine/audio/audio_utils.h"

#include "engine/assets/asset_system.h"
#include "engine/assets/audio_manager.h"

#include <foundation/job/data_policy.h>
#include <foundation/job/job_graph.h>
#include <foundation/utils/frame.h>

#include <fmod_studio.hpp>
#include <fmod_errors.h>

#include <foundation/logging/logger.h>
#include <foundation/memory/memory.h>
#include <lua-classes/audio_system.lua.cc>

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------
    void* AudioAlloc(unsigned int size, FMOD_MEMORY_TYPE, const char*)
    {
      return foundation::Memory::Allocate(size);
    }

    //------------------------------------------------------------------------------
    void* AudioReAlloc(void* ptr, unsigned int size, FMOD_MEMORY_TYPE, const char*)
    {
      return foundation::Memory::Reallocate(ptr, size);
    }

    //------------------------------------------------------------------------------
    void AudioFree(void *ptr, FMOD_MEMORY_TYPE, const char*)
    {
      foundation::Memory::Deallocate(ptr);
    }

    //------------------------------------------------------------------------------
    AudioSystem::AudioSystem() :
      IServiceSystem<AudioSystem>("AudioSystem"),
      bank_names_()
    {

    }

    //------------------------------------------------------------------------------
    void AudioSystem::OnInitialize(Application&, foundation::JobGraph& job_graph)
    {
      size_t data_size = 1024ul * 1024ul * 100ul;
      void* data = foundation::Memory::Allocate(data_size);
      
      AudioUtils::CheckError(
        FMOD::Memory_Initialize(data, static_cast<int>(data_size), 0, 0, 0, FMOD_MEMORY_ALL)
        );
      AudioUtils::CheckError(FMOD::Studio::System::create(&system_));
      AudioUtils::CheckError(
        system_->initialize(
          512,
          FMOD_STUDIO_INIT_NORMAL,
          FMOD_INIT_NORMAL,
          nullptr
        )
      );

      auto update = [](AudioSystem& system)
      {
        system.Update(foundation::Frame::delta_time());
      };

      job_graph.Add(make_job("audiosystem_update", "update", update, bind_write(*this)));

      ScriptableAudio::Initialize(this);
    }

    //------------------------------------------------------------------------------
    void AudioSystem::OnTerminate()
    {
      AudioUtils::CheckError(system_->release());
      system_ = nullptr;
    }

    //------------------------------------------------------------------------------
    void AudioSystem::Update(float)
    {
      AudioUtils::CheckError(system_->update());
    }

    //------------------------------------------------------------------------------
    AudioBank* AudioSystem::LoadBank(char* data, int length, bool is_master)
    {
      FMOD::Studio::Bank* fmod_bank;
      AudioUtils::CheckError(
        system_->loadBankMemory(
          data,
          length,
          FMOD_STUDIO_LOAD_MEMORY,
          FMOD_STUDIO_LOAD_BANK_NORMAL,
          &fmod_bank
        )
      );

      AudioBank bank = AudioBank(fmod_bank, is_master);

      banks_.push_back(bank);
      bank_names_.push_back(bank.GetPath());

      return &banks_.back();
    }

    //------------------------------------------------------------------------------
    AudioBank* AudioSystem::LoadBank(const char* path, bool is_master)
    {
      FMOD::Studio::Bank* fmod_bank;
      AudioUtils::CheckError(
        system_->loadBankFile(
          path,
          FMOD_STUDIO_LOAD_BANK_NORMAL,
          &fmod_bank)
      );
      AudioBank bank = AudioBank(fmod_bank, is_master);
      banks_.push_back(bank);
      bank_names_.push_back(bank.GetPath());

      return &banks_.back();
    }

    //------------------------------------------------------------------------------
    void AudioSystem::UnloadBank(AudioBank* bank)
    {
      for (int i = 0; i < banks_.size(); i++)
      {
        if (&banks_[i] == bank)
        {
          bank->Unload();
          banks_.erase(banks_.begin() + i);
          bank_names_.erase(bank_names_.begin() + i);
          return;
        }
      }

      PS_LOG(
        Error,
        "Trying to unload a bank that has already been unloaded or that was instantiated in a different system"
      );
    }

    //------------------------------------------------------------------------------
    foundation::Vector<foundation::String> AudioSystem::GetBankNames()
    {
      return bank_names_;
    }

    //------------------------------------------------------------------------------
    AudioBank* AudioSystem::GetBank(const char* name)
    {
      for (int i = 0; i < bank_names_.size(); i++)
      {
        if (strcmp(bank_names_[i].c_str(), name) == 0)
        {
          return &banks_[i];
        }
      }

      PS_LOG(Error, "Trying to load a bank does not exist");
      return &banks_[0];
    }

    //------------------------------------------------------------------------------
    AudioListener* AudioSystem::AddListener()
    {
      int new_num_listeners = static_cast<int>(listeners.size());

      AudioUtils::CheckError(system_->setNumListeners(new_num_listeners + 1));
      AudioListener listener = AudioListener(this, new_num_listeners);
      listeners.push_back(listener);

      return &listeners[new_num_listeners];
    }

    //------------------------------------------------------------------------------
    void AudioSystem::RemoveListener(AudioListener* listener)
    {
      int new_num_listeners = static_cast<int>(listeners.size()) - 1;
      AudioUtils::CheckError(system_->setNumListeners(new_num_listeners + 1));

      int removed_id = listener->listener_id_;
      listeners.erase(listeners.begin() + removed_id);

      for (int i = 0; i < listeners.size(); i++)
      {
        listeners[i].listener_id_ = i;
      }
    }

    //------------------------------------------------------------------------------
    FMOD::Studio::System* AudioSystem::GetFModSystem()
    {
      return system_;
    }

    //------------------------------------------------------------------------------
    void AudioSystem::StartCommandCapture(const char* path)
    {
      AudioUtils::CheckError(
        system_->startCommandCapture(
          path,
          FMOD_STUDIO_COMMANDCAPTURE_NORMAL)
      );
    }

    //------------------------------------------------------------------------------
    void AudioSystem::StopCommandCapture()
    {
      AudioUtils::CheckError(system_->stopCommandCapture());
    }

    //------------------------------------------------------------------------------ 
    AudioCommandReplay AudioSystem::LoadCommandReplay(const char* path)
    {
      FMOD::Studio::CommandReplay* command_replay;
      AudioUtils::CheckError(
        system_->loadCommandReplay(
          path,
          FMOD_STUDIO_COMMANDREPLAY_NORMAL,
          &command_replay
        )
      );
      return AudioCommandReplay(command_replay);
    }

    //------------------------------------------------------------------------------
    AudioSystem* ScriptableAudio::system_ = nullptr;
    void ScriptableAudio::Initialize(AudioSystem* system)
    {
      system_ = system;
    }

    //------------------------------------------------------------------------------
    void ScriptableAudio::LoadBank(const foundation::String& bank)
    {
      AssetHandle<AudioBankData> bank_data = AssetSystem::Instance().Load<AudioBankData>(bank);
      system_->LoadBank(bank_data->data.data(), static_cast<int>(bank_data->data.size()));
    }

    void ScriptableAudio::LoadMasterBank(const foundation::String& bank)
    {
      AssetHandle<AudioBankData> bank_data = AssetSystem::Instance().Load<AudioBankData>(bank);
      system_->LoadBank(bank_data->data.data(), static_cast<int>(bank_data->data.size()), true);
    }
  }
}