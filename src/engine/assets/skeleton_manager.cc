#include "skeleton_manager.h"

#include <foundation/io/binary_reader.h>
#include <foundation/memory/memory.h>
#include <foundation/pipeline-assets/skeleton.h>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    Skeleton* SkeletonManager::ImportAsset(const foundation::Path& asset_file)
    {
      foundation::BinaryReader reader(
        foundation::Path(application_->project_directory()) + asset_file);

      if (reader.is_ok())
      {
        foundation::SkeletonData asset_skeleton = reader.Read<foundation::SkeletonData>();

        Skeleton* skeleton = foundation::Memory::Construct<Skeleton>(asset_skeleton);

        return skeleton;
      }

      return nullptr;
    }
  }
}
