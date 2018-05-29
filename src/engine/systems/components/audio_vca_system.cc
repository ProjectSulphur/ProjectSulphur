#include "engine/systems/components/audio_vca_system.h"

#include "engine/systems/components/transform_system.h"
#include "engine/systems/components/rigid_body_system.h"
#include "engine/application/application.h"

#include <foundation/job/data_policy.h>
#include <foundation/job/job.h>
#include <foundation/job/job_graph.h>

#include <lua-classes/audio_vca_system.lua.cc>

namespace sulphur
{
  namespace engine
  {
    //---------------------------------------------------------------------------------------------
    AudioVCAComponent::AudioVCAComponent() :
      system_(nullptr)
    {
    }

    //---------------------------------------------------------------------------------------------
    AudioVCAComponent::AudioVCAComponent(System& system, size_t handle)
      :
      ComponentHandleBase(handle),
      system_(&system)
    {

    }

    //---------------------------------------------------------------------------------------------
    AudioVCASystem::AudioVCASystem()
      :
      IComponentSystem("AudioVCASystem")
    {
    }

    //---------------------------------------------------------------------------------------------
    AudioVCAComponent AudioVCASystem::Create(Entity& entity)
    {
      if (!entity.Has<TransformComponent>())
      {
        entity.Add<TransformComponent>();
      }

      AudioVCAComponent audio_event
        = AudioVCAComponent(*this, component_data_.data.Add("", "", nullptr, nullptr, false, entity));

      return audio_event;
    }

    //---------------------------------------------------------------------------------------------
    void AudioVCASystem::OnInitialize(Application& app, foundation::JobGraph&)
    {
      audio_system_ = &app.GetService<AudioSystem>();
    }

    void AudioVCASystem::OnTerminate()
    {
      component_data_.data.Clear();
    }

    //---------------------------------------------------------------------------------------------
    void AudioVCASystem::Destroy(ComponentHandleBase handle)
    {
      component_data_.data.Remove(handle);
    }

    //---------------------------------------------------------------------------------------------
    void AudioVCAComponent::SetBank(foundation::String bank)
    {
      system_->SetBank(*this, bank);
    }
    void AudioVCASystem::SetBank(AudioVCAComponent handle, foundation::String bank)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      component_data_.audio_bank[data_index] = audio_system_->GetBank(bank.c_str());
      component_data_.bank_path[data_index] = bank;
    }

    //---------------------------------------------------------------------------------------------
    void AudioVCAComponent::SetPath(foundation::String path)
    {
      system_->SetPath(*this, path);
    }
    void AudioVCASystem::SetPath(AudioVCAComponent handle, foundation::String path)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (component_data_.audio_bank[data_index] == nullptr)
      {
        PS_LOG(Error, "Please set the audiobank before setting the path");
        return;
      }
      AudioVCA* audio_vca
        = component_data_.audio_bank[data_index]->GetVCAByName(path);

      component_data_.audio_vca[data_index] = audio_vca;
      component_data_.vca_path[data_index] = path;
      component_data_.loaded[data_index] = true;
    }

    //---------------------------------------------------------------------------------------------
    foundation::String AudioVCAComponent::GetBank()
    {
      return system_->GetBank(*this);
    }
    foundation::String AudioVCASystem::GetBank(AudioVCAComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      return component_data_.bank_path[data_index].c_str();
    }

    //---------------------------------------------------------------------------------------------
    foundation::String AudioVCAComponent::GetPath()
    {
      return system_->GetPath(*this);
    }
    foundation::String AudioVCASystem::GetPath(AudioVCAComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      return component_data_.vca_path[data_index].c_str();
    }

    //---------------------------------------------------------------------------------------------
    float AudioVCAComponent::GetVolume()
    {
      return system_->GetVolume(*this);
    }
    float AudioVCASystem::GetVolume(AudioVCAComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio VCA is not loaded please set the bank and path first");
        return 0;
      }
      return component_data_.audio_vca[data_index]->GetVolume();
    }


    //---------------------------------------------------------------------------------------------
    void AudioVCAComponent::SetVolume(float volume)
    {
      system_->SetVolume(*this, volume);
    }
    void AudioVCASystem::SetVolume(AudioVCAComponent handle, float volume)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return;
      }
      component_data_.audio_vca[data_index]->SetVolume(volume);
    }
  }
}