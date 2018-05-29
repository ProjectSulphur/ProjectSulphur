#include "engine/systems/components/audio_event_system.h"

#include "engine/systems/components/transform_system.h"
#include "engine/systems/components/rigid_body_system.h"
#include "engine/application/application.h"

#include <foundation/job/data_policy.h>
#include <foundation/job/job.h>
#include <foundation/job/job_graph.h>

#include <lua-classes/audio_event_system.lua.cc>

namespace sulphur
{
  namespace engine
  {
    //---------------------------------------------------------------------------------------------
    AudioEventComponent::AudioEventComponent() :
      system_(nullptr)
    {
    }

    //---------------------------------------------------------------------------------------------
    AudioEventComponent::AudioEventComponent(System& system, size_t handle)
      :
      ComponentHandleBase(handle),
      system_(&system)
    {

    }

    //---------------------------------------------------------------------------------------------
    AudioEventSystem::AudioEventSystem() :
      IComponentSystem("AudioEventSystem")
    {
    }

    //---------------------------------------------------------------------------------------------
    AudioEventComponent AudioEventSystem::Create(Entity& entity)
    {
      if (!entity.Has<TransformComponent>())
      {
        entity.Add<TransformComponent>();
      }

      AudioEventComponent audio_event
        = AudioEventComponent(*this, component_data_.data.Add("", "", nullptr, nullptr, false, entity));

      return audio_event;
    }

    //---------------------------------------------------------------------------------------------
    void AudioEventSystem::OnInitialize(Application& app, foundation::JobGraph& job_graph)
    {
      audio_system_ = &app.GetService<AudioSystem>();

      auto update_positions_job = [](AudioEventSystem& es)
      {
        es.UpdatePositions();
      };

      foundation::Job update_positions = make_job(
        "update_audio_event_positions", 
        "update", 
        update_positions_job, 
        bind_write(*this)
      );

      job_graph.Add(std::move(update_positions));
    }

    void AudioEventSystem::OnTerminate()
    {
      for (int i = 0; i < component_data_.data.size(); i++)
      {
        component_data_.audio_event[i]->Release();
      }
      component_data_.data.Clear();
    }

    //---------------------------------------------------------------------------------------------
    void AudioEventSystem::Destroy(ComponentHandleBase handle)
    {
      Release(handle);
      component_data_.data.Remove(handle);
    }

    //---------------------------------------------------------------------------------------------
    void AudioEventSystem::UpdatePositions()
    {
      for (int i = 0; i < component_data_.data.size(); i++)
      {
        if (component_data_.loaded[i])
        {
          TransformComponent transform = component_data_.entity[i].Get<TransformComponent>();
          Audio3DAttributes attributes;
          attributes.position = transform.GetWorldPosition();
          attributes.forward = transform.GetWorldForward();
          attributes.up = transform.GetWorldUp();
          attributes.velocity = glm::vec3(0, 0, 0); // no doppler effect for you

          if (component_data_.entity[i].Has<RigidBodyComponent>())
          {
            RigidBodyComponent rigidbody = component_data_.entity[i].Get<RigidBodyComponent>();
            attributes.velocity = rigidbody.GetLinearVelocity();
          }

          component_data_.audio_event[i]->Set3DAttributes(attributes);
        }
      }
    }


    //---------------------------------------------------------------------------------------------
    void AudioEventComponent::SetBank(foundation::String bank)
    {
      system_->SetBank(*this, bank);
    }
    void AudioEventSystem::SetBank(AudioEventComponent handle, foundation::String bank)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      component_data_.audio_bank[data_index] = audio_system_->GetBank(bank.c_str());
      component_data_.bank_path[data_index] = bank;
    }

    //---------------------------------------------------------------------------------------------
    void AudioEventComponent::SetPath(foundation::String path)
    {
      system_->SetPath(*this, path);
    }
    void AudioEventSystem::SetPath(AudioEventComponent handle, foundation::String path)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (component_data_.audio_bank[data_index] == nullptr)
      {
        PS_LOG(Error, "Please set the audiobank before setting the path");
        return;
      }
      AudioEventDescription* audio_event_description
        = component_data_.audio_bank[data_index]->GetEventDescriptionByName(path);

      if (audio_event_description->IsSnapshot())
      {
        PS_LOG(Error, "Event %s is a snapshot, please load it via a snapshot component", path.c_str());
        return;
      }

      component_data_.audio_event[data_index] = audio_event_description->CreateInstance();
      component_data_.event_path[data_index] = path;
      component_data_.loaded[data_index] = true;
    }

    //---------------------------------------------------------------------------------------------
    foundation::String AudioEventComponent::GetBank()
    {
      return system_->GetBank(*this);
    }
    foundation::String AudioEventSystem::GetBank(AudioEventComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      return component_data_.bank_path[data_index].c_str();
    }

    //---------------------------------------------------------------------------------------------
    foundation::String AudioEventComponent::GetPath()
    {
      return system_->GetPath(*this);
    }
    foundation::String AudioEventSystem::GetPath(AudioEventComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      return component_data_.event_path[data_index].c_str();
    }

    //---------------------------------------------------------------------------------------------
    void AudioEventComponent::Release()
    {
      system_->Release(*this);
    }

    void AudioEventSystem::Release(ComponentHandleBase handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        // no event loaded
        return;
      }
      component_data_.audio_event[data_index]->Release();
      component_data_.audio_event[data_index] = nullptr;
      component_data_.loaded[data_index] = false;
    }

    //---------------------------------------------------------------------------------------------
    AudioEventDescription* AudioEventComponent::GetDescription()
    {
      return system_->GetDescription(*this);
    }
    AudioEventDescription* AudioEventSystem::GetDescription(AudioEventComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return nullptr;
      }
      return component_data_.audio_event[data_index]->GetDescription();
    }

    //---------------------------------------------------------------------------------------------
    int AudioEventComponent::GetParameterCount()
    {
      return system_->GetParameterCount(*this);
    }
    int AudioEventSystem::GetParameterCount(AudioEventComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return 0;
      }
      return component_data_.audio_event[data_index]->GetParameterCount();
    }

    //---------------------------------------------------------------------------------------------
    AudioParameter AudioEventComponent::GetParameter(foundation::String name)
    {
      return system_->GetParameter(*this, name);
    }
    AudioParameter AudioEventSystem::GetParameter(AudioEventComponent handle, foundation::String name)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
      }
      return component_data_.audio_event[data_index]->GetParameter(name.c_str());
    }

    //---------------------------------------------------------------------------------------------
    AudioParameter AudioEventComponent::GetParameterByIndex(int index)
    {
      return system_->GetParameterByIndex(*this, index);
    }
    AudioParameter AudioEventSystem::GetParameterByIndex(AudioEventComponent handle, int index)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
      }
      return component_data_.audio_event[data_index]->GetParameterByIndex(index);
    }

    //---------------------------------------------------------------------------------------------
    float AudioEventComponent::GetParameterValue(foundation::String name)
    {
      return system_->GetParameterValue(*this, name);
    }
    float AudioEventSystem::GetParameterValue(AudioEventComponent handle, foundation::String name)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return 0;
      }
      return component_data_.audio_event[data_index]->GetParameterValue(name.c_str());
    }

    //---------------------------------------------------------------------------------------------
    float AudioEventComponent::GetParameterValueByIndex(int index)
    {
      return system_->GetParameterValueByIndex(*this, index);
    }
    float AudioEventSystem::GetParameterValueByIndex(AudioEventComponent handle, int index)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return 0;
      }
      return component_data_.audio_event[data_index]->GetParameterValueByIndex(index);
    }

    //---------------------------------------------------------------------------------------------
    bool AudioEventComponent::GetPaused()
    {
      return system_->GetPaused(*this);
    }
    bool AudioEventSystem::GetPaused(AudioEventComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return true;
      }
      return component_data_.audio_event[data_index]->GetPaused();
    }

    //---------------------------------------------------------------------------------------------
    float AudioEventComponent::GetPitch()
    {
      return system_->GetPitch(*this);
    }
    float AudioEventSystem::GetPitch(AudioEventComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return 0;
      }
      return component_data_.audio_event[data_index]->GetPitch();
    }

    //---------------------------------------------------------------------------------------------
    PlaybackState AudioEventComponent::GetPlaybackState()
    {
      return system_->GetPlaybackState(*this);
    }
    PlaybackState AudioEventSystem::GetPlaybackState(AudioEventComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return PlaybackState::kStopped;
      }
      return component_data_.audio_event[data_index]->GetPlaybackState();
    }

    //---------------------------------------------------------------------------------------------
    float AudioEventComponent::GetReverbLevel(int index)
    {
      return system_->GetReverbLevel(*this, index);
    }
    float AudioEventSystem::GetReverbLevel(AudioEventComponent handle, int index)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return 0;
      }
      return component_data_.audio_event[data_index]->GetReverbLevel(index);
    }

    //---------------------------------------------------------------------------------------------
    int AudioEventComponent::GetTimelinePosition()
    {
      return system_->GetTimelinePosition(*this);
    }
    int AudioEventSystem::GetTimelinePosition(AudioEventComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return 0;
      }
      return component_data_.audio_event[data_index]->GetTimelinePosition();
    }

    //---------------------------------------------------------------------------------------------
    float AudioEventComponent::GetVolume()
    {
      return system_->GetVolume(*this);
    }
    float AudioEventSystem::GetVolume(AudioEventComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return 0;
      }
      return component_data_.audio_event[data_index]->GetVolume();
    }

    //---------------------------------------------------------------------------------------------
    bool AudioEventComponent::IsVirtual()
    {
      return system_->IsVirtual(*this);
    }
    bool AudioEventSystem::IsVirtual(AudioEventComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return false;
      }
      return component_data_.audio_event[data_index]->IsVirtual();
    }

    //---------------------------------------------------------------------------------------------
    void AudioEventComponent::SetParameterValue(foundation::String name, float value)
    {
      system_->SetParameterValue(*this, name, value);
    }
    void AudioEventSystem::SetParameterValue(AudioEventComponent handle, foundation::String name, float value)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return;
      }
      component_data_.audio_event[data_index]->SetParameterValue(name.c_str(), value);
    }

    //---------------------------------------------------------------------------------------------
    void AudioEventComponent::SetParameterValueByIndex(int index, float value)
    {
      system_->SetParameterValueByIndex(*this, index, value);
    }
    void AudioEventSystem::SetParameterValueByIndex(AudioEventComponent handle, int index, float value)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return;
      }
      component_data_.audio_event[data_index]->SetParameterValueByIndex(index, value);
    }

    //---------------------------------------------------------------------------------------------
    void AudioEventComponent::SetParameterValuesByIndices(
      foundation::Vector<int> indices, 
      foundation::Vector<float> values
    )
    {
      system_->SetParameterValuesByIndices(*this, indices, values);
    }
    void AudioEventSystem::SetParameterValuesByIndices(
      AudioEventComponent handle, 
      foundation::Vector<int> indices, 
      foundation::Vector<float> values
    )
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return;
      }
      component_data_.audio_event[data_index]->SetParameterValuesByIndices(indices, values);
    }

    //---------------------------------------------------------------------------------------------
    void AudioEventComponent::SetPaused(bool paused)
    {
      system_->SetPaused(*this, paused);
    }
    void AudioEventSystem::SetPaused(AudioEventComponent handle, bool paused)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return;
      }
      component_data_.audio_event[data_index]->SetPaused(paused);
    }

    //---------------------------------------------------------------------------------------------
    void AudioEventComponent::SetPitch(float pitch)
    {
      system_->SetPitch(*this, pitch);
    }
    void AudioEventSystem::SetPitch(AudioEventComponent handle, float pitch)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return;
      }
      component_data_.audio_event[data_index]->SetPitch(pitch);
    }

    //---------------------------------------------------------------------------------------------
    void AudioEventComponent::SetReverbLevel(int index, float reverb)
    {
      system_->SetReverbLevel(*this, index, reverb);
    }
    void AudioEventSystem::SetReverbLevel(AudioEventComponent handle, int index, float reverb)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return;
      }
      component_data_.audio_event[data_index]->SetReverbLevel(index, reverb);
    }

    //---------------------------------------------------------------------------------------------
    void AudioEventComponent::SetTimelinePosition(int position)
    {
      system_->SetTimelinePosition(*this, position);
    }
    void AudioEventSystem::SetTimelinePosition(AudioEventComponent handle, int position)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return;
      }
      component_data_.audio_event[data_index]->SetTimelinePosition(position);
    }

    //---------------------------------------------------------------------------------------------
    void AudioEventComponent::SetVolume(float volume)
    {
      system_->SetVolume(*this, volume);
    }
    void AudioEventSystem::SetVolume(AudioEventComponent handle, float volume)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return;
      }
      component_data_.audio_event[data_index]->SetVolume(volume);
    }

    //---------------------------------------------------------------------------------------------
    void AudioEventComponent::Start()
    {
      system_->Start(*this);
    }
    void AudioEventSystem::Start(AudioEventComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return;
      }
      component_data_.audio_event[data_index]->Start();
    }

    //---------------------------------------------------------------------------------------------
    void AudioEventComponent::Stop(bool fadeout)
    {
      system_->Stop(*this, fadeout);
    }
    void AudioEventSystem::Stop(AudioEventComponent handle, bool fadeout)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return;
      }
      component_data_.audio_event[data_index]->Stop(fadeout);
    }

    //---------------------------------------------------------------------------------------------
    void AudioEventComponent::TriggerCue()
    {
      system_->TriggerCue(*this);
    }
    void AudioEventSystem::TriggerCue(AudioEventComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio event is not loaded please set the bank and path first");
        return;
      }
      component_data_.audio_event[data_index]->TriggerCue();
    }

  }
}