#pragma once
#include "asset_interfaces.h"
#include "base_asset_manager.h"
#include "compute_shader.h"

namespace sulphur 
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::ComputeShaderManager : sulphur::engine::BaseAssetManager <sulphur::engine::ComputeShader>
    * @brief Manages compute shaders and loads them from packages.
    * @author Timo van Hees
    */
    class ComputeShaderManager : public BaseAssetManager<ComputeShader>
    {
    protected:
      /**
      * @see sulphur::engine::BaseAssetManager::ImportAsset.
      */
      ComputeShader* ImportAsset(const foundation::Path& asset_file) override;
      /**
      * @see sulphur::engine::BaseAssetManager::GetCacheName.
      */
      const foundation::String GetCacheName() const override;
    };

    //--------------------------------------------------------------------------------
    inline const foundation::String ComputeShaderManager::GetCacheName() const
    {
      return "";
    }
  }
}
