#pragma once
#include "engine/audio/audio_event_description.h"
#include "engine/audio/audio_bus.h"
#include "engine/audio/audio_vca.h"

#include <foundation/containers/vector.h>
#include <foundation/containers/string.h>

namespace FMOD
{
  namespace Studio
  {
    class Bank;
  }
}

namespace sulphur
{
  namespace engine
  {
    /**
    * @class sulphur::engine::AudioBank
    * @brief Wraps the FMod audio bank class
    * @author Rodi Lankester
    */
    class AudioBank
    {
      friend class AudioSystem;
    protected:
      /**
      * @brief Initializes the audio bank with the FMOD audio bank
      * @param[in] bank (FMOD::Studio::Bank*) The fmod studio audio bank
      * @param[in] is_master (bool) Is this bank a master bank
      */
      AudioBank(FMOD::Studio::Bank* bank, bool is_master = false);

      /**
      * @brief Unload this bank
      */
      void Unload();
    public:
      /**
      * @brief Get the amount of busses in the bank
      * @returns (int) The amount of busses
      */
      int GetBusCount();

      /**
      * @brief Get all busses contained in this bank
      * @returns (foundation::Vector<AudioBus>) All busses in this bank 
      */
      foundation::Vector<AudioBus> GetBusses();
      /**
      * @brief Get the names for all busses contained in this bank 
      * @retuns (foundation::Vector<foundation::String>>) The names of all busses in this bank
      */
      foundation::Vector<foundation::String> GetBusNames();
      /**
      * @brief Get a bus from this bank by it's name 
      * @param[in] name (const foundation::String&) The name of the bus 
      * @returns (AudioBus) The bus that belongs to the name
      */
      AudioBus* GetBusByName(const foundation::String& name);

      /**
      * @brief Get the amount of events in the bank
      * @returns (int) The amount of events
      */
      int GetEventCount();

      /**
      * @brief Get the event descriptions for all events in the audio bank
      * @returns (foundation::Vector<AudioEventDescription> The event descriptions
      */
      foundation::Vector<AudioEventDescription> GetEventDescriptions();
      /**
      * @brief Get the name of all event descriptions in the audio bank
      * @returns (foundation::Vector<foundation::String>) The names of the descriptions
      */
      foundation::Vector<foundation::String> GetEventNames();
      /**
      * @brief Get the event description by name
      * @param[in] name (foundation::String) The name of the event
      * @returns (AudioEventDescription) The event description
      */
      AudioEventDescription* GetEventDescriptionByName(const foundation::String& name);
      
      /**
      * @brief Get the path to the audio bank 
      * @returns (const char*) The path
      */
      foundation::String GetPath();

      /**
      * @brief Get the amount of VCA's in the audio bank
      * @returns (int) The amount of VCA's
      */
      int GetVCACount();

      /**
      * @brief Get all VCAs contained in this bank
      * @returns (foundation::Vector<AudioVCA>) All VCAs in this bank
      */
      foundation::Vector<AudioVCA> GetVCAs();
      /**
      * @brief Get the names for all VCAs contained in this bank
      * @retuns (foundation::Vector<foundation::String>>) The names of all VCAs in this bank
      */
      foundation::Vector<foundation::String> GetVCANames();
      /**
      * @brief Get a VCA from this bank by it's name
      * @param[in] name (const foundation::String&) The name of the VCA
      * @returns (AudioVCA) The VCA that belongs to the name
      */
      AudioVCA* GetVCAByName(const foundation::String& name);

    private:
      FMOD::Studio::Bank* bank_; //!< The fmod studio audio bank
      foundation::Vector<AudioEventDescription> event_descriptions_; //!< A list of all event descriptions in the bank
      foundation::Vector<foundation::String> event_names_; //!< A list of all event names in the bank

      foundation::Vector<AudioBus> busses_; //!< A list of all busses in this bank 
      foundation::Vector<foundation::String> bus_names_;

      foundation::Vector<AudioVCA> vcas_; //!< A list of all busses in this bank 
      foundation::Vector<foundation::String> vca_names_;
    };    
  }
}