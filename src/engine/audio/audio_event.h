#pragma once
#include "engine/audio/audio_structs.h"
#include "engine/audio/audio_parameter.h"

#include <foundation/containers/vector.h>
#include <foundation/memory/memory.h>

namespace FMOD
{
  namespace Studio
  {
    class EventInstance;
  }
}

namespace sulphur
{
  namespace engine
  {
    class AudioEventDescription;
    /**
    * @class sulphur::engine::AudioEvent
    * @brief Wraps the FMod EventInstance class
    * @author Rodi Lankester
    */
    class AudioEvent
    {
      friend class foundation::Memory;
    protected:
      /**
      * @brief Create an AudioEvent from a FMod EventInstance
      * @param[in] instance (FMOD::Studio::EventInstance*) The FMod event instance
      * @param[in] audio_event_description (AudioEventDescription*) The description that created the class
      */
      AudioEvent(FMOD::Studio::EventInstance* instance, AudioEventDescription* description);
    public:
      /**
      * @brief Release the event and underlying data
      */
      void Release();

      /**
      * @brief Get the current 3D attributes of the event
      * @returns (Audio3DAttributes) The current 3D attributes
      */
      Audio3DAttributes Get3DAttributes();

      /**
      * @brief Get the AudioEventDescription where this event is created from
      * @returns (AudioEventDescription) The event description
      */
      AudioEventDescription* GetDescription();

      /**
      * @brief Get the amount of parameters this event has 
      * @returns (int) The amount of parameters this event has 
      */
      int GetParameterCount();            

      /**
      * @brief Get a parameter by name 
      * @param[in] name (const char*) The name of the parameter
      * @returns (AudioParameter) The parameter that belongs to the name
      */
      AudioParameter GetParameter(const char* name);   

      /**
      * @brief Get a parameter by index
      * @param[in] index (int) The index of the parameter
      * @returns (AudioParameter) The parameter that belongs to the index
      */
      AudioParameter GetParameterByIndex(int index);      
      
      /**
      * @brief Get the value of a parameter by name 
      * @param[in] name (const char*) The name of the parameter
      * @returns (float) The value 
      */
      float GetParameterValue(const char* name);
      /**
      * @brief Get the value of a parameter by index
      * @param[in] index (int) The index of the parameter
      * @returns (float) The value
      */
      float GetParameterValueByIndex(int index);           

      /**
      * @brief Is this event paused?
      * @returns (bool) If this event is paused
      */
      bool GetPaused();

      /**
      * @brief Get the current pitch of the event
      * @returns (float) The current pitch
      */
      float GetPitch();

      /**
      * @brief Get the current PlaybackState of the event
      * @returns (PlaybackState) The current playback state
      */
      PlaybackState GetPlaybackState();

      // void GetProperty(); // todo (rodi) implement these properties

      /**
      * @brief Get the current reverb level of the event
      * @param[in] index (int) index of the low level reverb instance (0-3)
      * @returns (float) The current reverb level
      */
      float GetReverbLevel(int index);
      /**
      * @brief Get the current timeline position of the event
      * @returns (int) The current timeline position of the event
      */
      int GetTimelinePosition();

      //void GetUserData(); // todo (rodi) figure out userdata

      /**
      * @brief Get the current volume of the event
      * @returns (float) The current volume of the event
      */
      float GetVolume();

      /**
      * @brief Get if this event is virtual 
      * @returns (bool) Is the event currently virtual
      */
      bool IsVirtual();

      /**
      * @brief Set the 3D attributes of the event
      * @param[in] attributes (Audio3DAtrributes) The attributes you want to set it to
      */
      void Set3DAttributes(const Audio3DAttributes& attributes);

      /**
      * @brief Set the value of a parameter by name 
      * @param[in] name (const char*) The name of the parameter
      * @param[in] value (float) The value to set it to
      */
      void SetParameterValue(const char* name, float value); 
      /**
      * @brief Set the value of a parameter by name
      * @param[in] index (int) The index of the parameter
      * @param[in] value (float) The value to set it to
      */
      void SetParameterValueByIndex(int index, float value);     
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
      void SetPaused(bool paused);
      /**
      * @brief Set the current pitch of the event
      * @param[in] pitch (float) The pitch you want to set it to
      */
      void SetPitch(float pitch);


      /**
      * @brief Set the reverb level of the event
      * @param[in] index (int) Index of the low level reverb instance (0-3)
      * @param[in] reverb (float) The reverb level to set it to
      */
      void SetReverbLevel(int index, float reverb);
      /**
      * @brief Set the time line position of the event
      * @param[in] position (int) The position to set it to
      */
      void SetTimelinePosition(int position);

      //void setUserData(); // todo (rodi) figure out userdata

      /**
      * @brief Set the volume of the event
      * @param[in] volume (float) The volume to set it to
      */
      void SetVolume(float volume);

      /**
      * @brief Start the event
      */
      void Start();

      /**
      * @brief Stop the event
      * @param[in] fadeout (bool) Do you want the event to fade out?
      */
      void Stop(bool fadeout = false);

      /**
      * @brief Trigger a cue to move the timeline past a sustain point
      */
      void TriggerCue();

      // void GetChannelGroup(); // todo (rodi) implement channel groups)
      // void setCallback(); // todo (rodi) Implement callbacks
      // void setListenerMask(); //todo (rodi) Implement listener masks
      //void GetListenerMask();

      // void SetProperty();


    private:
      FMOD::Studio::EventInstance* instance_; //!< The FMod event instance
      AudioEventDescription* description_; //!< The description that created this
    };
  }
}