#pragma once
#include "asset_interfaces.h"
#include "base_asset_manager.h"
#include "shader.h"

namespace sulphur 
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::ShaderProgramManager : sulphur::engine::BaseAssetManager <sulphur::engine::ShaderProgram>
    * @brief Manages shader programs and loads them from packages.
    * @author Timo van Hees
    */
    class ShaderProgramManager : public BaseAssetManager<ShaderProgram>
    {
    protected:
      /**
      * @see sulphur::engine::BaseAssetManager::ImportAsset.
      */
      ShaderProgram* ImportAsset(const foundation::String& asset_file) override;
      /**
      * @see sulphur::engine::BaseAssetManager::GetCacheName.
      */
      const foundation::String GetCacheName() const override;
    };

    //--------------------------------------------------------------------------------
    inline const foundation::String ShaderProgramManager::GetCacheName() const
    {
      return "shader_package";
    }
  }
}
