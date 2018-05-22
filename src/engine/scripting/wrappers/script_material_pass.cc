#include "engine/scripting/wrappers/script_material_pass.h"
#include "engine/scripting/wrappers/script_asset_handle.h"
#include "engine/scripting/script_utils.h"
#include "engine/scripting/script_system.h"
#include "engine/assets/material.h"
#include "engine/scripting/scriptable_values/scriptable_args.h"

void sulphur::engine::ScriptMaterialPass::New(ScriptableArgs* args)
{
  args->Check("");
  MaterialPass* material_pass = reinterpret_cast<MaterialPass*>(lua_newuserdata(args->script_system()->lua_state(), sizeof(MaterialPass)));
  material_pass = new(material_pass) MaterialPass();
  foundation::SharedPointer<ScriptableValue> script_material_pass = args->script_system()->GetFromStack(-1);
  SetMetaTable( script_material_pass, Name());
  args->AddReturn(script_material_pass);
}

sulphur::foundation::SharedPointer<sulphur::engine::ScriptableValue>
sulphur::engine::ScriptMaterialPass::Copy(ScriptSystem* script_system, const MaterialPass& material_pass)
{
  MaterialPass* lua_material_pass = reinterpret_cast<MaterialPass*>(lua_newuserdata(script_system->lua_state(), sizeof(MaterialPass)));
  lua_material_pass = new(lua_material_pass) MaterialPass(material_pass);
  foundation::SharedPointer<ScriptableValue> script_material_pass = script_system->GetFromStack(-1);
  SetMetaTable(script_material_pass, Name());
  return script_material_pass;
}

void sulphur::engine::ScriptMaterialPass::SetShader(ScriptableArgs* args)
{
  args->Check("UU");
  MaterialPass* material_pass = CheckUserData<MaterialPass>(Name(), args->GetArg(0));
  ShaderHandle* handle = ScriptAssetHandle::Get<Shader>(args->GetArg(1));
  material_pass->SetShader(*handle);
}

void sulphur::engine::ScriptMaterialPass::shader(ScriptableArgs* args)
{
  args->Check("U");
  MaterialPass* material_pass
    = CheckUserData<MaterialPass>(Name(), args->GetArg(0));
  args->AddReturn(ScriptAssetHandle::Instantiate(args->script_system(), material_pass->shader()));
}

void sulphur::engine::ScriptMaterialPass::textures(ScriptableArgs* args)
{
  args->Check("U");
  MaterialPass* material_pass
    = CheckUserData<MaterialPass>(Name(), args->GetArg(0));
  foundation::SharedPointer<ScriptableValue> table
    = ScriptUtils::CreateTable(args->script_system());

  foundation::Vector<TextureHandle> textures = material_pass->textures();
  for (int i = 0; i < textures.size(); i++)
  {
    ScriptUtils::SetObjectValue(
      table,
      ScriptUtils::Instantiate(args->script_system(), i),
      ScriptAssetHandle::Instantiate(args->script_system(), textures[i])
    );
  }

  args->AddReturn(table);
}

void sulphur::engine::ScriptMaterialPass::GetTexture(ScriptableArgs* args)
{
  args->Check("UN");
  MaterialPass* material_pass
    = CheckUserData<MaterialPass>(Name(), args->GetArg(0));

  int i = ScriptUtils::As<int>(args->GetArg(1));

  args->AddReturn(ScriptAssetHandle::Instantiate(args->script_system(), material_pass->GetTexture(i)));
}

void sulphur::engine::ScriptMaterialPass::SetTexture(ScriptableArgs* args)
{
  args->Check("UNU");
  MaterialPass* material_pass
    = CheckUserData<MaterialPass>(Name(), args->GetArg(0));

  int i = ScriptUtils::As<int>(args->GetArg(1));

  TextureHandle* handle = ScriptAssetHandle::Get<Texture>(args->GetArg(2));

  material_pass->SetTexture(i, *handle);
}

void sulphur::engine::ScriptMaterialPass::NumTextures(ScriptableArgs* args)
{
  args->Check("U");
  MaterialPass* material_pass
    = CheckUserData<MaterialPass>(Name(), args->GetArg(0));

  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), material_pass->NumTextures()));
}
