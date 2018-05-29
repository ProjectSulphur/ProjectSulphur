#include "engine/systems/components/audio_bus_system.h"

#include <lua-classes/audio_bus_system.lua.cc>

namespace sulphur
{
  namespace engine
  {
    AudioBusSystem::AudioBusSystem() :
      IComponentSystem("AudioBusSystem")
    {

    }

    //---------------------------------------------------------------------------------------------

    AudioBusComponent AudioBusSystem::Create(Entity& entity)
    {
      AudioBusComponent audio_bus
        = AudioBusComponent(*this, data_.data.Add("", "", nullptr, nullptr, false, entity));

      return audio_bus;
    }

    //---------------------------------------------------------------------------------------------

    void AudioBusSystem::Destroy(ComponentHandleBase handle)
    {
      data_.data.Remove(handle);
    }

    //---------------------------------------------------------------------------------------------

    void AudioBusSystem::OnInitialize(Application& app, foundation::JobGraph&)
    {
      audio_system_ = &app.GetService<AudioSystem>();
    }

    //---------------------------------------------------------------------------------------------

    void AudioBusSystem::OnTerminate()
    {
      data_.data.Clear();
    }

    //---------------------------------------------------------------------------------------------

    AudioBusComponent::AudioBusComponent() :
      system_(nullptr)
    {

    }

    //---------------------------------------------------------------------------------------------

    AudioBusComponent::AudioBusComponent(System& system, size_t handle) :
      ComponentHandleBase(handle),
      system_(&system)
    {

    }


    //---------------------------------------------------------------------------------------------
    void AudioBusComponent::SetBank(foundation::String bank)
    {
      system_->SetBank(*this, bank);
    }
    void AudioBusSystem::SetBank(AudioBusComponent handle, foundation::String bank)
    {
      size_t data_index = data_.data.GetDataIndex(handle);
      data_.audio_bank[data_index] = audio_system_->GetBank(bank.c_str());
      data_.bank_path[data_index] = bank;
    }

    //---------------------------------------------------------------------------------------------
    void AudioBusComponent::SetPath(foundation::String path)
    {
      system_->SetPath(*this, path);
    }
    void AudioBusSystem::SetPath(AudioBusComponent handle, foundation::String path)
    {
      size_t data_index = data_.data.GetDataIndex(handle);
      if (data_.audio_bank[data_index] == nullptr)
      {
        PS_LOG(Error, "Please set the audiobank before setting the path");
        return;
      }
      AudioBus* audio_bus
        = data_.audio_bank[data_index]->GetBusByName(path);

      data_.audio_bus[data_index] = audio_bus;
      data_.bus_path[data_index] = path;
      data_.loaded[data_index] = true;
    }

    //---------------------------------------------------------------------------------------------
    foundation::String AudioBusComponent::GetBank()
    {
      return system_->GetBank(*this);
    }
    foundation::String AudioBusSystem::GetBank(AudioBusComponent handle)
    {
      size_t data_index = data_.data.GetDataIndex(handle);
      return data_.bank_path[data_index].c_str();
    }

    //---------------------------------------------------------------------------------------------
    foundation::String AudioBusComponent::GetPath()
    {
      return system_->GetPath(*this);
    }
    foundation::String AudioBusSystem::GetPath(AudioBusComponent handle)
    {
      size_t data_index = data_.data.GetDataIndex(handle);
      return data_.bus_path[data_index];
    }

    //---------------------------------------------------------------------------------------------

    bool AudioBusComponent::GetMute()
    {
      return system_->GetMute(*this);
    }
    bool AudioBusSystem::GetMute(AudioBusComponent component)
    {
      size_t data_index = data_.data.GetDataIndex(component);
      return data_.audio_bus[data_index]->GetMute();
    }

    //---------------------------------------------------------------------------------------------

    bool AudioBusComponent::GetPaused()
    {
      return system_->GetPaused(*this);
    }
    bool AudioBusSystem::GetPaused(AudioBusComponent component)
    {
      size_t data_index = data_.data.GetDataIndex(component);
      return data_.audio_bus[data_index]->GetPaused();
    }

    //---------------------------------------------------------------------------------------------

    float AudioBusComponent::GetVolume()
    {
      return system_->GetVolume(*this);
    }
    float AudioBusSystem::GetVolume(AudioBusComponent component)
    {
      size_t data_index = data_.data.GetDataIndex(component);
      return data_.audio_bus[data_index]->GetVolume();
    }

    //---------------------------------------------------------------------------------------------

    void AudioBusComponent::SetMute(bool mute)
    {
      system_->SetMute(*this, mute);
    }
    void AudioBusSystem::SetMute(AudioBusComponent component, bool mute)
    {
      size_t data_index = data_.data.GetDataIndex(component);
      data_.audio_bus[data_index]->SetMute(mute);
    }

    //---------------------------------------------------------------------------------------------

    void AudioBusComponent::SetPaused(bool paused)
    {
      system_->SetPaused(*this, paused);
    }
    void AudioBusSystem::SetPaused(AudioBusComponent component, bool paused) 
    {
      size_t data_index = data_.data.GetDataIndex(component);
      data_.audio_bus[data_index]->SetPaused(paused);
    }

    //---------------------------------------------------------------------------------------------

    void AudioBusComponent::SetVolume(float volume)
    {
      system_->SetVolume(*this, volume);
    }
    void AudioBusSystem::SetVolume(AudioBusComponent component, float volume) 
    {
      size_t data_index = data_.data.GetDataIndex(component);
      data_.audio_bus[data_index]->SetVolume(volume);
    }

    //---------------------------------------------------------------------------------------------

    void AudioBusComponent::StopAllEvents(bool fade)
    {
      system_->StopAllEvents(*this, fade);
    }
    void AudioBusSystem::StopAllEvents(AudioBusComponent component, bool fade)
    {
      size_t data_index = data_.data.GetDataIndex(component);
      data_.audio_bus[data_index]->StopAllEvents(fade);
    }

  }
}