#include "asset_system.h"

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    AssetSystem::AssetSystem() : initialized_(false)
    {}

    //--------------------------------------------------------------------------------
    AssetSystem& AssetSystem::Instance()
    {
      static AssetSystem instance;
      if(instance.initialized_ == false)
      {
        instance.Initialize();
      }
      return instance;
    }

    //--------------------------------------------------------------------------------
    void AssetSystem::Shutdown()
    {
      AssetSystem& as = AssetSystem::Instance();

      as.model_manager_.Shutdown();
      as.mesh_manager_.Shutdown();
      as.shader_manager_.Shutdown();
      as.shader_program_manager_.Shutdown();
      as.material_manager_.Shutdown();
      as.texture_manager_.Shutdown();

      as.initialized_ = false;
    }

    //--------------------------------------------------------------------------------
    void AssetSystem::ReleaseGPUHandles()
    {
      AssetSystem& as = AssetSystem::Instance();

      as.model_manager_.ReleaseGPUHandles();
      as.mesh_manager_.ReleaseGPUHandles();
      as.shader_manager_.ReleaseGPUHandles();
      as.shader_program_manager_.ReleaseGPUHandles();
      as.material_manager_.ReleaseGPUHandles();
      as.texture_manager_.ReleaseGPUHandles();
    }

    //--------------------------------------------------------------------------------
    void AssetSystem::Release(AssetType type, foundation::AssetID id)
    {
      switch (type)
      {
      case AssetType::kModel:
        model_manager_.Release(id);
        break;
      case AssetType::kMesh:
        mesh_manager_.Release(id);
        break;
      case AssetType::kShader:
        shader_manager_.Release(id);
        break;
      case AssetType::kShaderProgram:
        shader_program_manager_.Release(id);
        break;
      case AssetType::kMaterial:
        material_manager_.Release(id); 
        break;
      case AssetType::kTexture:
        texture_manager_.Release(id); 
        break;
      default:
        break;
      }
    }

    //--------------------------------------------------------------------------------
    void AssetSystem::Release(AssetType type, const foundation::AssetName& name)
    {
      switch (type)
      {
      case AssetType::kModel:
        model_manager_.Release(name);
        break;
      case AssetType::kMesh:
        mesh_manager_.Release(name);
        break;
      case AssetType::kShader:
        shader_manager_.Release(name);
        break;
      case AssetType::kShaderProgram:
        shader_program_manager_.Release(name);
        break;
      case AssetType::kMaterial:
        material_manager_.Release(name);
        break;
      case AssetType::kTexture:
        texture_manager_.Release(name);
        break;
      default:
        break;
      }
    }

    //--------------------------------------------------------------------------------
    int AssetSystem::GetReferenceCount(AssetType type, foundation::AssetID id)
    {
      switch (type)
      {
      case AssetType::kModel:
        return model_manager_.GetReferenceCount(id);
      case AssetType::kMesh:
        return mesh_manager_.GetReferenceCount(id);
      case AssetType::kShader:
        return shader_manager_.GetReferenceCount(id);
      case AssetType::kShaderProgram:
        return shader_program_manager_.GetReferenceCount(id);
      case AssetType::kMaterial:
        return material_manager_.GetReferenceCount(id);
      case AssetType::kTexture:
        return texture_manager_.GetReferenceCount(id);
      default:
        return 0;
      }
    }

    //--------------------------------------------------------------------------------
    int AssetSystem::GetReferenceCount(AssetType type,
                                       const foundation::AssetName& name)
    {
      switch (type)
      {
      case AssetType::kModel:
        return model_manager_.GetReferenceCount(name);
      case AssetType::kMesh:
        return mesh_manager_.GetReferenceCount(name);
      case AssetType::kShader:
        return shader_manager_.GetReferenceCount(name);
      case AssetType::kShaderProgram:
        return shader_program_manager_.GetReferenceCount(name);
      case AssetType::kMaterial:
        return material_manager_.GetReferenceCount(name);
      case AssetType::kTexture:
        return texture_manager_.GetReferenceCount(name);
      default:
        return 0;
      }
    }

    //--------------------------------------------------------------------------------
    void AssetSystem::Initialize()
    {
      model_manager_.Initialize();
      mesh_manager_.Initialize();
      shader_manager_.Initialize();
      shader_program_manager_.Initialize();
      material_manager_.Initialize();
      texture_manager_.Initialize();
      initialized_ = true;
    }
  }
}
