#pragma once

#include "engine/assets/mesh.h"
#include "engine/assets/material.h"

namespace sulphur
{
  namespace engine
  {
    class Model
    {
    public:
      Model(const MeshHandle& mesh_handle, 
        const foundation::Vector<MaterialHandle>& material_handles);
      ~Model() = default;

      const MeshHandle& mesh_handle() const;
      const foundation::Vector<MaterialHandle>& material_handles() const;
      const MaterialHandle& GetMaterialHandle(int i) const;
      size_t GetMaterialHandlesSize() const;

    private:
      MeshHandle mesh_handle_;
      foundation::Vector<MaterialHandle> material_handles_;
    };

    using ModelHandle = AssetHandle<Model>; //!< Asset handle to a mesh
  }
}
