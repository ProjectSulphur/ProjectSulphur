#pragma once
#include "engine/core/entity_system.h"
#include "engine/systems/component_system.h"

#include "engine/audio/audio_event.h"

#include "engine/scripting/scriptable_object.h"
#include "engine/audio/audio_system.h"


namespace sulphur
{
  namespace engine
  {
    class AudioSnapshotSystem;

    /**
    * @class sulphur::engine::AudioSnapshotComponent
    * @brief Component that interfaces to an audio snapshot
    * @author Rodi Lankester
    */
    SCRIPT_CLASS() class AudioSnapshotComponent : public ComponentHandleBase
    {
    public:
      SCRIPT_NAME(AudioSnapshotComponent);
      SCRIPT_COMPONENT();

      using System = AudioSnapshotSystem; //!< The system type that this component uses

      /**
      * @brief Constructs an audio snapshot component
      */
      AudioSnapshotComponent();

      /**
      * @brief Constructs an audio snapshot component with a system and a handle
      * @param[in] system (System&) The system that created this component
      * @param[in] handle (size_t) the handle that belongs to this component
      */
      AudioSnapshotComponent(System& system, size_t handle);

      /**
      * @brief Set the bank where to load the event from
      * @param[in] bank (Sulphur::Foundation::String) The bank name
      */
      SCRIPT_FUNC() void SetBank(foundation::String bank);

      /**
      * @brief Set the path where to load the event from
      * @param[in] bank (Sulphur::Foundation::String) The path name
      */
      SCRIPT_FUNC() void SetPath(foundation::String path);

      /**
      @brief Get the current bank name
      @returns (Sulphur::Foundation::String) The current bank name
      */
      SCRIPT_FUNC() foundation::String GetBank();
      /**
      @brief Get the current path
      @returns (Sulphur::Foundation::String) The current path
      */
      SCRIPT_FUNC() foundation::String GetPath();

      /**
      * @brief Release the event and underlying data
      */
      SCRIPT_FUNC() void Release();

      /**
      * @brief Get the AudioSnapshotDescription where this event is created from
      * @returns (Sulphur::Engine::AudioEventDescription) The event description
      */
      AudioEventDescription* GetDescription();

      /**
      * @brief Get the amount of parameters this event has
      * @returns (int) The amount of parameters this event has
      */
      SCRIPT_FUNC() int GetParameterCount();

      /**
      * @brief Get a parameter by name
      * @param[in] name (sulphur::foundation::String) The name of the parameter
      * @returns (Sulphur::Engine::AudioParameter) The parameter that belongs to the name
      */
      AudioParameter GetParameter(foundation::String name);

      /**
      * @brief Get a parameter by index
      * @param[in] index (int) The index of the parameter
      * @returns (Sulphur::Engine::AudioParameter) The parameter that belongs to the index
      */
      AudioParameter GetParameterByIndex(int index);

      /**
      * @brief Get the value of a parameter by name
      * @param[in] name (sulphur::foundation::String) The name of the parameter
      * @returns (float) The value
      */
      SCRIPT_FUNC() float GetParameterValue(foundation::String name);

      /**
      * @brief Get the value of a parameter by index
      * @param[in] index (int) The index of the parameter
      * @returns (float) The value
      */
      SCRIPT_FUNC()float GetParameterValueByIndex(int index);
     
      /**
      * @brief Get the current PlaybackState of the event
      * @returns (Sulphur::Engine::PlaybackState) The current playback state
      */
      PlaybackState GetPlaybackState();

      /**
      * @brief Set the value of a parameter by name
      * @param[in] name (sulphur::foundation::String) The name of the parameter
      * @param[in] value (float) The value to set it to
      */
      SCRIPT_FUNC() void SetParameterValue(foundation::String name, float value);

      /**
      * @brief Set the value of a parameter by name
      * @param[in] index (int) The index of the parameter
      * @param[in] value (float) The value to set it to
      */
      SCRIPT_FUNC() void SetParameterValueByIndex(int index, float value);

      /**
      * @brief Set the values of parameters by a list of indices and values
      * @param[in] indices (Sulphur::Foundation::Vector<int>) A list of indices to set
      * @param[in] values (Sulphur::Foundation::Vector<float>) A list of values to set the indices to
      */
      void SetParameterValuesByIndices(foundation::Vector<int> indices, foundation::Vector<float> values);

      /**
      * @brief Start the event
      */
      SCRIPT_FUNC()void Start();

      /**
      * @brief Stop the event
      * @param[in] fadeout (bool) Do you want the event to fade out?
      */
      SCRIPT_FUNC() void Stop(bool fadeout = false);

    private:
      AudioSnapshotSystem* system_; //!< The system that belongs to this event
    };

    /**
    * @class sulphur::engine::AudioSnapshotData
    * @brief The data belonging to audio snapshot components
    * @author Rodi Lankester
    */
    class AudioSnapshotData
    {
    public:

      /**
      * @brief Create an instance of AudioSnapshotData
      */
      AudioSnapshotData()
        : data((void**)&bank_path)
      {

      }

      using ComponentSystemData = SystemData <
        foundation::String,
        foundation::String,
        foundation::SharedPointer<AudioEvent>,
        AudioBank*,
        bool,
        Entity
      >; //!< Specifies the data types to use

      foundation::String* bank_path; //!< The path to the current audio bank
      foundation::String* event_path; //!< The current event path
      foundation::SharedPointer<AudioEvent>* audio_event; //!< The instance of the currently loaded event
      AudioBank** audio_bank; //!< The currently selected audio bank
      bool* loaded; //!< Is this event loaded
      Entity* entity; //!< The entity to which this component belongs
      ComponentSystemData data; //!< The actual data container of this component
    };

    /**
    * @class sulphur::engine::AudioSnapshotSystem
    * @brief The system belonging the the audio snapshot component
    * @author Rodi Lankester
    */
    class AudioSnapshotSystem : public IComponentSystem
    {
    public:
      /**
      * @brief Constructs an audio snapshot system
      */
      AudioSnapshotSystem();

      /**
      * @see sulphur::engine::IComponentSystem::Create
      */
      template<typename ComponentT>
      ComponentT Create(Entity& entity) { return Create(entity); };


      /**
      * @see sulphur::engine::IComponentSystem::Destroy
      */
      void Destroy(ComponentHandleBase handle) override;

      /**
      * @brief Create an audio snapshot component belonging to the passed entity
      * @param[in] entity (Sulphur::Engine::Entity&) The entity this component belongs to
      * @returns (Sulphur::Engine::AudioSnapshotComponent) The created audio snapshot component
      */
      AudioSnapshotComponent Create(Entity& entity);

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
      * @param[in] handle (Sulphur::Engine::AudioSnapshotComponent) The handle of the component to perform this action for
      * @param[in] bank (Sulphur::Foundation::String) The bank name
      */
      SCRIPT_FUNC() void SetBank(AudioSnapshotComponent handle, foundation::String bank);
      /**
      * @brief Set the path of the event
      * @param[in] handle (Sulphur::Engine::AudioSnapshotComponent) The handle of the component to perform this action for
      * @param[in] bank (Sulphur::Foundation::String) The path
      */
      SCRIPT_FUNC() void SetPath(AudioSnapshotComponent handle, foundation::String path);

      /**
      @brief Get the current bank name
      * @param[in] handle (Sulphur::Engine::AudioSnapshotComponent) The handle of the component to perform this action for
      @returns (Sulphur::Foundation::String) The current bank name
      */
      SCRIPT_FUNC() foundation::String GetBank(AudioSnapshotComponent handle);
      /**
      @brief Get the current path
      * @param[in] handle (Sulphur::Engine::AudioSnapshotComponent) The handle of the component to perform this action for
      @returns (sulphur::foundation::String) The current path
      */
      SCRIPT_FUNC() foundation::String GetPath(AudioSnapshotComponent handle);

      /**
      * @brief Release the event and underlying data
      */
      void Release(ComponentHandleBase handle);

      /**
      * @brief Get the AudioSnapshotDescription where this event is created from
      * @param[in] handle (AudioSnapshotComponent) The handle of the component to perform this action for
      * @returns (AudioSnapshotDescription) The event description
      */
      AudioEventDescription* GetDescription(AudioSnapshotComponent handle);

      /**
      * @brief Get the amount of parameters this event has
      * @param[in] handle (AudioSnapshotComponent) The handle of the component to perform this action for
      * @returns (int) The amount of parameters this event has
      */
      int GetParameterCount(AudioSnapshotComponent handle);

      /**
      * @brief Get a parameter by name
      * @param[in] handle (AudioSnapshotComponent) The handle of the component to perform this action for
      * @param[in] name (foundation::String) The name of the parameter
      * @returns (Sulphur::Engine::AudioParameter) The parameter that belongs to the name
      */
      AudioParameter GetParameter(AudioSnapshotComponent handle, foundation::String name);

      /**
      * @brief Get a parameter by index
      * @param[in] handle (Sulphur::Engine::AudioSnapshotComponent) The handle of the component to perform this action for
      * @param[in] index (int) The index of the parameter
      * @returns (Sulphur::Engine::AudioParameter) The parameter that belongs to the index
      */
      AudioParameter GetParameterByIndex(AudioSnapshotComponent handle, int index);

      /**
      * @brief Get the value of a parameter by name
      * @param[in] handle (Sulphur::Engine::AudioSnapshotComponent) The handle of the component to perform this action for
      * @param[in] name (sulphur::foundation::String) The name of the parameter
      * @returns (float) The value
      */
      float GetParameterValue(AudioSnapshotComponent handle, foundation::String name);

      /**
      * @brief Get the value of a parameter by index
      * @param[in] handle (Sulphur::Engine::AudioSnapshotComponent) The handle of the component to perform this action for
      * @param[in] index (int) The index of the parameter
      * @returns (float) The value
      */
      float GetParameterValueByIndex(AudioSnapshotComponent handle, int index);

      /**
      * @brief Get the current PlaybackState of the event
      * @param[in] handle (Sulphur::Engine::AudioSnapshotComponent) The handle of the component to perform this action for
      * @returns (Sulphur::Engine::PlaybackState) The current playback state
      */
      PlaybackState GetPlaybackState(AudioSnapshotComponent handle);
      
      /**
      * @brief Set the value of a parameter by name
      * @param[in] handle (Sulphur::Engine::AudioSnapshotComponent) The handle of the component to perform this action for
      * @param[in] name (sulphur::foundation::String) The name of the parameter
      * @param[in] value (float) The value to set it to
      */
      void SetParameterValue(AudioSnapshotComponent handle, foundation::String name, float value);

      /**
      * @brief Set the value of a parameter by name
      * @param[in] handle (Sulphur::Engine::AudioSnapshotComponent) The handle of the component to perform this action for
      * @param[in] index (int) The index of the parameter
      * @param[in] value (float) The value to set it to
      */
      void SetParameterValueByIndex(AudioSnapshotComponent handle, int index, float value);

      /**
      * @brief Set the values of parameters by a list of indices and values
      * @param[in] handle (Sulphur::Engine::AudioSnapshotComponent) The handle of the component to perform this action for
      * @param[in] indices (sulphur::foundation::Vector<int>) A list of indices to set
      * @param[in] values (sulphur::foundation::Vector<float>) A list of values to set the indices to
      */
      void SetParameterValuesByIndices(AudioSnapshotComponent handle, foundation::Vector<int> indices, foundation::Vector<float> values);
    
      /**
      * @brief Start the event
      * @param[in] handle (Sulphur::Engine::AudioSnapshotComponent) The handle of the component to perform this action for
      */
      void Start(AudioSnapshotComponent handle);

      /**
      * @brief Stop the event
      * @param[in] handle (Sulphur::Engine::AudioSnapshotComponent) The handle of the component to perform this action for
      * @param[in] fadeout (bool) Do you want the event to fade out?
      */
      void Stop(AudioSnapshotComponent handle, bool fadeout);
    private:
      /**
      * @brief Update the position of all audio snapshot components
      */
      void UpdatePositions();

      AudioSystem* audio_system_; //!< The instance of the audio system
      AudioSnapshotData component_data_; //!< The data of the component
    };
  }
}