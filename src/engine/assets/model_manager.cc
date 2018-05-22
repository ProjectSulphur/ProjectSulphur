#include "model_manager.h"
#include "asset_system.h"

#include <foundation/io/binary_reader.h>
#include <foundation/containers/vector.h>
#include <foundation/memory/memory.h>

#include <foundation/pipeline-assets/model.h>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    Model* ModelManager::ImportAsset(const foundation::String& asset_file)
    {
      foundation::BinaryReader reader(asset_file);
      if (reader.is_ok())
      {
        AssetSystem& asset_system = AssetSystem::Instance();

        foundation::ModelData asset_model = reader.Read<foundation::ModelData>();

        AssetHandle<Mesh> mesh = asset_system.Load<Mesh>(asset_model.mesh.id);

        foundation::Vector<AssetHandle<Material>> materials(asset_model.materials.size());
        for (int i = 0; i < asset_model.materials.size(); ++i)
        {
          materials[i] = asset_system.Load<Material>(asset_model.materials[i].id);
        }

        Model* model = foundation::Memory::Construct<Model>(mesh, materials);

        return model;
      }

      return nullptr;
    }
  }
}
