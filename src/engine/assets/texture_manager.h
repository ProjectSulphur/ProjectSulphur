#pragma once
#include "asset_interfaces.h"
#include "base_asset_manager.h"
#include "texture.h"

namespace sulphur 
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::TextureManager : sulphur::engine::BaseAssetManager<sulphur::engine::Texture>
    * @brief Manages textures and loads them from packages.
    * @author Timo van Hees
    */
    class TextureManager : public BaseAssetManager<Texture>
    {
    protected:
      /**
      * @see sulphur::engine::BaseAssetManager::ImportAsset.
      */
      Texture* ImportAsset(const foundation::String& asset_file) override;
      /**
      * @see sulphur::engine::BaseAssetManager::GetCacheName.
      */
      const foundation::String GetCacheName() const override;
    };

    //--------------------------------------------------------------------------------
    inline const foundation::String TextureManager::GetCacheName() const
    {
      return "texture_package";
    }
  }
}
