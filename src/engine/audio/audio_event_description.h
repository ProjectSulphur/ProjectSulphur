#pragma once
#include "engine/audio/audio_event.h"

#include <foundation/containers/vector.h>
#include <foundation/containers/string.h>
#include <foundation/memory/memory.h>
namespace FMOD
{
  namespace Studio
  {
    class EventDescription;
  }
}

namespace sulphur
{
  namespace engine
  {

    class AudioEvent;

    /**
    * @class sulphur::engine::AudioEventDescription
    * @brief Wraps FMod's EventDescription
    * @author Rodi Lankester
    */
    class AudioEventDescription
    {
      friend class AudioEvent;
      friend class AudioBank;
    protected:
      /**
      * @brief Create an AudioEventDescription from a FMod Event description
      * @param[in] description (FMOD::Studio::EventDescription) The FMod studio event description
      */
      AudioEventDescription(FMOD::Studio::EventDescription* description);
    public:
      /**
      * @brief Create an event instance from this event description
      * @returns (foundation::SharedPointer<AudioEvent>) The event instance
      */
      foundation::SharedPointer<AudioEvent> CreateInstance();

      /**
      * @brief Get the amount of instances made and not released from this description
      * @returns (int) The amount of instances made and not released from this description 
      */
      int GetInstanceCount();

      /**
      * @brief Get the length in miliseconds of this event
      * @returns (int) The length in miliseconds
      */
      int GetLength(); // in miliseconds

      /**
      * @brief Get the maximum distance attenuation for this event dsscription
      * @returns (float) the maximum distance attenuation
      */
      float GetMaximumDistance();
      /**
      * @brief Get the minimum distance attenuation for this event description
      * @returns (float) The minimum distance attenuation for this event discription
      */
      float GetMinimumDistance();

      /**
      * @brief Get the path of this event
      * @returns (const char*) The path
      */
      foundation::String GetPath();

      /**
      * @brief Get the sound size for this event 
      */
      float GetSoundSize();

      /**
      * @brief Does this event have a trigger cue?
      * @returns (bool) Does it have a trigger cue
      */
      bool HasCue();

      /**
      * @brief Is this event 3D?
      * @returns (bool) Is it 3D?
      */
      bool Is3D();
      /**
      * @brief Is this event a one shot event?
      * @returns (bool) Is it a one shot event?
      */
      bool IsOneshot();
      /**
      * @brief Is this event a snapshot?
      * @returns (bool) Is it a snapshot
      */
      bool IsSnapshot();
      /**
      * @brief Does this event contain one or more streamed sounds?
      * @returns (bool) Does it contain one or more streamed sounds?
      */
      bool IsStream();

      /**
      * @brief Release all instances of this event
      */
      void ReleaseAllInstances();

      // SetCallback // todo (Rodi) implement audio callbacks
    private:
      FMOD::Studio::EventDescription* description_; //!< The FMod EventDescription
    };
  }
}