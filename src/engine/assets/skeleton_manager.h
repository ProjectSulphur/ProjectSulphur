#pragma once

#include "engine/assets/base_asset_manager.h"
#include "engine/assets/skeleton.h"

namespace sulphur 
{
  namespace engine 
  {
    class Skeleton;

    /**
    * @class sulphur::engine::SkeletonManager : sulphur::engine::BaseAssetManager<sulphur::foundation::SkeletonData>
    * @brief Manages skeletons and loads them from packages.
    * @author Timo van Hees
    */
    class SkeletonManager : public BaseAssetManager<Skeleton>
    {
    protected:
      /**
      * @see sulphur::engine::BaseAssetManager::ImportAsset.
      */
      Skeleton* ImportAsset(const foundation::Path& asset_file) override;

      /**
      * @see sulphur::engine::BaseAssetManager::GetCacheName.
      */
      const foundation::String GetCacheName() const override;
    };

    //--------------------------------------------------------------------------------
    inline const foundation::String SkeletonManager::GetCacheName() const
    {
      return "skeleton_package";
    }
  }
}
