#pragma once
#include "asset_interfaces.h"
#include "mesh_manager.h"
#include "texture_manager.h"
#include "shader_manager.h"
#include "material_manager.h"
#include "model_manager.h"
#include "shader_program_manager.h"

namespace sulphur 
{
  namespace engine 
  {
    /**
     * @brief Enum for all possible asset types that can be used by the asset system.
     */
    enum struct AssetType : uint8_t
    {
      kInvalid,
      kModel,
      kMesh,
      kShader,
      kShaderProgram,
      kMaterial,
      kTexture
    };

    /**
     * @class sulphur::engine::AssetSystem
     * @brief Interface for interacting with the asset managers.
     * @author Timo van Hees
     */
    class AssetSystem
    {
    public:
      AssetSystem();
      ~AssetSystem() = default;

      /**
       * @return Singleton instance of asset system.
       */
      static AssetSystem& Instance();

      /**
      * @brief Releases all assets
      */
      static void Shutdown();

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
       * @brief Get a handle to an asset by ID.
       * @tparam T The type asset.
       * @param[in] id (sulphur::foundation::AssetID) The ID of the asset.
       * @return (sulphur::engine::AssetHandle<T>) A handle to the asset of type T.
       */
      template <class T>
      AssetHandle<T> GetHandle(foundation::AssetID id);

      /**
       * @brief Get a handle to an asset by name.
       * @tparam T The type of the asset.
       * @param[in] name (const sulphur::foundation::AssetName&) The name of the asset.
       * @return (sulphur::engine::AssetHandle<T>) A handle to the asset of type T.
       */
      template <class T>
      AssetHandle<T> GetHandle(const foundation::AssetName& name);

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
      * @tparam T The type of the asset.
      * @param[in] name (const sulphur::foundation::AssetName&) The ID of the asset.
      * @return (sulphur::engine::AssetHandle<T>) A handle to the asset of type T.
      */
      template <class T>
      AssetHandle<T> Load(const foundation::AssetName& name);

      /**
      * @brief Releases all GPU handles and resets them to invalid without invalidating the CPU handles.
      */
      static void ReleaseGPUHandles();

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
      /**
       * @brief Initializes the asset managers.
       */
      void Initialize();

      ModelManager model_manager_;                  //!< Model manager used by the asset system.
      MeshManager mesh_manager_;                    //!< Mesh manager used by the asset system.
      TextureManager texture_manager_;              //!< Texture manager used by the asset system.
      ShaderManager shader_manager_;                //!< Shader manager used by the asset system.
      ShaderProgramManager shader_program_manager_; //!< Shader program manager used by the asset system.
      MaterialManager material_manager_;            //!< Material manager used by the asset system.
      bool initialized_;                            //!< If the asset system has been initialized.
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
  }
}
