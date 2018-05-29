#pragma once
#include "asset_interfaces.h"
#include "base_asset_manager.h"
#include "model.h"

namespace sulphur 
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::ModelManager : sulphur::engine::BaseAssetManager<sulphur::engine::Model>
    * @brief Manages models and loads them from packages.
    * @author Timo van Hees
    */
    class ModelManager : public BaseAssetManager<Model>
    {
    protected:
      /**
      * @see sulphur::engine::BaseAssetManager::ImportAsset.
      */
      Model* ImportAsset(const foundation::Path& asset_file) override;
      /**
      * @see sulphur::engine::BaseAssetManager::GetCacheName.
      */
      const foundation::String GetCacheName() const override;
    };

    //--------------------------------------------------------------------------------
    inline const foundation::String ModelManager::GetCacheName() const
    {
      return "model_package";
    }
  }
}
