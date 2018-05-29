#include "engine/systems/components/audio_listener_system.h"

#include "engine/systems/components/transform_system.h"
#include "engine/systems/components/rigid_body_system.h"

#include <foundation/job/data_policy.h>
#include <foundation/job/job.h>
#include <foundation/job/job_graph.h>

#include <lua-classes/audio_listener_system.lua.cc>
namespace sulphur
{
  namespace engine
  {
    //---------------------------------------------------------------------------------------------
    AudioListenerSystem::AudioListenerSystem() :
      IComponentSystem("AudioListenerSystem")
    {
    }

    //---------------------------------------------------------------------------------------------
    AudioListenerComponent AudioListenerSystem::Create(Entity& entity)
    {
      if (!entity.Has<TransformComponent>())
      {
        entity.Add<TransformComponent>();
      }

      AudioListenerComponent audio_listener
        = AudioListenerComponent(*this, component_data_.data.Add(audio_system_->AddListener(), entity));

      return audio_listener;
    }

    //---------------------------------------------------------------------------------------------
    void AudioListenerSystem::OnInitialize(Application& app, foundation::JobGraph& job_graph)
    {
      audio_system_ = &app.GetService<AudioSystem>();

      auto update_positions_job = [](AudioListenerSystem& ls)
      {
        ls.UpdatePositions();
      };

      foundation::Job update_positions = make_job(
        "update_audio_listener_positions",
        "update",
        update_positions_job,
        bind_write(*this)
      );

      job_graph.Add(std::move(update_positions));
    }

    //---------------------------------------------------------------------------------------------
    void AudioListenerSystem::OnTerminate()
    {
      for (int i = 0; i < component_data_.data.size(); i++)
      {
        audio_system_->RemoveListener(component_data_.audio_listener[i]);
      }
      component_data_.data.Clear();
    }

    //---------------------------------------------------------------------------------------------
    void AudioListenerSystem::Destroy(ComponentHandleBase handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      audio_system_->RemoveListener(component_data_.audio_listener[data_index]);

      component_data_.data.Remove(handle);
    }

    //---------------------------------------------------------------------------------------------
    AudioListenerComponent::AudioListenerComponent()
      :
      system_(nullptr)
    {
    }

    //---------------------------------------------------------------------------------------------
    AudioListenerComponent::AudioListenerComponent(System& system, size_t handle)
      :
      ComponentHandleBase(handle),
      system_(&system)
    {

    }
  
    //---------------------------------------------------------------------------------------------
    float AudioListenerComponent::GetWeight()
    {
      return system_->GetWeight(*this);
    }
    float AudioListenerSystem::GetWeight(AudioListenerComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      return component_data_.audio_listener[data_index]->GetWeight();
    }

    //---------------------------------------------------------------------------------------------
    void  AudioListenerComponent::SetWeight(float weight)
    {
      system_->SetWeight(*this, weight);
    }
    void AudioListenerSystem::SetWeight(AudioListenerComponent handle, float weight)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      component_data_.audio_listener[data_index]->SetWeight(weight);
    }

    //---------------------------------------------------------------------------------------------
    void AudioListenerSystem::UpdatePositions()
    {
      for (int i = 0; i < component_data_.data.size(); i++)
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

        component_data_.audio_listener[i]->Set3DAttributes(attributes);
      }
    }
  }
}