#pragma once
#include "base_asset_manager.h"
#include "mesh.h"

namespace sulphur 
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::MeshManager : sulphur::engine::BaseAssetManager<sulphur::engine::Mesh>
    * @brief Manages meshes and loads them from packages.
    * @author Timo van Hees
    */
    class MeshManager : public BaseAssetManager<Mesh>
    {
    protected:
      /**
      * @see sulphur::engine::BaseAssetManager::ImportAsset.
      */
      Mesh* ImportAsset(const foundation::Path& asset_file) override;
      /**
      * @see sulphur::engine::BaseAssetManager::GetCacheName.
      */
      const foundation::String GetCacheName() const override;
    };

    //--------------------------------------------------------------------------------
    inline const foundation::String MeshManager::GetCacheName() const
    {
      return "mesh_package";
    }
  }
}
