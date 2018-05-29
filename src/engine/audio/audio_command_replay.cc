#include "engine/audio/audio_command_replay.h"
#include "engine/audio/audio_utils.h"

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------
    AudioCommandReplay::AudioCommandReplay(FMOD::Studio::CommandReplay* command_replay)
    {
      command_replay_ = command_replay;
    }

    //------------------------------------------------------------------------------
    void AudioCommandReplay::Start()
    {
      AudioUtils::CheckError(command_replay_->start());
    }

    //------------------------------------------------------------------------------
    void AudioCommandReplay::Stop()
    {
      AudioUtils::CheckError(command_replay_->stop());
    }

    //------------------------------------------------------------------------------
    void AudioCommandReplay::release()
    {
      AudioUtils::CheckError(command_replay_->release());
      command_replay_ = nullptr;
    }

    //------------------------------------------------------------------------------
    int AudioCommandReplay::GetCommandAtTime(const float& time)
    {
      int id = 0;
      AudioUtils::CheckError(command_replay_->getCommandAtTime(time, &id));
      return id;
    }

    //------------------------------------------------------------------------------
    int AudioCommandReplay::GetCommandCount()
    {
      int count = 0;
      AudioUtils::CheckError(command_replay_->getCommandCount(&count));
      return count;
    }

    //------------------------------------------------------------------------------
    AudioCommandInfo AudioCommandReplay::GetCommandInfo(int id)
    {
      FMOD_STUDIO_COMMAND_INFO fmod_info;
      AudioUtils::CheckError(command_replay_->getCommandInfo(id, &fmod_info));

      AudioCommandInfo info;
      info.commandname = fmod_info.commandname;
      info.framenumber = fmod_info.framenumber;
      info.frametime = fmod_info.frametime;
      info.instancehandle = fmod_info.instancehandle;
      info.instancetype = static_cast<AudioInstanceType>(fmod_info.instancetype);
      info.outputhandle = fmod_info.outputhandle;
      info.outputtype = static_cast<AudioInstanceType>(fmod_info.instancetype);
      info.parentcommandindex = fmod_info.parentcommandindex;

      return info;
    }

    //------------------------------------------------------------------------------
    foundation::String AudioCommandReplay::GetCommandString(int id)
    {
      char command_string[512];
      AudioUtils::CheckError(command_replay_->getCommandString(id, command_string, 512));
      return foundation::String(command_string);
    }

    //------------------------------------------------------------------------------
    int AudioCommandReplay::GetCurrentCommand()
    {
      int id = 0;
      float time = 0;
      AudioUtils::CheckError(command_replay_->getCurrentCommand(&id, &time));
      return id;
    }

    //------------------------------------------------------------------------------
    float AudioCommandReplay::GetLength()
    {
      float length = 0;
      AudioUtils::CheckError(command_replay_->getLength(&length));
      return length;
    }

    //------------------------------------------------------------------------------
    bool AudioCommandReplay::GetPaused()
    {
      bool paused = false;
      AudioUtils::CheckError(command_replay_->getPaused(&paused));
      return paused;
    }

    //------------------------------------------------------------------------------
    PlaybackState AudioCommandReplay::GetPlaybackState()
    {
      FMOD_STUDIO_PLAYBACK_STATE state = FMOD_STUDIO_PLAYBACK_STOPPED;
      AudioUtils::CheckError(command_replay_->getPlaybackState(&state));
      return static_cast<PlaybackState>(state);
    }

    //------------------------------------------------------------------------------
    void AudioCommandReplay::SeekToCommand(int id)
    {
      AudioUtils::CheckError(command_replay_->seekToCommand(id));
    }

    //------------------------------------------------------------------------------
    void AudioCommandReplay::SeekToTime(float time)
    {
      AudioUtils::CheckError(command_replay_->seekToTime(time));
    }

    //------------------------------------------------------------------------------
    void AudioCommandReplay::SetBankPath(const char* path)
    {
      AudioUtils::CheckError(command_replay_->setBankPath(path));
    }

    //------------------------------------------------------------------------------
    void AudioCommandReplay::SetPaused(bool paused)
    {
      AudioUtils::CheckError(command_replay_->setPaused(paused));
    }
  }
}
