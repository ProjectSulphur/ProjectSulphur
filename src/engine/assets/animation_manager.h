#pragma once

#include "engine/assets/base_asset_manager.h"
#include "engine/assets/animation.h"

#include <foundation/pipeline-assets/animation.h>

namespace sulphur 
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::AnimationManager : sulphur::engine::BaseAssetManager<sulphur::engine::Animation>
    * @brief Manages animations and loads them from packages.
    * @author Riko Ophorst
    */
    class AnimationManager : public BaseAssetManager<Animation>
    {
    protected:
      /**
      * @see sulphur::engine::BaseAssetManager::ImportAsset.
      */
      Animation* ImportAsset(const foundation::Path& asset_file) override;
      /**
      * @see sulphur::engine::BaseAssetManager::GetCacheName.
      */
      const foundation::String GetCacheName() const override;
    };

    //--------------------------------------------------------------------------------
    inline const foundation::String AnimationManager::GetCacheName() const
    {
      return "animation_package";
    }
  }
}
