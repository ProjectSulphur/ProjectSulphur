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
    class AudioEventSystem;

    /**
    * @class sulphur::engine::AudioEventComponent
    * @brief Component that interfaces to an audio event
    * @author Rodi Lankester
    */
    SCRIPT_CLASS() class AudioEventComponent : public ComponentHandleBase
    {
    public:
      SCRIPT_NAME(AudioEventComponent);
      SCRIPT_COMPONENT();

      using System = AudioEventSystem; //!< The system type that this component uses
      
      /**
      * @brief Constructs an audio event component
      */
      AudioEventComponent();

      /**
      * @brief Constructs an audio event component with a system and a handle
      * @param[in] system (System&) The system that created this component
      * @param[in] handle (size_t) the handle that belongs to this component
      */
      AudioEventComponent(System& system, size_t handle);

      /**
      * @brief Set the bank where to load the event from
      * @param[in] bank (foundation::String) The bank name
      */
      SCRIPT_FUNC() void SetBank(foundation::String bank);

      /**
      * @brief Set the path where to load the event from
      * @param[in] bank (foundation::String) The path name
      */
      SCRIPT_FUNC() void SetPath(foundation::String path);

      /**
      @brief Get the current bank name
      @returns (foundation::String) The current bank name
      */
      SCRIPT_FUNC() foundation::String GetBank();
      /**
      @brief Get the current path
      @returns (foundation::String) The current path
      */
      SCRIPT_FUNC() foundation::String GetPath();

      /**
      * @brief Release the event and underlying data
      */
      SCRIPT_FUNC() void Release();

      /**
      * @brief Get the AudioEventDescription where this event is created from
      * @returns (AudioEventDescription) The event description
      */
      AudioEventDescription* GetDescription();

      /**
      * @brief Get the amount of parameters this event has
      * @returns (int) The amount of parameters this event has
      */
      SCRIPT_FUNC() int GetParameterCount();

      /**
      * @brief Get a parameter by name
      * @param[in] name (foundation::String) The name of the parameter
      * @returns (AudioParameter) The parameter that belongs to the name
      */
      AudioParameter GetParameter(foundation::String name);

      /**
      * @brief Get a parameter by index
      * @param[in] index (int) The index of the parameter
      * @returns (AudioParameter) The parameter that belongs to the index
      */
      AudioParameter GetParameterByIndex(int index);

      /**
      * @brief Get the value of a parameter by name
      * @param[in] name (foundation::String) The name of the parameter
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
      * @brief Is this event paused?
      * @returns (bool) If this event is paused
      */
      SCRIPT_FUNC() bool GetPaused();

      /**
      * @brief Get the current pitch of the event
      * @returns (float) The current pitch
      */
      SCRIPT_FUNC() float GetPitch();

      /**
      * @brief Get the current PlaybackState of the event
      * @returns (PlaybackState) The current playback state
      */
      PlaybackState GetPlaybackState();

      /**
      * @brief Get the current reverb level of the event
      * @param[in] index (int) index of the low level reverb instance (0-3)
      * @returns (float) The current reverb level
      */
      SCRIPT_FUNC() float GetReverbLevel(int index);
      /**
      * @brief Get the current timeline position of the event
      * @returns (int) The current timeline position of the event
      */
      SCRIPT_FUNC() int GetTimelinePosition();

      /**
      * @brief Get the current volume of the event
      * @returns (float) The current volume of the event
      */
      SCRIPT_FUNC() float GetVolume();

      /**
      * @brief Get if this event is virtual
      * @returns (bool) Is the event currently virtual
      */
      SCRIPT_FUNC() bool IsVirtual();

      /**
      * @brief Set the value of a parameter by name
      * @param[in] name (foundation::String) The name of the parameter
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
      * @param[in] indices (foundation::Vector<int>) A list of indices to set
      * @param[in] values (foundation::Vector<float>) A list of values to set the indices to
      */
      void SetParameterValuesByIndices(foundation::Vector<int> indices, foundation::Vector<float> values);

      /**
      * @brief Set if the event is paused
      * @param[in] (bool) Is the event paused?
      */
      SCRIPT_FUNC() void SetPaused(bool paused);
      /**
      * @brief Set the current pitch of the event
      * @param[in] pitch (float) The pitch you want to set it to
      */
      SCRIPT_FUNC() void SetPitch(float pitch);

      /**
      * @brief Set the reverb level of the event
      * @param[in] index (int) Index of the low level reverb instance (0-3)
      * @param[in] reverb (float) The reverb level to set it to
      */
      SCRIPT_FUNC() void SetReverbLevel(int index, float reverb);
      /**
      * @brief Set the time line position of the event
      * @param[in] position (int) The position to set it to
      */
      SCRIPT_FUNC() void SetTimelinePosition(int position);

      /**
      * @brief Set the volume of the event
      * @param[in] volume (float) The volume to set it to
      */
      SCRIPT_FUNC() void SetVolume(float volume);

      /**
      * @brief Start the event
      */
      SCRIPT_FUNC()void Start();

      /**
      * @brief Stop the event
      * @param[in] fadeout (bool) Do you want the event to fade out?
      */
      SCRIPT_FUNC() void Stop(bool fadeout = false);

      /**
      * @brief Trigger a cue to move the timeline past a sustain point
      */
      SCRIPT_FUNC() void TriggerCue();

      private:
        AudioEventSystem* system_; //!< The system that belongs to this event
    };
    
    /**
    * @class sulphur::engine::AudioEventData
    * @brief The data belonging to audio event components
    * @author Rodi Lankester
    */
    class AudioEventData
    {
    public:

      /**
      * @brief Create an instance of AudioEventData
      */
      AudioEventData() 
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
    * @class sulphur::engine::AudioEventSystem
    * @brief The system belonging the the audio event component
    * @author Rodi Lankester
    */
    class AudioEventSystem : public IComponentSystem
    {
    public:
      /**
      * @brief Constructs an audio event system
      */
      AudioEventSystem();

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
      * @brief Create an audio event component belonging to the passed entity
      * @param[in] entity (Entity&) The entity this component belongs to
      * @returns (AudioEventComponent) The created audio event component
      */
      AudioEventComponent Create(Entity& entity);
      
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
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @param[in] bank (foundation::String) The bank name
      */
      SCRIPT_FUNC() void SetBank(AudioEventComponent handle, foundation::String bank);
      /**
      * @brief Set the path of the event
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @param[in] bank (foundation::String) The path
      */
      SCRIPT_FUNC() void SetPath(AudioEventComponent handle, foundation::String path);

      /**
      @brief Get the current bank name
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      @returns (foundation::String) The current bank name
      */
      SCRIPT_FUNC() foundation::String GetBank(AudioEventComponent handle);
      /**
      @brief Get the current path
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      @returns (foundation::String) The current path
      */
      SCRIPT_FUNC() foundation::String GetPath(AudioEventComponent handle);

      /**
      * @brief Release the event and underlying data
      */
      void Release(ComponentHandleBase handle);

      /**
      * @brief Get the AudioEventDescription where this event is created from
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @returns (AudioEventDescription) The event description
      */
      AudioEventDescription* GetDescription(AudioEventComponent handle);

      /**
      * @brief Get the amount of parameters this event has
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @returns (int) The amount of parameters this event has
      */
       int GetParameterCount(AudioEventComponent handle);

      /**
      * @brief Get a parameter by name
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @param[in] name (foundation::String) The name of the parameter
      * @returns (AudioParameter) The parameter that belongs to the name
      */
      AudioParameter GetParameter(AudioEventComponent handle, foundation::String name);

      /**
      * @brief Get a parameter by index
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @param[in] index (int) The index of the parameter
      * @returns (AudioParameter) The parameter that belongs to the index
      */
      AudioParameter GetParameterByIndex(AudioEventComponent handle, int index);

      /**
      * @brief Get the value of a parameter by name
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @param[in] name (foundation::String) The name of the parameter
      * @returns (float) The value
      */
       float GetParameterValue(AudioEventComponent handle, foundation::String name);

      /**
      * @brief Get the value of a parameter by index
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @param[in] index (int) The index of the parameter
      * @returns (float) The value
      */
      float GetParameterValueByIndex(AudioEventComponent handle, int index);

      /**
      * @brief Is this event paused?
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @returns (bool) If this event is paused
      */
       bool GetPaused(AudioEventComponent handle);

      /**
      * @brief Get the current pitch of the event
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @returns (float) The current pitch
      */
       float GetPitch(AudioEventComponent handle);

      /**
      * @brief Get the current PlaybackState of the event
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @returns (PlaybackState) The current playback state
      */
      PlaybackState GetPlaybackState(AudioEventComponent handle);

      /**
      * @brief Get the current reverb level of the event
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @param[in] index (int) index of the low level reverb instance (0-3)
      * @returns (float) The current reverb level
      */
       float GetReverbLevel(AudioEventComponent handle, int index);
      /**
      * @brief Get the current timeline position of the event
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @returns (int) The current timeline position of the event
      */
       int GetTimelinePosition(AudioEventComponent handle);

      /**
      * @brief Get the current volume of the event
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @returns (float) The current volume of the event
      */
       float GetVolume(AudioEventComponent handle);

      /**
      * @brief Get if this event is virtual
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @returns (bool) Is the event currently virtual
      */
       bool IsVirtual(AudioEventComponent handle);

      /**
      * @brief Set the value of a parameter by name
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @param[in] name (foundation::String) The name of the parameter
      * @param[in] value (float) The value to set it to
      */
       void SetParameterValue(AudioEventComponent handle, foundation::String name, float value);

      /**
      * @brief Set the value of a parameter by name
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @param[in] index (int) The index of the parameter
      * @param[in] value (float) The value to set it to
      */
       void SetParameterValueByIndex(AudioEventComponent handle, int index, float value);

      /**
      * @brief Set the values of parameters by a list of indices and values
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @param[in] indices (foundation::Vector<int>) A list of indices to set
      * @param[in] values (foundation::Vector<float>) A list of values to set the indices to
      */
       void SetParameterValuesByIndices(AudioEventComponent handle, foundation::Vector<int> indices, foundation::Vector<float> values);

      /**
      * @brief Set if the event is paused
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @param[in] (bool) Is the event paused?
      */
       void SetPaused(AudioEventComponent handle, bool paused);
      /**
      * @brief Set the current pitch of the event
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @param[in] pitch (float) The pitch you want to set it to
      */
       void SetPitch(AudioEventComponent handle, float pitch);

      /**
      * @brief Set the reverb level of the event
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @param[in] index (int) Index of the low level reverb instance (0-3)
      * @param[in] reverb (float) The reverb level to set it to
      */
       void SetReverbLevel(AudioEventComponent handle, int index, float reverb);
      /**
      * @brief Set the time line position of the event
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @param[in] position (int) The position to set it to
      */
       void SetTimelinePosition(AudioEventComponent handle, int position);

      /**
      * @brief Set the volume of the event
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @param[in] volume (float) The volume to set it to
      */
       void SetVolume(AudioEventComponent handle, float volume);

      /**
      * @brief Start the event
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      */
      void Start(AudioEventComponent handle);

      /**
      * @brief Stop the event
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      * @param[in] fadeout (bool) Do you want the event to fade out?
      */
       void Stop(AudioEventComponent handle, bool fadeout);

      /**
      * @brief Trigger a cue to move the timeline past a sustain point
      * @param[in] handle (AudioEventComponent) The handle of the component to perform this action for
      */
       void TriggerCue(AudioEventComponent handle);
    private:
      /**
      * @brief Update the position of all audio event components
      */
      void UpdatePositions();

      AudioSystem* audio_system_;
      AudioEventData component_data_; //!< An instance of the container that stores per-component data
    };
  }
}