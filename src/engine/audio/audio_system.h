#pragma once
#include "engine/audio/audio_bank.h"
#include "engine/audio/audio_listener.h"
#include "engine/audio/audio_command_replay.h"

#include "engine/systems/service_system.h"

#include "engine/scripting/scriptable_object.h"

#include <foundation/containers/vector.h>

namespace FMOD
{
  namespace Studio
  {
    class System;
  }
}

namespace sulphur
{
  namespace engine
  {
    class AudioBank;
    class AudioListener;

    /**
    * @class sulphur::engine::AudioSystem : public IServiceSystem<AudioSystem>
    * @brief Wraps the FMod Audio system class
    * @author Rodi Lankester
    */
    class AudioSystem : public IServiceSystem<AudioSystem>
    {
    public:
      /**
      * @brief Constructor
      */
      AudioSystem();

      /**
      * @brief Initialize the audio system
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;
      /**
      * @brief Shutdown the audio system
      */
      void OnTerminate() override;

      /**
      * @brief Update the audio system
      * @param[in] delta_time (float) the current frames delta time
      */
      void Update(float delta_time);

      /**
      * @brief Load an Audio bank from data
      * @param[in] data (char*) The data
      * @param[in] length (int) The length of the data
      * @param[in] is_master (bool) Is this bank a master bank?
      * @returns (AudioBank*) The loaded audio bank
      */
      AudioBank* LoadBank(char* data, int length, bool is_master = false);

      /**
      * @brief Load an Audio bank from a file
      * @param[in] path (const char*) The file path
      * @param[in] is_master (bool) Is this bank a master bank?
      * @returns (AudioBank*) The loaded audio bank
      */
      AudioBank* LoadBank(const char* path, bool is_master = false);

      /**
      * @brief Unload an audio bank
      * @param[in] bank (AudioBank*) The bank to unload
      */
      void UnloadBank(AudioBank* bank);

      /**
      * @brief Get a list of all loaded audio bank names
      * @returns (foundation::Vector<foundation::String>) The names of the banks
      */
      foundation::Vector<foundation::String> GetBankNames();

      /**
      * @brief Get an AudioBank by name
      * @param[in] name (const char*) The name
      * @returns (AudioBank*) the audio bank
      */
      AudioBank* GetBank(const char* name);

      /**
      * @brief Add an AudioListener to the scene
      * @returns (AudioListener*) The audio listener just created
      */
      AudioListener* AddListener();
      /**
      * @brief Remove an audio listener from the scene
      * @param[in] (AudioListener*) The audio listener to remove
      */
      void RemoveListener(AudioListener* listener);

      /**
      * @brief Get the underlying FMod system
      * @returns (FMOD::Studio::System*) The fmod system
      */
      FMOD::Studio::System* GetFModSystem();

      /**
      * @brief Start capturing all commands to the specified file path
      * @param[in] path (const char*) The path you want to capture the commands to
      */
      void StartCommandCapture(const char* path);

      /**
      * @brief Stop capturing commands
      */
      void StopCommandCapture();

      /**
      * @brief Load previously captured commands into a command replay from the specified file path
      * @param[in] path (const char*) The command replay file you want to load
      * @returns (AudioCommandReplay) The command replay instance
      */
      AudioCommandReplay LoadCommandReplay(const char* path);
    private:
      FMOD::Studio::System* system_; //!< The underlying fmod system

      foundation::Vector<AudioListener> listeners; //!< A list of all audio listeners

      foundation::Vector<AudioBank> banks_; //!< A list of the loaded banks
      foundation::Vector<foundation::String> bank_names_; //!< A list of the names of all loaded banks
    };

    /**
    * @class sulphur::engine::ScriptableAudio : public ScriptableObject
    * @brief Exposes the audio system to scripting
    * @author Rodi Lankester
    */
    SCRIPT_CLASS() class ScriptableAudio : public ScriptableObject
    {
    public: 
      SCRIPT_NAME(Audio);

      /**
      * @brief Initializes the scriptable audio with the audio system instance
      * @param[in] system (AudioSystem*) The audio system instance
      */
      static void Initialize(AudioSystem* system);

      /**
      * @brief Loads an audio bank from name
      * @param[in] bank (const foundation::String&) The name of the bank
      */
      SCRIPT_FUNC(static) void LoadBank(const foundation::String& bank);
      /**
      * @brief Loads a master audio bank from name
      * @param[in] bank (const foundation::String&) The name of the bank
      */
      SCRIPT_FUNC(static) void LoadMasterBank(const foundation::String& bank);
    private:
      static AudioSystem* system_; //!< The audio system instance
    };
  }
}