#include "animation_manager.h"

#include <foundation/io/binary_reader.h>
#include <foundation/memory/memory.h>
#include <foundation/pipeline-assets/animation.h>
#include <foundation/io/filesystem.h>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    Animation* AnimationManager::ImportAsset(const foundation::Path& asset_file)
    {
      foundation::BinaryReader reader(
        application_->project_directory() + asset_file);
      if (reader.is_ok())
      {
        Animation* asset_animation = foundation::Memory::Construct<Animation>(
          reader.Read<foundation::AnimationData>()
        );

        return asset_animation;
      }

      return nullptr;
    }
  }
}
