#pragma once
#include "asset_interfaces.h"
#include "base_asset_manager.h"
#include "shader.h"

namespace sulphur 
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::ShaderManager : sulphur::engine::BaseAssetManager <sulphur::engine::Shader>
    * @brief Manages shaders and loads them from packages.
    * @author Timo van Hees
    */
    class ShaderManager : public BaseAssetManager<Shader>
    {
    protected:
      /**
      * @see sulphur::engine::BaseAssetManager::ImportAsset.
      */
      Shader* ImportAsset(const foundation::Path& asset_file) override;
      /**
      * @see sulphur::engine::BaseAssetManager::GetCacheName.
      */
      const foundation::String GetCacheName() const override;
    };

    //--------------------------------------------------------------------------------
    inline const foundation::String ShaderManager::GetCacheName() const
    {
      return "";
    }
  }
}
