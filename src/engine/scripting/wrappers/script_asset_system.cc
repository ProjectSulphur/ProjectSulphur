#include "engine/scripting/wrappers/script_asset_system.h"
#include "engine/scripting/wrappers/script_asset_handle.h"
#include "engine/scripting/wrappers/script_mesh.h"
#include "engine/scripting/wrappers/script_model.h"
#include "engine/scripting/wrappers/script_shader.h"
#include "engine/scripting/wrappers/script_shader_program.h"
#include "engine/scripting/wrappers/script_material.h"
#include "engine/scripting/wrappers/script_texture.h"

#include "engine/scripting/scriptable_values/scriptable_args.h"


#include "engine/scripting/script_utils.h"

#include "engine/assets/asset_system.h"



const char * sulphur::engine::ScriptAssetSystem::AssetTypes[] =
{
  "Invalid",
  "Model",
  "Mesh",
  "Shader",
  "ShaderProgram",
  "Material",
  "Texture"
};

void sulphur::engine::ScriptAssetSystem::RegisterEnum(ScriptSystem* script_system)
{
  foundation::SharedPointer<ScriptableValue> assets = ScriptUtils::CreateTable(script_system);
  const char * asset_name = AssetTypes[0];
  int i = 0;
  while (asset_name != NULL)
  {
    ScriptUtils::SetObjectValue(
      assets,
      ScriptUtils::Instantiate(script_system, asset_name),
      ScriptUtils::Instantiate(script_system, i)
    );

    ++i;
    asset_name = AssetTypes[i];
  }

  script_system->RegisterValue("AssetTypes", assets);
}

void sulphur::engine::ScriptAssetSystem::AddAsset(ScriptableArgs* args)
{
  args->Check("NUS");

  int type = ScriptUtils::As<int>(args->GetArg(0));
  const char* name = ScriptUtils::As<const char*>(args->GetArg(2));
  AssetSystem& system = AssetSystem::Instance();

  // init with NIL
  foundation::SharedPointer<ScriptableValue> handle = ScriptUtils::Instantiate(args->script_system());
  switch (type)
  {
  case 1:
    handle = ScriptAssetHandle::Instantiate(
      args->script_system(),
      system.AddAsset(
        CheckUserData<Model>(
          ScriptModel::Name(),
          args->GetArg(1)),
        name
      )
    );
    break;
  case 2:
    handle = ScriptAssetHandle::Instantiate(
      args->script_system(), 
      system.AddAsset(
        CheckUserData<Mesh>(
          ScriptMesh::Name(),
          args->GetArg(1)),
          name
      )
    );
    break;
  case 3:
    handle = ScriptAssetHandle::Instantiate(
      args->script_system(),
      system.AddAsset(
        CheckUserData<Shader>(
          ScriptShader::Name(),
          args->GetArg(1)),
        name
      )
    );
    break;
  case 4:
    handle = ScriptAssetHandle::Instantiate(
      args->script_system(),
      system.AddAsset(
        CheckUserData<ShaderProgram>(
          ScriptShaderProgram::Name(),
          args->GetArg(1)),
        name
      )
    );
    break;
  case 5:
    handle = ScriptAssetHandle::Instantiate(
      args->script_system(),
      system.AddAsset(
        CheckUserData<Material>(
          ScriptMaterial::Name(),
          args->GetArg(1)),
        name
      )
    );
    break;
  case 6:
    handle = ScriptAssetHandle::Instantiate(
      args->script_system(),
      system.AddAsset(
        CheckUserData<Texture>(
          ScriptTexture::Name(),
          args->GetArg(1)),
        name
      )
    );
    break;
  }

  args->AddReturn(handle);
}

void sulphur::engine::ScriptAssetSystem::GetHandle(ScriptableArgs* args)
{
  args->Check("NS");

  int type = ScriptUtils::As<int>(args->GetArg(0));
  const char* name = ScriptUtils::As<const char*>(args->GetArg(1));

  foundation::AssetName asset_name = name;
  AssetSystem& system = AssetSystem::Instance();

  // init with NIL
  foundation::SharedPointer<ScriptableValue> handle= ScriptUtils::Instantiate(args->script_system()); 
  switch (type)
  {
  case 1:
    handle = ScriptAssetHandle::Instantiate(args->script_system(), system.GetHandle<Model>(asset_name));
    break;
  case 2:
    handle = ScriptAssetHandle::Instantiate(args->script_system(), system.GetHandle<Mesh>(asset_name));
    break;
  case 3:
    handle = ScriptAssetHandle::Instantiate(args->script_system(), system.GetHandle<Shader>(asset_name));
    break;
  case 4:
    handle = ScriptAssetHandle::Instantiate(args->script_system(), system.GetHandle<ShaderProgram>(asset_name));
    break;
  case 5:
    handle = ScriptAssetHandle::Instantiate(args->script_system(), system.GetHandle<Material>(asset_name));
    break;
  case 6:
    handle = ScriptAssetHandle::Instantiate(args->script_system(), system.GetHandle<Texture>(asset_name));
    break;
  }

  args->AddReturn(handle);
}

void sulphur::engine::ScriptAssetSystem::Load(ScriptableArgs* args)
{
  args->Check("NS");

  int type = ScriptUtils::As<int>(args->GetArg(0));
  const char* name = ScriptUtils::As<const char*>(args->GetArg(1));

  foundation::AssetName asset_name = name;
  AssetSystem& system = AssetSystem::Instance();

  // init with NIL
  foundation::SharedPointer<ScriptableValue> handle = ScriptUtils::Instantiate(args->script_system());
  switch (type)
  {
  case 1:
    handle = ScriptAssetHandle::Instantiate(args->script_system(), system.Load<Model>(asset_name));
    break;
  case 2:
    handle = ScriptAssetHandle::Instantiate(args->script_system(), system.Load<Mesh>(asset_name));
    break;
  case 3:
    handle = ScriptAssetHandle::Instantiate(args->script_system(), system.Load<Shader>(asset_name));
    break;
  case 4:
    handle = ScriptAssetHandle::Instantiate(args->script_system(), system.Load<ShaderProgram>(asset_name));
    break;
  case 5:
    handle = ScriptAssetHandle::Instantiate(args->script_system(), system.Load<Material>(asset_name));
    break;
  case 6:
    handle = ScriptAssetHandle::Instantiate(args->script_system(), system.Load<Texture>(asset_name));
    break;
  }

  args->AddReturn(handle);
}

void sulphur::engine::ScriptAssetSystem::Release(ScriptableArgs* args)
{
  args->Check("NS");

  int type = ScriptUtils::As<int>(args->GetArg(0));
  const char* name = ScriptUtils::As<const char*>(args->GetArg(1));

  foundation::AssetName asset_name = name;
  AssetSystem& system = AssetSystem::Instance();

  system.Release(static_cast<AssetType>(type), asset_name);
}
