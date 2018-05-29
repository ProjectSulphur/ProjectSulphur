#pragma once
#include "asset_interfaces.h"
#include "base_asset_manager.h"
#include "post_process_material.h"

namespace sulphur 
{
  namespace engine 
  {
    /**
     * @class sulphur::engine::PostProcessMaterialManager : sulphur::engine::BaseAssetManager<sulphur::engine::PostProcessMaterial>
     * @brief Manages materials and loads them from packages.
     * @author Timo van Hees
     */
    class PostProcessMaterialManager : public BaseAssetManager<PostProcessMaterial>
    {
    protected:
      /**
       * @see sulphur::engine::BaseAssetManager::ImportAsset.
       */
      PostProcessMaterial* ImportAsset(const foundation::Path& asset_file) override;
      /**
      * @see sulphur::engine::BaseAssetManager::GetCacheName.
      */
      const foundation::String GetCacheName() const override;
    };

    inline const foundation::String PostProcessMaterialManager::GetCacheName() const
    {
      return ""; // Doesn't have one yet
    }
  }
}
