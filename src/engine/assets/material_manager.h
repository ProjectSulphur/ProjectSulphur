#pragma once
#include "asset_interfaces.h"
#include "base_asset_manager.h"
#include "material.h"

namespace sulphur 
{
  namespace engine 
  {
    /**
     * @class sulphur::engine::MaterialManager : sulphur::engine::BaseAssetManager<sulphur::engine::Material>
     * @brief Manages materials and loads them from packages.
     * @author Timo van Hees
     */
    class MaterialManager : public BaseAssetManager<Material>
    {
    protected:
      /**
       * @see sulphur::engine::BaseAssetManager::ImportAsset.
       */
      Material* ImportAsset(const foundation::String& asset_file) override;
      /**
      * @see sulphur::engine::BaseAssetManager::GetCacheName.
      */
      const foundation::String GetCacheName() const override;
    };

    inline const foundation::String MaterialManager::GetCacheName() const
    {
      return "material_package";
    }
  }
}
