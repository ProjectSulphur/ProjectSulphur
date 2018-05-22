#include "model.h"

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    Model::Model(const MeshHandle& mesh_handle,
      const foundation::Vector<MaterialHandle>& material_handles) :
    mesh_handle_(mesh_handle),
    material_handles_(material_handles)
    {}

    //--------------------------------------------------------------------------------
    const MeshHandle& Model::mesh_handle() const
    {
      return mesh_handle_;
    }

    //--------------------------------------------------------------------------------
    const foundation::Vector<MaterialHandle>& Model::material_handles() const
    {
      return material_handles_;
    }

    //--------------------------------------------------------------------------------
    const MaterialHandle& Model::GetMaterialHandle(int i) const
    {
      return material_handles_[i];
    }

    //--------------------------------------------------------------------------------
    size_t Model::GetMaterialHandlesSize() const
    {
      return material_handles_.size();
    }
  }
}