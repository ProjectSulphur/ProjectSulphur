#include "engine/assets/scriptable_asset_system.h"
#include "engine/assets/asset_system.h"

#include "engine/scripting/script_utils.h"

#include <lua-classes/scriptable_asset_system.lua.cc>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    ScriptableAsset::ScriptableAsset() :
      type_(AssetTypes::kUnknown),
      handle_(nullptr),
      valid_(false)
    {

    }

    //--------------------------------------------------------------------------------
    ScriptableAsset::ScriptableAsset(
      AssetTypes type,
      const foundation::AssetName& name,
      AssetHandle<void>& handle
    ) :
      type_(type),
      name_(name),
      handle_(nullptr),
      valid_(false)
    {
      handle_ = foundation::Memory::ConstructShared<AssetHandle<void>>(handle);

      valid_ = true;
    }

    //--------------------------------------------------------------------------------
    bool ScriptableAsset::IsValid()
    {
      return valid_;
    }

    //--------------------------------------------------------------------------------
    AssetHandle<void>* ScriptableAsset::GetHandle()
    {
      return handle_.get();
    }

    //--------------------------------------------------------------------------------
    ScriptableAsset::AssetTypes ScriptableAsset::type() const
    {
      return type_;
    }

    //--------------------------------------------------------------------------------
    const foundation::AssetName& ScriptableAsset::name() const
    {
      return name_;
    }

    //--------------------------------------------------------------------------------
    ScriptableAsset ScriptableAssetSystem::Load(
      ScriptableAsset::AssetTypes type, 
      const foundation::String& name)
    {
      if (type >= ScriptableAsset::AssetTypes::kUnknown || name.size() > 64)
      {
        PS_LOG(Error, "Attempted to load an invalid asset type from scripting");
        return ScriptableAsset();
      }

      AssetHandle<void> handle = AssetSystem::Instance().Load(
        static_cast<AssetType>(type), 
        name);

      ScriptableAsset asset = ScriptableAsset(type, name, handle);
      return asset;
    }

    //--------------------------------------------------------------------------------
    void ScriptableAssetSystem::Unload(ScriptHandle value)
    {
      ScriptableAsset* asset = 
        reinterpret_cast<ScriptableAsset*>(ScriptUtils::As<void*>(value));

      if (
        asset->IsValid() == false || 
        asset->type() == ScriptableAsset::AssetTypes::kUnknown)
      {
        PS_LOG(Error, "Attempted to unload an invalid asset from scripting");
        return;
      }

      AssetType type = static_cast<AssetType>(asset->type());

      AssetSystem::Instance().Release(type, asset->name());
    }
  }
}