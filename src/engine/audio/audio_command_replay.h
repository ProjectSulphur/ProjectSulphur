#pragma once 
#include "engine/audio/audio_structs.h"
#include <foundation/containers/string.h>
namespace sulphur
{
  namespace engine
  {
    /**
    * @struct sulphur::engine::AudioInstanceType
    * @brief Naming scheme conformant version of FMOD_STUDIO_INSTANCE_TYPE
    * @author Rodi Lankester
    */
    enum struct AudioInstanceType
    {
      kNone = FMOD_STUDIO_INSTANCETYPE_NONE,                          //!< No type known
      kSystem = FMOD_STUDIO_INSTANCETYPE_SYSTEM,                      //!< This is a system
      kEventDescription = FMOD_STUDIO_INSTANCETYPE_EVENTDESCRIPTION,  //!< This is an event description
      kEvent = FMOD_STUDIO_INSTANCETYPE_EVENTINSTANCE,                //!< This is an event
      kParameter = FMOD_STUDIO_INSTANCETYPE_PARAMETERINSTANCE,        //!< This is a parameter
      kBus = FMOD_STUDIO_INSTANCETYPE_BUS,                            //!< This is a bus
      kVCA = FMOD_STUDIO_INSTANCETYPE_VCA,                            //!< This is a VCA
      kBank = FMOD_STUDIO_INSTANCETYPE_BANK,                          //!< This is a bank
      kCommandReplay = FMOD_STUDIO_INSTANCETYPE_COMMANDREPLAY         //!< This is a command replay
    };

    /**
    * @struct sulphur::engine::AudioCommandInfo
    * @brief Naming scheme conformant version of FMOD_STUDIO_COMMANDINFO
    * @author Rodi Lankester
    */
    struct AudioCommandInfo
    {
      const char* commandname;                //!< The full name of the API function for this command.
      int parentcommandindex;                 //!< For commands that operate on an instance, this is the command that created the instance.
      int framenumber;                        //!< The frame the command belongs to. 
      float frametime;                        //!< The playback time at which this command will be executed. 
      AudioInstanceType  instancetype;        //!< The type of object that this command uses as an instance. 
      AudioInstanceType  outputtype;          //!< The type of object that this command outputs, if any.
      unsigned int  instancehandle;           //!< The original handle value of the instance.  This will no longer correspond to any actual object in playback. 
      unsigned int  outputhandle;             //!< The original handle value of the command output.  This will no longer correspond to any actual object in playback. 
    };

    /**
    * @class sulphur::engine::AudioCommandReplay
    * @brief Wraps the FMod CommandReplay class
    * @author Rodi Lankester
    */
    class AudioCommandReplay
    {
      friend class AudioSystem;
    protected:
      /**
      * @brief Create an instance of AudioCommandReplay from a FMOD::Studio::CommandReplay instance
      * @param[in] command_replay (FMOD::Studio::CommandReplay*) The FMOD::STUDIO::CommandReplay instance
      */
      AudioCommandReplay(FMOD::Studio::CommandReplay* command_replay);
    public:
      /**
      * @brief Start the command replay
      */
      void Start();
      /**
      * @brief Stop the command replay
      */
      void Stop();
      /**
      * @brief Release the FMOD::Studio::CommandReplay instance
      */
      void release();

      /**
      * @brief Get the command played at a set time
      * @param[in] time (const float&) The time at which you want to get the command
      * @returns (int) The command id
      */
      int GetCommandAtTime(const float& time);
      /**
      * @brief Get the amount of commands in this command replay
      * @returns (int) The amount of commands
      */
      int GetCommandCount();

      /**
      * @brief Get detailed info about the command with the id passed
      * @param[in] id (int) The command id you want to get more info about
      * @returns (AudioCommandInfo) Detailed information about the command with the specified id
      */
      AudioCommandInfo GetCommandInfo(int id);
      /**
      * @brief Get the command string for the command with the id passed
      * @params[in] id (int) The command id you want to get the command string for
      * @returns (foundation::String) The command string
      */
      foundation::String GetCommandString(int id);

      /**
      * @brief Get the id for the command that is currently playing
      * @returns (int) The current command's id
      */
      int GetCurrentCommand();

      /**
      * @brief Get the total playback time of the command replay
      * @returns (float) The total playback time of the command replay
      */
      float GetLength();
      /**
      * @brief Get if the command replay is paused
      * @returns (bool) Is it paused
      */
      bool GetPaused();
      /**
      * @brief Get the current playback state of the command replay
      * @returns (PlaybackState) The current playback state
      */
      PlaybackState GetPlaybackState();

      /**
      * @brief Seek to the command with the specified id
      * @param[in] id (int) The command you want to seek to
      */
      void SeekToCommand(int id);
      /**
      * @brief Seek to the specified time
      * @param[in] time (float) The time you want to seek to
      */
      void SeekToTime(float time);
      /**
      * @brief Set the folder you want the command replay to load banks from
      * @param[in] path (const char*) The path you want the command replay to load banks for
      */
      void SetBankPath(const char* path);

      /**
      * @brief Set if the command replay is paused
      * @param[in] paused (bool) Do you want it paused?
      */
      void SetPaused(bool paused);
    private:
      FMOD::Studio::CommandReplay* command_replay_;

      // void setCreateInstanceCallback();       // todo(rodi) audio callbacks
      // void setFrameCallback();              // todo(rodi) audio callbacks
      // void setLoadBankCallback();           // todo(rodi) audio callbacks
    };
  }
}