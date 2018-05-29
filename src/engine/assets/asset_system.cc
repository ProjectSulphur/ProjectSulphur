#include "asset_system.h"

#include "engine/application/application.h"
#include <foundation/io/filesystem.h>

namespace sulphur
{
  namespace engine
  {
    AssetSystem* AssetSystem::instance_ = nullptr;

    //--------------------------------------------------------------------------------
    AssetSystem::AssetSystem() :
      IServiceSystem("AssetSystem"),
      asset_managers_(static_cast<int>(AssetType::kNumAssetTypes), nullptr)
    {
    }

    //--------------------------------------------------------------------------------
    void AssetSystem::OnInitialize(Application& app, foundation::JobGraph&)
    {
      // Add managers to array
      asset_managers_[static_cast<int>(AssetType::kModel)] = &model_manager_;
      asset_managers_[static_cast<int>(AssetType::kMesh)] = &mesh_manager_;
      asset_managers_[static_cast<int>(AssetType::kShader)] = &shader_manager_;
      asset_managers_[static_cast<int>(AssetType::kComputeShader)] = &compute_shader_manager_;
      asset_managers_[static_cast<int>(AssetType::kShaderProgram)] = &shader_program_manager_;
      asset_managers_[static_cast<int>(AssetType::kMaterial)] = &material_manager_;
      asset_managers_[static_cast<int>(AssetType::kPostProcessMaterial)] = 
        &post_process_material_manager_;
      asset_managers_[static_cast<int>(AssetType::kTexture)] = &texture_manager_;
      asset_managers_[static_cast<int>(AssetType::kSkeleton)] = &skeleton_manager_;
      asset_managers_[static_cast<int>(AssetType::kAnimation)] = &animation_manager_;
      asset_managers_[static_cast<int>(AssetType::kScript)] = &script_manager_;
      asset_managers_[static_cast<int>(AssetType::kAudio)] = &audio_manager_;

      // Initialize subsystems
      for(size_t i = 0; i < asset_managers_.size(); ++i)
      {
        if(asset_managers_[i] != nullptr)
        {
          asset_managers_[i]->Initialize(app);
        }
      }

      // Save a static instance here to backwards-support the old convention
      instance_ = this;
    }

    //--------------------------------------------------------------------------------
    void AssetSystem::OnTerminate()
    {
      ReleaseGPUHandles();
    }

    //--------------------------------------------------------------------------------
    void AssetSystem::OnShutdown()
    {
      for (size_t i = 0; i < asset_managers_.size(); ++i)
      {
        if (asset_managers_[i] != nullptr)
        {
          asset_managers_[i]->Shutdown();
        }
      }
    }

    //--------------------------------------------------------------------------------
    AssetSystem& AssetSystem::Instance()
    {
      PS_LOG_IF(instance_ == nullptr, Fatal, "Attempt to access asset system before it was initialized");
      return *instance_;
    }

    //--------------------------------------------------------------------------------
    void AssetSystem::ReleaseGPUHandles()
    {
      for (size_t i = 0; i < asset_managers_.size(); ++i)
      {
        if (asset_managers_[i] != nullptr)
        {
          asset_managers_[i]->ReleaseGPUHandles();
        }
      }
    }

    //--------------------------------------------------------------------------------
    AssetHandle<void> AssetSystem::AddAsset(AssetType asset_type, void* asset,
      const foundation::AssetName& name)
    {
      switch (asset_type)
      {
      case AssetType::kModel:
        return model_manager_.Add(static_cast<Model*>(asset), name);
      case AssetType::kMesh:
        return mesh_manager_.Add(static_cast<Mesh*>(asset), name);
      case AssetType::kShader:
        return shader_manager_.Add(static_cast<Shader*>(asset), name);
      case AssetType::kComputeShader:
        return compute_shader_manager_.Add(static_cast<ComputeShader*>(asset), name);
      case AssetType::kShaderProgram:
        return shader_program_manager_.Add(static_cast<ShaderProgram*>(asset), name);
      case AssetType::kMaterial:
        return material_manager_.Add(static_cast<Material*>(asset), name);
      case AssetType::kPostProcessMaterial:
        return post_process_material_manager_.Add(static_cast<PostProcessMaterial*>(asset),
          name);
      case AssetType::kTexture:
        return texture_manager_.Add(static_cast<Texture*>(asset), name);
      case AssetType::kSkeleton:
        return skeleton_manager_.Add(static_cast<Skeleton*>(asset), name);
      case AssetType::kAnimation:
        return animation_manager_.Add(static_cast<Animation*>(asset), name);
      case AssetType::kScript:
        return script_manager_.Add(static_cast<Script*>(asset), name);
      case AssetType::kAudio:
        return audio_manager_.Add(static_cast<AudioBankData*>(asset), name);
      default:
        return AssetHandle<void>();
      }
    }

    //--------------------------------------------------------------------------------
    AssetHandle<void> AssetSystem::GetHandle(AssetType asset_type, 
      foundation::AssetID id)
    {
      return AssetHandle<void>(asset_managers_[static_cast<int>(asset_type)], 
        asset_managers_[static_cast<int>(asset_type)]->GetHandle(id));
    }

    //--------------------------------------------------------------------------------
    AssetHandle<void> AssetSystem::GetHandle(AssetType asset_type,
      const foundation::AssetName& name)
    {
      return AssetHandle<void>(asset_managers_[static_cast<int>(asset_type)],
        asset_managers_[static_cast<int>(asset_type)]->GetHandle(name));
    }

    //--------------------------------------------------------------------------------
    AssetHandle<void> AssetSystem::Load(AssetType asset_type, 
      foundation::AssetID id)
    {

      return AssetHandle<void>(asset_managers_[static_cast<int>(asset_type)],
        asset_managers_[static_cast<int>(asset_type)]->Load(id));
    }

    //--------------------------------------------------------------------------------
    AssetHandle<void> AssetSystem::Load(AssetType asset_type,
      const foundation::AssetName& name)
    {
      return AssetHandle<void>(asset_managers_[static_cast<int>(asset_type)],
        asset_managers_[static_cast<int>(asset_type)]->Load(name));
    }

    //--------------------------------------------------------------------------------
    void AssetSystem::Release(AssetType asset_type, foundation::AssetID id)
    {
      asset_managers_[static_cast<int>(asset_type)]->Release(id);
    }

    //--------------------------------------------------------------------------------
    void AssetSystem::Release(AssetType asset_type, const foundation::AssetName& name)
    {
      asset_managers_[static_cast<int>(asset_type)]->Release(name);
    }

    //--------------------------------------------------------------------------------
    int AssetSystem::GetReferenceCount(AssetType asset_type, foundation::AssetID id)
    {
      return asset_managers_[static_cast<int>(asset_type)]->GetReferenceCount(id);
    }

    //--------------------------------------------------------------------------------
    int AssetSystem::GetReferenceCount(AssetType asset_type,
      const foundation::AssetName& name)
    {
      return asset_managers_[static_cast<int>(asset_type)]->GetReferenceCount(name);
    }

    //--------------------------------------------------------------------------------
    void AssetSystem::RefreshCache()
    {
      for (size_t i = 0; i < asset_managers_.size(); ++i)
      {
        if (asset_managers_[i] != nullptr)
        {
          asset_managers_[i]->RefreshCache();
        }
      }
    }
  }
}
