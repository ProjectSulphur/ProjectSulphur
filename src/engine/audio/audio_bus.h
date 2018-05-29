#pragma once 
#include <foundation/containers/string.h>

namespace FMOD
{
  namespace Studio
  {
    class Bus;
  }
}

namespace sulphur
{
  namespace engine
  {
    /**
    * @class sulphur::engine::AudioBus
    * @brief Interfaces to the FMod sytem to provide Bus functionality
    * @author Rodi Lankester
    */
    class AudioBus
    {
      friend class AudioBank;
    protected:
      /**
      * @brief Create an AudioBus instance from a FMOD::Studio::Bus instance
      * @param[in] fmod_vca (FMOD::Studio::Bus*) The FMOD::Studio::Bus instance
      */
      AudioBus(FMOD::Studio::Bus* fmod_bus);
    public:
      /**
      * @brief Get if this bus is muted
      * @returns (bool) Is this bus muted?
      */
      bool GetMute();
      /**
      * @brief Get the path of this bus 
      * @returns (foundation::String) The path of this bus
      */
      foundation::String GetPath();
      /**
      * @brief Get if this bus is paused
      * @returns (bool) Is this bus paused?
      */
      bool GetPaused();
      /**
      * @brief Get the volume of this bus 
      * @returns (float) The volume of this bus 
      */
      float GetVolume();

      /**
      * @brief Mute or unmute this bus 
      * @param[in] mute (bool) Do you want to mute or unmute this bus? 
      */
      void SetMute(bool mute);
      /**
      * @brief Pause or unpause this bus 
      * @param[in] paused (bool) Do you want to pause or unpause this bus 
      */
      void SetPaused(bool paused);
      /**
      * @brief Set the volume of this bus 
      * @param[in] volume (float) The volume you want this bus set to
      */
      void SetVolume(float volume);
      /**
      * @brief Stop all events belonging to this bus 
      * @param[in] fade (bool) Do you want to allow the audio to fade out 
      */
      void StopAllEvents(bool fade = true);

      // Studio::Bus::getChannelGroup no support for this as of yet
      //  lockChannelGroup no support for this as of yet
      // unlockChannelGroup no support for this as of yet
    protected:
      FMOD::Studio::Bus* fmod_bus_; //!< The FMOD::Studio::Bus instance
    };
  }
}