#include "engine/systems/components/audio_snapshot_system.h"

#include "engine/systems/components/transform_system.h"
#include "engine/systems/components/rigid_body_system.h" 
#include "engine/application/application.h"

#include <foundation/job/data_policy.h>
#include <foundation/job/job.h>
#include <foundation/job/job_graph.h>

#include <lua-classes/audio_snapshot_system.lua.cc>

namespace sulphur
{
  namespace engine
  {
    //---------------------------------------------------------------------------------------------
    AudioSnapshotComponent::AudioSnapshotComponent() :
      system_(nullptr)
    {
    }

    //---------------------------------------------------------------------------------------------
    AudioSnapshotComponent::AudioSnapshotComponent(System& system, size_t handle)
      :
      ComponentHandleBase(handle),
      system_(&system)
    {

    }

    //---------------------------------------------------------------------------------------------
    AudioSnapshotSystem::AudioSnapshotSystem()
      :
      IComponentSystem("AudioSnapshotSystem")
    {
    }

    //---------------------------------------------------------------------------------------------
    AudioSnapshotComponent AudioSnapshotSystem::Create(Entity& entity)
    {
      if (!entity.Has<TransformComponent>())
      {
        entity.Add<TransformComponent>();
      }

      AudioSnapshotComponent audio_event
        = AudioSnapshotComponent(*this, component_data_.data.Add("", "", nullptr, nullptr, false, entity));

      return audio_event;
    }

    //---------------------------------------------------------------------------------------------
    void AudioSnapshotSystem::OnInitialize(Application& app, foundation::JobGraph& job_graph)
    {
      audio_system_ = &app.GetService<AudioSystem>();

      auto update_positions_job = [](AudioSnapshotSystem& es)
      {
        es.UpdatePositions();
      };

      foundation::Job update_positions = make_job(
        "update_audio_snapshot_positions",
        "update",
        update_positions_job,
        bind_write(*this)
      );

      job_graph.Add(std::move(update_positions));
    }

    void AudioSnapshotSystem::OnTerminate()
    {
      for (int i = 0; i < component_data_.data.size(); i++)
      {
        component_data_.audio_event[i]->Release();
      }
      component_data_.data.Clear();
    }

    //---------------------------------------------------------------------------------------------
    void AudioSnapshotSystem::Destroy(ComponentHandleBase handle)
    {
      Release(handle);
      component_data_.data.Remove(handle);
    }

    //---------------------------------------------------------------------------------------------
    void AudioSnapshotSystem::UpdatePositions()
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
    void AudioSnapshotComponent::SetBank(foundation::String bank)
    {
      system_->SetBank(*this, bank);
    }
    void AudioSnapshotSystem::SetBank(AudioSnapshotComponent handle, foundation::String bank)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      component_data_.audio_bank[data_index] = audio_system_->GetBank(bank.c_str());
      component_data_.bank_path[data_index] = bank;
    }

    //---------------------------------------------------------------------------------------------
    void AudioSnapshotComponent::SetPath(foundation::String path)
    {
      system_->SetPath(*this, path);
    }
    void AudioSnapshotSystem::SetPath(AudioSnapshotComponent handle, foundation::String path)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (component_data_.audio_bank[data_index] == nullptr)
      {
        PS_LOG(Error, "Please set the audiobank before setting the path");
        return;
      }
      AudioEventDescription* audio_event_description
        = component_data_.audio_bank[data_index]->GetEventDescriptionByName(path);

      if (audio_event_description->IsSnapshot() == false)
      {
        PS_LOG(Error, "Event %s is a normal event, please load it via an event component", path.c_str());
        return;
      }

      component_data_.audio_event[data_index] = audio_event_description->CreateInstance();
      component_data_.event_path[data_index] = path;
      component_data_.loaded[data_index] = true;
    }

    //---------------------------------------------------------------------------------------------
    foundation::String AudioSnapshotComponent::GetBank()
    {
      return system_->GetBank(*this);
    }
    foundation::String AudioSnapshotSystem::GetBank(AudioSnapshotComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      return component_data_.bank_path[data_index].c_str();
    }

    //---------------------------------------------------------------------------------------------
    foundation::String AudioSnapshotComponent::GetPath()
    {
      return system_->GetPath(*this);
    }
    foundation::String AudioSnapshotSystem::GetPath(AudioSnapshotComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      return component_data_.event_path[data_index].c_str();
    }

    //---------------------------------------------------------------------------------------------
    void AudioSnapshotComponent::Release()
    {
      system_->Release(*this);
    }
    void AudioSnapshotSystem::Release(ComponentHandleBase handle)
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
    AudioEventDescription* AudioSnapshotComponent::GetDescription()
    {
      return system_->GetDescription(*this);
    }
    AudioEventDescription* AudioSnapshotSystem::GetDescription(AudioSnapshotComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio snapshot is not loaded please set the bank and path first");
        return nullptr;
      }
      return component_data_.audio_event[data_index]->GetDescription();
    }

    //---------------------------------------------------------------------------------------------
    int AudioSnapshotComponent::GetParameterCount()
    {
      return system_->GetParameterCount(*this);
    }
    int AudioSnapshotSystem::GetParameterCount(AudioSnapshotComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio snapshot is not loaded please set the bank and path first");
        return 0;
      }
      return component_data_.audio_event[data_index]->GetParameterCount();
    }

    //---------------------------------------------------------------------------------------------
    AudioParameter AudioSnapshotComponent::GetParameter(foundation::String name)
    {
      return system_->GetParameter(*this, name);
    }
    AudioParameter AudioSnapshotSystem::GetParameter(AudioSnapshotComponent handle, foundation::String name)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio snapshot is not loaded please set the bank and path first");
      }
      return component_data_.audio_event[data_index]->GetParameter(name.c_str());
    }

    //---------------------------------------------------------------------------------------------
    AudioParameter AudioSnapshotComponent::GetParameterByIndex(int index)
    {
      return system_->GetParameterByIndex(*this, index);
    }
    AudioParameter AudioSnapshotSystem::GetParameterByIndex(AudioSnapshotComponent handle, int index)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio snapshot is not loaded please set the bank and path first");
      }
      return component_data_.audio_event[data_index]->GetParameterByIndex(index);
    }

    //---------------------------------------------------------------------------------------------
    float AudioSnapshotComponent::GetParameterValue(foundation::String name)
    {
      return system_->GetParameterValue(*this, name);
    }
    float AudioSnapshotSystem::GetParameterValue(AudioSnapshotComponent handle, foundation::String name)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio snapshot is not loaded please set the bank and path first");
        return 0;
      }
      return component_data_.audio_event[data_index]->GetParameterValue(name.c_str());
    }

    //---------------------------------------------------------------------------------------------
    float AudioSnapshotComponent::GetParameterValueByIndex(int index)
    {
      return system_->GetParameterValueByIndex(*this, index);
    }
    float AudioSnapshotSystem::GetParameterValueByIndex(AudioSnapshotComponent handle, int index)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio snapshot is not loaded please set the bank and path first");
        return 0;
      }
      return component_data_.audio_event[data_index]->GetParameterValueByIndex(index);
    }

    //---------------------------------------------------------------------------------------------
    PlaybackState AudioSnapshotComponent::GetPlaybackState()
    {
      return system_->GetPlaybackState(*this);
    }
    PlaybackState AudioSnapshotSystem::GetPlaybackState(AudioSnapshotComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio snapshot is not loaded please set the bank and path first");
        return PlaybackState::kStopped;
      }
      return component_data_.audio_event[data_index]->GetPlaybackState();
    }

    //---------------------------------------------------------------------------------------------
    void AudioSnapshotComponent::SetParameterValue(foundation::String name, float value)
    {
      system_->SetParameterValue(*this, name, value);
    }
    void AudioSnapshotSystem::SetParameterValue(AudioSnapshotComponent handle, foundation::String name, float value)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio snapshot is not loaded please set the bank and path first");
        return;
      }
      component_data_.audio_event[data_index]->SetParameterValue(name.c_str(), value);
    }

    //---------------------------------------------------------------------------------------------
    void AudioSnapshotComponent::SetParameterValueByIndex(int index, float value)
    {
      system_->SetParameterValueByIndex(*this, index, value);
    }
    void AudioSnapshotSystem::SetParameterValueByIndex(AudioSnapshotComponent handle, int index, float value)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio snapshot is not loaded please set the bank and path first");
        return;
      }
      component_data_.audio_event[data_index]->SetParameterValueByIndex(index, value);
    }

    //---------------------------------------------------------------------------------------------
    void AudioSnapshotComponent::SetParameterValuesByIndices(
      foundation::Vector<int> indices,
      foundation::Vector<float> values
    )
    {
      system_->SetParameterValuesByIndices(*this, indices, values);
    }
    void AudioSnapshotSystem::SetParameterValuesByIndices(
      AudioSnapshotComponent handle,
      foundation::Vector<int> indices,
      foundation::Vector<float> values
    )
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio snapshot is not loaded please set the bank and path first");
        return;
      }
      component_data_.audio_event[data_index]->SetParameterValuesByIndices(indices, values);
    }

    //---------------------------------------------------------------------------------------------
    void AudioSnapshotComponent::Start()
    {
      system_->Start(*this);
    }
    void AudioSnapshotSystem::Start(AudioSnapshotComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio snapshot is not loaded please set the bank and path first");
        return;
      }
      component_data_.audio_event[data_index]->Start();
    }

    //---------------------------------------------------------------------------------------------
    void AudioSnapshotComponent::Stop(bool fadeout)
    {
      system_->Stop(*this, fadeout);
    }
    void AudioSnapshotSystem::Stop(AudioSnapshotComponent handle, bool fadeout)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.loaded[data_index])
      {
        PS_LOG(Error, "Audio snapshot is not loaded please set the bank and path first");
        return;
      }
      component_data_.audio_event[data_index]->Stop(fadeout);
    }
  }
}