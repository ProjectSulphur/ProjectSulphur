#pragma once
#include "engine/systems/service_system.h"
#include "engine/assets/asset_interfaces.h"
#include "engine/assets/mesh_manager.h"
#include "engine/assets/texture_manager.h"
#include "engine/assets/shader_manager.h"
#include "engine/assets/compute_shader_manager.h"
#include "engine/assets/material_manager.h"
#include "engine/assets/post_process_material_manager.h"
#include "engine/assets/model_manager.h"
#include "engine/assets/shader_program_manager.h"
#include "engine/assets/skeleton_manager.h"
#include "engine/assets/animation_manager.h"
#include "engine/assets/script_manager.h"
#include "engine/assets/audio_manager.h"

namespace sulphur 
{
  namespace engine 
  {
    class MessagingSystem;

    /**
     * @brief Enum for all possible asset types that can be used by the asset system.
     */
    enum struct AssetType : uint8_t
    {
      kInvalid,
      kModel,
      kMesh,
      kShader,
      kComputeShader,
      kShaderProgram,
      kMaterial,
      kPostProcessMaterial,
      kTexture,
      kSkeleton,
      kAnimation,
      kScript,
      kAudio,
      kNumAssetTypes
    };

    /**
     * @class sulphur::engine::AssetSystem :
     *        sulphur::engine::IServiceSystem <sulphur::engine::AssetSystem>,
     *        sulphur::engine::editor::IMessageListener
     * @brief Interface for interacting with the asset managers.
     * @author Timo van Hees
     */
    class AssetSystem : public IServiceSystem<AssetSystem>
    {
    public:
      /**
      * @brief Default constructor
      */
      AssetSystem();
      
      /**
      * @see sulphur::engine::IServiceSystem::OnInitialize
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;
      /**
      * @brief Releases all GPU handles
      * @see sulphur::engine::IServiceSystem::OnTerminate
      */
      void OnTerminate() override;
      /**
      * @brief Releases all assets
      * @see sulphur::engine::IServiceSystem::OnShutdown
      */
      void OnShutdown() override;

      /**
      * @brief Returns a global instance of the asset system
      * @remarks May only be safely called after the application succesfully initialized all systems
      */
      static AssetSystem& Instance();
      
      /**
      * @brief Releases all GPU handles and resets them to invalid without invalidating the CPU handles.
      */
      void ReleaseGPUHandles();
      /**
      * @brief Reloads the cache files
      */
      void RefreshCache();

      /**
       * @brief Adds an unmanaged asset to the manager.
       * @tparam T The type of the asset.
       * @param[in] asset (T*) Pointer to unmanaged asset of type T.
       * @param[in] name (const sulphur::foundation::AssetName&) The name of the asset.
       * @return (sulphur::engine::AssetHandle<T>) A handle to the managed asset.
       */
      template <class T>
      AssetHandle<T> AddAsset(T* asset, const foundation::AssetName& name);

      /**
      * @brief Adds an unmanaged asset to the manager.
      * @param[in] asset_type (sulphur::engine::AssetType) The type of the asset.
      * @param[in] asset (void*) Pointer to unmanaged asset.
      * @param[in] name (const sulphur::foundation::AssetName&) The name of the asset.
      * @return (sulphur::engine::AssetHandle<void>) A handle to the managed asset.
      */
      AssetHandle<void> AddAsset(AssetType asset_type, void* asset, 
        const foundation::AssetName& name);

      /**
       * @brief Get a handle to an asset by ID.
       * @tparam T The type asset.
       * @param[in] id (sulphur::foundation::AssetID) The ID of the asset.
       * @return (sulphur::engine::AssetHandle<T>) A handle to the asset of type T.
       */
      template <class T>
      AssetHandle<T> GetHandle(foundation::AssetID id);

      /**
      * @brief Get a handle to an asset by ID.
      * @param[in] asset_type (sulphur::engine::AssetType) The type of the asset.
      * @param[in] id (sulphur::foundation::AssetID) The ID of the asset.
      * @return (sulphur::engine::AssetHandle<void>) A handle to the asset of type void.
      * @remark Meant only to be used by the scripting system.
      */
      AssetHandle<void> GetHandle(AssetType asset_type, foundation::AssetID id);

      /**
       * @brief Get a handle to an asset by name.
       * @tparam T The type of the asset.
       * @param[in] name (const sulphur::foundation::AssetName&) The name of the asset.
       * @return (sulphur::engine::AssetHandle<T>) A handle to the asset of type T.
       */
      template <class T>
      AssetHandle<T> GetHandle(const foundation::AssetName& name);

      /**
      * @brief Get a handle to an asset by name.
      * @param[in] asset_type (sulphur::engine::AssetType) The type of the asset.
      * @param[in] name (const sulphur::foundation::AssetName&) The name of the asset.
      * @return (sulphur::engine::AssetHandle<void>) A handle to the asset of type void.
      * @remark Meant only to be used by the scripting system.
      */
      AssetHandle<void> GetHandle(AssetType asset_type, const foundation::AssetName& name);

      /**
       * @brief Loads an asset from a package created with the builder.
       * @tparam T The type of the asset.
       * @param[in] id (sulphur::foundation::AssetID) The ID of the asset.
       * @return (sulphur::engine::AssetHandle<T>) A handle to the asset of type T.
       */
      template <class T>
      AssetHandle<T> Load(foundation::AssetID id);

      /**
       * @brief Loads an asset from a package created with the builder.
       * @param[in] asset_type (sulphur::engine::AssetType) The type of the asset.
       * @param[in] id (sulphur::foundation::AssetID) The ID of the asset.
       * @return (sulphur::engine::AssetHandle<void>) A handle to the asset of type void.
       * @remark Meant only to be used by the scripting system.
       */
      AssetHandle<void> Load(AssetType asset_type, foundation::AssetID id);

      /**
      * @brief Loads an asset from a package created with the builder.
      * @tparam T The type of the asset.
      * @param[in] name (const sulphur::foundation::AssetName&) The ID of the asset.
      * @return (sulphur::engine::AssetHandle<T>) A handle to the asset of type T.
      */
      template <class T>
      AssetHandle<T> Load(const foundation::AssetName& name);

      /**
      * @brief Loads an asset from a package created with the builder.
      * @param[in] asset_type (sulphur::engine::AssetType) The type of the asset.
      * @param[in] name (const sulphur::foundation::AssetName&) The ID of the asset.
      * @return (sulphur::engine::AssetHandle<void>) A handle to the asset of type void.
      * @remark Meant only to be used by the scripting system.
      */
      AssetHandle<void> Load(AssetType asset_type, const foundation::AssetName& name);

      /**
       * @brief Release an asset instantly by ID without invalidating the handles.
       * @param[in] type (sulphur::engine::AssetType) The type of the asset to release.
       * @param[in] id (sulphur::foundation::AssetID) The ID of the asset.
       */
      void Release(AssetType type, foundation::AssetID id);
      /**
      * @brief Release an asset instantly by name without invalidating the handles.
      * @param[in] type (sulphur::engine::AssetType) The type of the asset to release.
      * @param[in] name (const sulphur::foundation::AssetName&) The name of the asset.
      */
      void Release(AssetType type, const foundation::AssetName& name);

      /**
       * @brief Get the number of references to an asset by ID.
       * @param[in] type (sulphur::engine::AssetType) The type of the asset to release.
       * @param[in] id (sulphur::foundation::AssetID) The ID of the asset.
       * @return (int) The number of references.
       */
      int GetReferenceCount(AssetType type, foundation::AssetID id);
      /**
      * @brief Get the number of references to an asset by name.
      * @param[in] type (sulphur::engine::AssetType) The type of the asset to release.
      * @param[in] name (const sulphur::foundation::AssetName&) The name of the asset.
      * @return (int) The number of references.
      */
      int GetReferenceCount(AssetType type, const foundation::AssetName& name);

    private:
      static AssetSystem* instance_; //!< A static instance for global access

      MessagingSystem* messaging_service_; //!< A pointer to the messaging system to listen for editor-messages

      ModelManager model_manager_;                                //!< Model manager used by the asset system.
      MeshManager mesh_manager_;                                  //!< Mesh manager used by the asset system.
      TextureManager texture_manager_;                            //!< Texture manager used by the asset system.
      ShaderManager shader_manager_;                              //!< Shader manager used by the asset system.
      ComputeShaderManager compute_shader_manager_;               //!< Compute shader manager used by the asset system.
      ShaderProgramManager shader_program_manager_;               //!< Shader program manager used by the asset system.
      MaterialManager material_manager_;                          //!< Material manager used by the asset system.
      PostProcessMaterialManager post_process_material_manager_;  //!< PostProcessMaterial manager used by the asset system.
      SkeletonManager skeleton_manager_;                          //!< Skeleton manager used by the asset system.
      AnimationManager animation_manager_;                        //!< Animation manager used by the asset system.
      ScriptManager script_manager_;                              //!< Script manager used by the asset system.
      AudioManager audio_manager_;                                //!< Audio manager used by the asset system.
      foundation::Vector<IAssetManager*> asset_managers_;         //!< All asset managers in array form.
    };

    //----------------------------------------------Model----------------------------------------------------
    /**
     * @see sulphur::engine::AssetSystem::AddAsset
     */
    template <>
    inline AssetHandle<Model> AssetSystem::AddAsset<Model>(
      Model* asset, const foundation::AssetName& name)
    {
      return model_manager_.Add(reinterpret_cast<Model*>(asset), name);
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<Model> AssetSystem::GetHandle<Model>(foundation::AssetID id)
    {
      return AssetHandle<Model>(&model_manager_, model_manager_.GetHandle(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<Model> AssetSystem::GetHandle<Model>(const foundation::AssetName& name)
    {
      return AssetHandle<Model>(&model_manager_, model_manager_.GetHandle(name));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<Model> AssetSystem::Load<Model>(foundation::AssetID id)
    {
      return AssetHandle<Model>(&model_manager_, model_manager_.Load(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<Model> AssetSystem::Load<Model>(const foundation::AssetName& name)
    {
      return AssetHandle<Model>(&model_manager_, model_manager_.Load(name));
    }


    //----------------------------------------------Mesh----------------------------------------------------
    /**
    * @see sulphur::engine::AssetSystem::AddAsset
    */
    template <>
    inline AssetHandle<Mesh> AssetSystem::AddAsset<Mesh>(
      Mesh* asset, const foundation::AssetName& name)
    {
      return mesh_manager_.Add(reinterpret_cast<Mesh*>(asset), name);
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<Mesh> AssetSystem::GetHandle<Mesh>(foundation::AssetID id)
    {
      return AssetHandle<Mesh>(&mesh_manager_, mesh_manager_.GetHandle(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<Mesh> AssetSystem::GetHandle<Mesh>(const foundation::AssetName& name)
    {
      return AssetHandle<Mesh>(&mesh_manager_, mesh_manager_.GetHandle(name));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<Mesh> AssetSystem::Load<Mesh>(foundation::AssetID id)
    {
      return AssetHandle<Mesh>(&mesh_manager_, mesh_manager_.Load(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<Mesh> AssetSystem::Load<Mesh>(const foundation::AssetName& name)
    {
      return AssetHandle<Mesh>(&mesh_manager_, mesh_manager_.Load(name));
    }

    //--------------------------------------------Texture---------------------------------------------------
    /**
    * @see sulphur::engine::AssetSystem::AddAsset
    */
    template <>
    inline AssetHandle<Texture> AssetSystem::AddAsset<Texture>(
      Texture* asset, const foundation::AssetName& name)
    {
      return texture_manager_.Add(reinterpret_cast<Texture*>(asset), name);
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<Texture> AssetSystem::GetHandle<Texture>(foundation::AssetID id)
    {
      return AssetHandle<Texture>(&texture_manager_, texture_manager_.GetHandle(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<Texture> AssetSystem::GetHandle<Texture>(const foundation::AssetName& name)
    {
      return AssetHandle<Texture>(&texture_manager_, texture_manager_.GetHandle(name));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<Texture> AssetSystem::Load<Texture>(foundation::AssetID id)
    {
      return AssetHandle<Texture>(&texture_manager_, texture_manager_.Load(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<Texture> AssetSystem::Load<Texture>(const foundation::AssetName& name)
    {
      return AssetHandle<Texture>(&texture_manager_, texture_manager_.Load(name));
    }

    //--------------------------------------------Shader---------------------------------------------------
    /**
    * @see sulphur::engine::AssetSystem::AddAsset
    */
    template <>
    inline AssetHandle<Shader> AssetSystem::AddAsset<Shader>(
      Shader* asset, const foundation::AssetName& name)
    {
      return shader_manager_.Add(reinterpret_cast<Shader*>(asset), name);
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<Shader> AssetSystem::GetHandle<Shader>(foundation::AssetID id)
    {
      return AssetHandle<Shader>(&shader_manager_, shader_manager_.GetHandle(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<Shader> AssetSystem::GetHandle<Shader>(const foundation::AssetName& name)
    {
      return AssetHandle<Shader>(&shader_manager_, shader_manager_.GetHandle(name));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<Shader> AssetSystem::Load<Shader>(foundation::AssetID id)
    {
      return AssetHandle<Shader>(&shader_manager_, shader_manager_.Load(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<Shader> AssetSystem::Load<Shader>(const foundation::AssetName& name)
    {
      return AssetHandle<Shader>(&shader_manager_, shader_manager_.Load(name));
    }

    //--------------------------------------------ComputeShader---------------------------------------------------
    /**
    * @see sulphur::engine::AssetSystem::AddAsset
    */
    template <>
    inline AssetHandle<ComputeShader> AssetSystem::AddAsset<ComputeShader>(
      ComputeShader* asset, const foundation::AssetName& name)
    {
      return compute_shader_manager_.Add(reinterpret_cast<ComputeShader*>(asset), name);
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<ComputeShader> AssetSystem::GetHandle<ComputeShader>(foundation::AssetID id)
    {
      return AssetHandle<ComputeShader>(&compute_shader_manager_, compute_shader_manager_.GetHandle(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<ComputeShader> AssetSystem::GetHandle<ComputeShader>(const foundation::AssetName& name)
    {
      return AssetHandle<ComputeShader>(&compute_shader_manager_, compute_shader_manager_.GetHandle(name));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<ComputeShader> AssetSystem::Load<ComputeShader>(foundation::AssetID id)
    {
      return AssetHandle<ComputeShader>(&compute_shader_manager_, compute_shader_manager_.Load(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<ComputeShader> AssetSystem::Load<ComputeShader>(const foundation::AssetName& name)
    {
      return AssetHandle<ComputeShader>(&compute_shader_manager_, compute_shader_manager_.Load(name));
    }

    //-----------------------------------------Shader Program------------------------------------------------
    /**
    * @see sulphur::engine::AssetSystem::AddAsset
    */
    template <>
    inline AssetHandle<ShaderProgram> AssetSystem::AddAsset<ShaderProgram>(
      ShaderProgram* asset, const foundation::AssetName& name)
    {
      return shader_program_manager_.Add(reinterpret_cast<ShaderProgram*>(asset), name);
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<ShaderProgram> AssetSystem::GetHandle<ShaderProgram>(foundation::AssetID id)
    {
      return AssetHandle<ShaderProgram>(&shader_program_manager_, shader_program_manager_.GetHandle(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<ShaderProgram> AssetSystem::GetHandle<ShaderProgram>(const foundation::AssetName& name)
    {
      return AssetHandle<ShaderProgram>(&shader_program_manager_, shader_program_manager_.GetHandle(name));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<ShaderProgram> AssetSystem::Load<ShaderProgram>(foundation::AssetID id)
    {
      return AssetHandle<ShaderProgram>(&shader_program_manager_, shader_program_manager_.Load(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<ShaderProgram> AssetSystem::Load<ShaderProgram>(const foundation::AssetName& name)
    {
      return AssetHandle<ShaderProgram>(&shader_program_manager_, shader_program_manager_.Load(name));
    }

    //-------------------------------------------Material---------------------------------------------------
    /**
    * @see sulphur::engine::AssetSystem::AddAsset
    */
    template <>
    inline AssetHandle<Material> AssetSystem::AddAsset<Material>(
      Material* asset, const foundation::AssetName& name)
    {
      return material_manager_.Add(reinterpret_cast<Material*>(asset), name);
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<Material> AssetSystem::GetHandle<Material>(foundation::AssetID id)
    {
      return AssetHandle<Material>(&material_manager_, material_manager_.GetHandle(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<Material> AssetSystem::GetHandle<Material>(const foundation::AssetName& name)
    {
      return AssetHandle<Material>(&material_manager_, material_manager_.GetHandle(name));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<Material> AssetSystem::Load<Material>(foundation::AssetID id)
    {
      return AssetHandle<Material>(&material_manager_, material_manager_.Load(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<Material> AssetSystem::Load<Material>(const foundation::AssetName& name)
    {
      return AssetHandle<Material>(&material_manager_, material_manager_.Load(name));
    }

    //-------------------------------------------PostProcessMaterial---------------------------------------------------
    /**
    * @see sulphur::engine::AssetSystem::AddAsset
    */
    template <>
    inline AssetHandle<PostProcessMaterial> AssetSystem::AddAsset<PostProcessMaterial>(
      PostProcessMaterial* asset, const foundation::AssetName& name)
    {
      return post_process_material_manager_.Add(reinterpret_cast<PostProcessMaterial*>(asset), name);
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<PostProcessMaterial> AssetSystem::GetHandle<PostProcessMaterial>(foundation::AssetID id)
    {
      return AssetHandle<PostProcessMaterial>(&post_process_material_manager_, post_process_material_manager_.GetHandle(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<PostProcessMaterial> AssetSystem::GetHandle<PostProcessMaterial>(const foundation::AssetName& name)
    {
      return AssetHandle<PostProcessMaterial>(&post_process_material_manager_, post_process_material_manager_.GetHandle(name));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<PostProcessMaterial> AssetSystem::Load<PostProcessMaterial>(foundation::AssetID id)
    {
      return AssetHandle<PostProcessMaterial>(&post_process_material_manager_, post_process_material_manager_.Load(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<PostProcessMaterial> AssetSystem::Load<PostProcessMaterial>(const foundation::AssetName& name)
    {
      return AssetHandle<PostProcessMaterial>(&post_process_material_manager_, post_process_material_manager_.Load(name));
    }

    //--------------------------------------------Skeleton---------------------------------------------------
    /**
    * @see sulphur::engine::AssetSystem::AddAsset
    */
    template <>
    inline AssetHandle<Skeleton> AssetSystem::AddAsset<Skeleton>(
      Skeleton* asset, 
      const foundation::AssetName& name
    )
    {
      return skeleton_manager_.Add(reinterpret_cast<Skeleton*>(asset), name);
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<Skeleton> AssetSystem::GetHandle<Skeleton>(foundation::AssetID id)
    {
      return AssetHandle<Skeleton>(&skeleton_manager_, skeleton_manager_.GetHandle(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<Skeleton> AssetSystem::GetHandle<Skeleton>(const foundation::AssetName& name)
    {
      return AssetHandle<Skeleton>(&skeleton_manager_, skeleton_manager_.GetHandle(name));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<Skeleton> AssetSystem::Load<Skeleton>(foundation::AssetID id)
    {
      return AssetHandle<Skeleton>(&skeleton_manager_, skeleton_manager_.Load(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<Skeleton> AssetSystem::Load<Skeleton>(const foundation::AssetName& name)
    {
      return AssetHandle<Skeleton>(&skeleton_manager_, skeleton_manager_.Load(name));
    }

    //--------------------------------------------Animation---------------------------------------------------
    /**
    * @see sulphur::engine::AssetSystem::AddAsset
    */
    template <>
    inline AssetHandle<Animation> AssetSystem::AddAsset<Animation>(
      Animation* asset, const foundation::AssetName& name)
    {
      return animation_manager_.Add(reinterpret_cast<Animation*>(asset), name);
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<Animation> AssetSystem::GetHandle<Animation>(foundation::AssetID id)
    {
      return AssetHandle<Animation>(&animation_manager_, animation_manager_.GetHandle(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<Animation> AssetSystem::GetHandle<Animation>(const foundation::AssetName& name)
    {
      return AssetHandle<Animation>(&animation_manager_, animation_manager_.GetHandle(name));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<Animation> AssetSystem::Load<Animation>(foundation::AssetID id)
    {
      return AssetHandle<Animation>(&animation_manager_, animation_manager_.Load(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<Animation> AssetSystem::Load<Animation>(const foundation::AssetName& name)
    {
      return AssetHandle<Animation>(&animation_manager_, animation_manager_.Load(name));
    }

    //--------------------------------------------Script---------------------------------------------------
    /**
    * @see sulphur::engine::AssetSystem::AddAsset
    */
    template <>
    inline AssetHandle<Script> AssetSystem::AddAsset<Script>(
      Script* asset, const foundation::AssetName& name)
    {
      return script_manager_.Add(reinterpret_cast<Script*>(asset), name);
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<Script> AssetSystem::GetHandle<Script>(foundation::AssetID id)
    {
      return AssetHandle<Script>(&script_manager_, script_manager_.GetHandle(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<Script> AssetSystem::GetHandle<Script>(const foundation::AssetName& name)
    {
      return AssetHandle<Script>(&script_manager_, script_manager_.GetHandle(name));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<Script> AssetSystem::Load<Script>(foundation::AssetID id)
    {
      return AssetHandle<Script>(&script_manager_, script_manager_.Load(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<Script> AssetSystem::Load<Script>(const foundation::AssetName& name)
    {
      return AssetHandle<Script>(&script_manager_, script_manager_.Load(name));
    }

    //--------------------------------------------Audio---------------------------------------------------
    /**
    * @see sulphur::engine::AssetSystem::AddAsset
    */
    template <>
    inline AssetHandle<AudioBankData> AssetSystem::AddAsset<AudioBankData>(
      AudioBankData* asset, const foundation::AssetName& name)
    {
      return audio_manager_.Add(reinterpret_cast<AudioBankData*>(asset), name);
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<AudioBankData> AssetSystem::GetHandle<AudioBankData>(foundation::AssetID id)
    {
      return AssetHandle<AudioBankData>(&audio_manager_, audio_manager_.GetHandle(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::GetHandle
    */
    template <>
    inline AssetHandle<AudioBankData> AssetSystem::GetHandle<AudioBankData>(const foundation::AssetName& name)
    {
      return AssetHandle<AudioBankData>(&audio_manager_, audio_manager_.GetHandle(name));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<AudioBankData> AssetSystem::Load<AudioBankData>(foundation::AssetID id)
    {
      return AssetHandle<AudioBankData>(&audio_manager_, audio_manager_.Load(id));
    }

    /**
    * @see sulphur::engine::AssetSystem::Load
    */
    template <>
    inline AssetHandle<AudioBankData> AssetSystem::Load<AudioBankData>(const foundation::AssetName& name)
    {
      return AssetHandle<AudioBankData>(&audio_manager_, audio_manager_.Load(name));
    }
  }
}
