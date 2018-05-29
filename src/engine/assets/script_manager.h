#pragma once

#include "engine/assets/base_asset_manager.h"
#include "engine/assets/script.h"

namespace sulphur 
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::ScriptManager : sulphur::engine::BaseAssetManager<sulphur::engine::Script>
    * @brief Manages scripts and loads them from packages.
    * @author Timo van Hees
    */
    class ScriptManager : public BaseAssetManager<Script>
    {
    protected:
      /**
      * @see sulphur::engine::BaseAssetManager::ImportAsset.
      */
      Script* ImportAsset(const foundation::Path& asset_file) override;

      /**
      * @see sulphur::engine::BaseAssetManager::GetCacheName.
      */
      const foundation::String GetCacheName() const override;
    };

    //--------------------------------------------------------------------------------
    inline const foundation::String ScriptManager::GetCacheName() const
    {
      return "script_package";
    }
  }
}
