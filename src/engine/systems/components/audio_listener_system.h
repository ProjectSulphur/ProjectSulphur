#pragma once
#include "engine/core/entity_system.h"
#include "engine/systems/component_system.h"

#include "engine/audio/audio_listener.h"

#include "engine/scripting/scriptable_object.h"
#include "engine/audio/audio_system.h"


namespace sulphur
{
  namespace engine
  {
    class AudioListenerSystem;

    /**
    * @class sulphur::engine::AudioListenerComponent
    * @brief Component that interfaces to an audio listener
    * @author Rodi Lankester
    */
    SCRIPT_CLASS() class AudioListenerComponent : public ComponentHandleBase
    {
    public:
      SCRIPT_NAME(AudioListenerComponent);
      SCRIPT_COMPONENT();

      using System = AudioListenerSystem; //!< The system type that this component uses

      /**
      * @brief Construct an audio listener component
      */
      AudioListenerComponent();

      /**
      * @brief Constructs an audio listener component with a system and a handle
      * @param[in] system (System&) The system that created this component
      * @param[in] handle (size_t) the handle that belongs to this component
      */
      AudioListenerComponent(System& system, size_t handle);

      /**
      * @brief Get this listener's weight
      * @returns (float) This listener's weight
      */
      SCRIPT_FUNC() float GetWeight();
      /**
      * @brief Set this listener's weight
      * @param[in] (float) This listeners weight
      */
      SCRIPT_FUNC() void SetWeight(float weight);
    private:
      System* system_; //!< The system that belongs to this component
    };

    /**
    * @class sulphur::engine::AudioListenerData
    * @brief The data belonging to audio listener components
    * @author Rodi Lankester
    */
    class AudioListenerData
    {
    public:

      /**
      * @brief Create an instance of AudioListenerData
      */
      AudioListenerData()
        : data((void**)&audio_listener)
      {

      }

      using ComponentSystemData = SystemData <
        AudioListener*,
        Entity
      >; //!< Specifies the data types to use

      AudioListener** audio_listener; //!< The audio listener instance belonging to this component
      Entity* entity; //!< The entity this component belongs to 
      ComponentSystemData data; //!< The actual data
    };

    /**
    * @class sulphur::engine::AudioListenerSystem
    * @brief The system belonging the the audio listener component
    * @author Rodi Lankester
    */
    class AudioListenerSystem : public IComponentSystem
    {
    public:
      /**
      * @brief Creates an instance of audio listener system
      */
      AudioListenerSystem();

      /**
      * @see sulphur::engine::IComponentSystem::Create
      */
      template<typename ComponentT>
      ComponentT Create(Entity& entity) { return Create(entity); };

      /**
      * @brief Destroy an audio listener component
      * @param[in] handle (AudioListenerComponent) The component to destroy
      */
      void Destroy(ComponentHandleBase handle) override;

      /**
      * @brief Create an audio listener component belonging to the passed entity
      * @param[in] entity (Entity&) The entity this component belongs to
      * @returns (AudioListenerComponent) The created audio listener component
      */
      AudioListenerComponent Create(Entity& entity);
      
      /**
      * @internal
      * @see sulphur::engine::IComponentSystem::OnInitialize
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;

      /**
      * @internal
      * @see sulphur::engine::IComponentSystem::OnTerminate
      */
      void OnTerminate() override;
     
      /**
      * @brief Get this listener's weight
      * @returns (float) This listener's weight
      */
      float GetWeight(AudioListenerComponent handle);
      /**
      * @brief Set this listener's weight
      * @param[in] (float) This listeners weight
      */
      void SetWeight(AudioListenerComponent handle, float weight);
    private:
      /**
      * @brief Update the positions of all audio listener components
      */
      void UpdatePositions();

      AudioSystem* audio_system_; //!< The instance of the audio system

      AudioListenerData component_data_; //!< An instance of the container that stores per-component data
    };
  }
}