#pragma once
#include "engine/core/entity_system.h"
#include "engine/systems/component_system.h"

#include "engine/audio/audio_bus.h"

#include "engine/scripting/scriptable_object.h"
#include "engine/audio/audio_system.h"

namespace sulphur
{
  namespace engine
  {
    class AudioBusSystem;

    /**
    * @class sulphur::engine::AudioBusComponent : public sulphur::engine::ComponentHandleBase
    * @brief Component that interfaces to an audio bus
    * @author Rodi Lankester
    */
    SCRIPT_CLASS() class AudioBusComponent : public ComponentHandleBase
    {   
    public:
      SCRIPT_NAME(AudioBusComponent);
      SCRIPT_COMPONENT();

      using System = AudioBusSystem; //!< Defines the type of system this component uses

      /**
      * @brief Construct an audio bus component
      */
      AudioBusComponent();

      /**
      * @brief Constructs an audio bus component with a system and a handle
      * @param[in] system (System&) The system that created this component
      * @param[in] handle (size_t) the handle that belongs to this component
      */
      AudioBusComponent(System& system, size_t handle);

      /**
      * @brief Set the bank where to load the bus from
      * @param[in] bank (Sulphur::foundation::String) The bank name
      */
      SCRIPT_FUNC() void SetBank(foundation::String bank);

      /**
      * @brief Set the path where to load the bus from
      * @param[in] bank (Sulphur::foundation::String) The path name
      */
      SCRIPT_FUNC() void SetPath(foundation::String path);

      /**
      @brief Get the current bank name
      @returns (Sulphur::foundation::String) The current bank name
      */
      SCRIPT_FUNC() foundation::String GetBank();
      /**
      @brief Get the current path
      @returns (Sulphur::foundation::String) The current path
      */
      SCRIPT_FUNC() foundation::String GetPath();

      /**
      * @brief get if this bus is muted
      * @returns (bool) is this bus muted?
      */
      SCRIPT_FUNC() bool GetMute();

      /**
      * @brief get if this bus is paused
      * @returns (bool) is this bus paused?
      */
      SCRIPT_FUNC() bool GetPaused();

      /**
      * @brief get the volume of this bus
      * @returns (float) the volume of this bus
      */
      SCRIPT_FUNC() float GetVolume();

      /**
      * @brief mute or unmute this bus
      * @param[in] mute (bool) do you want to mute or unmute this bus?
      */
      SCRIPT_FUNC() void SetMute(bool mute);

      /**
      * @brief pause or unpause this bus
      * @param[in] paused (bool) do you want to pause or unpause this bus
      */
      SCRIPT_FUNC() void SetPaused(bool paused);

      /**
      * @brief set the volume of this bus
      * @param[in] volume (float) the volume you want this bus set to
      */
      SCRIPT_FUNC() void SetVolume(float volume);

      /**
      * @brief stop all events belonging to this bus
      * @param[in] fade (bool) do you want to allow the audio to fade out
      */
      SCRIPT_FUNC() void StopAllEvents(bool fade = true);
    private:
      System* system_; //!< The system this component belongs to
    };

    /**
    * @class sulphur::engine::AudioBusData
    * @brief The data belonging to audio bus components
    * @author Rodi Lankester
    */
    class AudioBusData
    {
    public:

      /**
      * @brief Create an instance of AudioBusData
      */
      AudioBusData()
        : data((void**)&bank_path)
      {

      }

      using ComponentSystemData = SystemData <
        foundation::String,
        foundation::String,
        AudioBus*,
        AudioBank*,
        bool,
        Entity
      >; //!< Specifies the data types to use

      foundation::String* bank_path; //!< The path to the current audio bank
      foundation::String* bus_path; //!< The current event path
      AudioBus** audio_bus; //!< The instance of the currently loaded event
      AudioBank** audio_bank; //!< The currently selected audio bank
      bool* loaded; //!< Is this event loaded
      ComponentSystemData data; //!< The actual data container of this component
    };

    /**
    * @class sulphur::engine::AudioBusSystem
    * @brief The system belonging the the audio bus component
    * @author Rodi Lankester
    */
    class AudioBusSystem : public IComponentSystem
    {
    public:
      /**
      * @brief Constructs an audio bus sytem
      */
      AudioBusSystem();

      /**
      * @see sulphur::engine::IComponentSystem::Create
      */
      template<typename ComponentT>
      ComponentT Create(Entity& entity) { return Create(entity); };

      /**
      * @brief Create an audio bus component belonging to the passed entity
      * @param[in] entity (Entity&) The entity this component belongs to
      * @returns (Sulphur::Engine::AudioBusComponent) The created audio bus component
      */
      AudioBusComponent Create(Entity& entity);

      /**
      * @brief Destroy an audio bus component
      * @param[in] handle (Sulphur::Engine::AudioBusComponent) The component to destroy
      */
      void Destroy(ComponentHandleBase handle) override;

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
      * @brief Set the bank where to load the event from
      * @param[in] handle (Sulphur::Engine::AudioBusComponent) The handle of the component to perform this action for
      * @param[in] bank (Sulphur::foundation::String) The bank name
      */
      void SetBank(AudioBusComponent handle, foundation::String bank);
      /**
      * @brief Set the path of the event
      * @param[in] handle (Sulphur::Engine::AudioBusComponent) The handle of the component to perform this action for
      * @param[in] bank (Sulphur::foundation::String) The path
      */
      void SetPath(AudioBusComponent handle, foundation::String path);

      /**
      * @brief Get the current bank name
      * @param[in] handle (Sulphur::Engine::AudioBusComponent) The handle of the component to perform this action for
      * @returns (Sulphur::foundation::String) The current bank name
      */
      foundation::String GetBank(AudioBusComponent handle);
      /**
      * @brief Get the current path
      * @param[in] handle (Sulphur::Engine::AudioBusComponent) The handle of the component to perform this action for
      * @returns (Sulphur::foundation::String) The current path
      */
      foundation::String GetPath(AudioBusComponent handle);

      /**
      * @brief get if this bus is muted
      * @param[in] component (sulphur::engine::AudioBusComponent) The component to perform the action on
      * @returns (bool) is this bus muted?
      */
      bool GetMute(AudioBusComponent component);

      /**
      * @brief get if this bus is paused
      * @param[in] component (sulphur::engine::AudioBusComponent) The component to perform the action on
      * @returns (bool) is this bus paused?
      */
      bool GetPaused(AudioBusComponent component);

      /**
      * @brief get the volume of this bus
      * @param[in] component (sulphur::engine::AudioBusComponent) The component to perform the action on
      * @returns (float) the volume of this bus
      */
      float GetVolume(AudioBusComponent component);

      /**
      * @brief mute or unmute this bus
      * @param[in] component (sulphur::engine::AudioBusComponent) The component to perform the action on
      * @param[in] mute (bool) do you want to mute or unmute this bus?
      */
      void SetMute(AudioBusComponent component, bool mute);

      /**
      * @brief pause or unpause this bus
      * @param[in] component (sulphur::engine::AudioBusComponent) The component to perform the action on
      * @param[in] paused (bool) do you want to pause or unpause this bus
      */
      void SetPaused(AudioBusComponent component, bool paused);

      /**
      * @brief set the volume of this bus
      * @param[in] component (sulphur::engine::AudioBusComponent) The component to perform the action on
      * @param[in] volume (float) the volume you want this bus set to
      */
      void SetVolume(AudioBusComponent component, float volume);

      /**
      * @brief stop all events belonging to this bus
      * @param[in] component (sulphur::engine::AudioBusComponent) The component to perform the action on
      * @param[in] fade (bool) do you want to allow the audio to fade out
      */
      void StopAllEvents(AudioBusComponent component, bool fade = true);
    private:
      AudioSystem* audio_system_; //!< The instance of the audio system
      AudioBusData data_; //!< The actual data describing the components
    };
  }
}