#include "engine/assets/audio_manager.h"
#include "engine/audio/audio_bank.h"

#include <foundation/io/binary_reader.h>
#include <foundation/memory/memory.h>
#include <foundation/pipeline-assets/audio.h>
#include <foundation/io/filesystem.h>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    AudioBankData* AudioManager::ImportAsset(const foundation::Path& asset_file)
    {
      foundation::BinaryReader reader(
        foundation::Path(application_->project_directory()) + asset_file);

      if (reader.is_ok())
      {
        AudioBankData* audio_bank = foundation::Memory::Construct<AudioBankData>();
        *audio_bank = reader.Read<foundation::AudioBankData>();

        return audio_bank;
      }

      return nullptr;
    }
  }
}
