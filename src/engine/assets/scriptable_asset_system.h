#pragma once

#include "engine/scripting/scriptable_object.h"

#include <foundation/utils/asset_definitions.h>

namespace sulphur
{
  namespace engine
  {
    template <typename T>
    class AssetHandle;

    /**
    * @class sulphur::engine::ScriptableAsset : public sulphur::engine::ScriptableObject
    * @brief Contains an asset handle to be fed into the asset system
    * @author Daniel Konings
    */
    SCRIPT_CLASS() class ScriptableAsset : public ScriptableObject
    {

    public:

      /**
      * @brief Used to expose the asset types enumerator to Lua
      * @see sulphur::engine::AssetType
      * @remarks This has to be done because of the project's cross-dependency with graphics,
      *          we don't want to include scripting there.
      */
      SCRIPT_ENUM() enum AssetTypes
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
        kUnknown
      };

      SCRIPT_NAME(Asset);

      /**
      * @brief Default constructor, invalidates all handle information
      */
      ScriptableAsset();

      /**
      * @brief Creates a new handle from an asset handle
      * @param[in] type (sulphur::engine::ScriptableAsset::AssetTypes) The type of the handle
      * @param[in] name (const sulphur::foundation::AssetName&) The name of this asset
      * @param[in] handle (sulphur::engine::AssetHandle<void>&) The handle to save in this handle
      */
      ScriptableAsset(
        AssetTypes type, 
        const foundation::AssetName& name, 
        AssetHandle<void>& handle);

      /**
      * @return (bool) Is this handle valid?
      */
      SCRIPT_FUNC() bool IsValid();

      /**
      * @return (sulphur::engine::AssetHandle<void>*) The handle contained in this scriptable asset
      */
      AssetHandle<void>* GetHandle();

      /**
      * @return (sulphur::engine::ScriptableAsset::AssetTypes) The type of this asset
      */
      AssetTypes type() const;

      /**
      * @return (const sulphur::foundation::AssetName&) The name of this asset
      */
      const foundation::AssetName& name() const;

    private:

      AssetTypes type_; //!< The type of this asset
      foundation::AssetName name_; //!< The name of this asset
      foundation::SharedPointer<AssetHandle<void>> handle_; //!< The actual handle
      bool valid_; //!< Is this handle valid?
    };

    /**
    * @class sulphur::engine::ScriptableAssetSystem : public sulphur::engine::ScriptableObject
    * @brief Used to load assets statically in the scripting environment by type ID
    * @author Daniel Konings
    */
    SCRIPT_CLASS() class ScriptableAssetSystem : public ScriptableObject
    {
      
    public:

      SCRIPT_NAME(AssetSystem);

      /**
      * @brief Loads an asset by name in the caches
      * @see sulphur::engine::AssetSystem::Load
      * @param[in] type (sulphur::engine::ScriptableAsset::AssetTypes) The type of asset to load
      * @return (sulphur::engine::ScriptableAsset) The loaded asset
      */
      SCRIPT_FUNC(static) ScriptableAsset Load(
        ScriptableAsset::AssetTypes type, 
        const foundation::String& name);

      /**
      * @brief Unloads an asset by its script handle
      * @param[in] value (sulphur::engine::ScriptHandle) The handle to unload
      */
      SCRIPT_FUNC(static) void Unload(ScriptHandle value);
    };
  }
}