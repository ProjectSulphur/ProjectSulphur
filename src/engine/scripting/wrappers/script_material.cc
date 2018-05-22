#include "engine/scripting/wrappers/script_material.h"
#include "engine/scripting/wrappers/script_material_pass.h"
#include "engine/scripting/script_system.h"
#include "engine/scripting/script_utils.h"
#include "engine/assets/material.h"
#include "engine/scripting/scriptable_values/scriptable_args.h"

void sulphur::engine::ScriptMaterial::New(ScriptableArgs* args)
{
  args->Check("");
  Material* material = reinterpret_cast<Material*>(lua_newuserdata(args->script_system()->lua_state(), sizeof(Material)));
  material = new(material) Material();
  foundation::SharedPointer<ScriptableValue> script_material = args->script_system()->GetFromStack(-1);
  SetMetaTable( script_material, Name());
  args->AddReturn(script_material);
}

sulphur::foundation::SharedPointer<sulphur::engine::ScriptableValue> 
sulphur::engine::ScriptMaterial::Copy(ScriptSystem* script_system, const Material& material)
{
  Material* lua_material = reinterpret_cast<Material*>(lua_newuserdata(script_system->lua_state(), sizeof(Material)));
  lua_material = new(lua_material) Material(material);
  foundation::SharedPointer<ScriptableValue> script_material = script_system->GetFromStack(-1);
  SetMetaTable(script_material, Name());
  return script_material;
}

void sulphur::engine::ScriptMaterial::material_passes(ScriptableArgs* args)
{
  args->Check("U");
  Material* material = CheckUserData<Material>(Name(), args->GetArg(0));
  

  foundation::SharedPointer<ScriptableValue> table
    = ScriptUtils::CreateTable(args->script_system());
  for (int i = 0; i < material->NumMaterialPasses(); i++)
  {
    ScriptUtils::SetObjectValue(
      table,
      ScriptUtils::Instantiate(args->script_system(), i),
      ScriptMaterialPass::Copy(args->script_system(), material->GetMaterialPass(i))
    );
  }

  args->AddReturn(table);
}

void sulphur::engine::ScriptMaterial::AddMaterialPass(ScriptableArgs* args)
{
  args->Check("UU");
  Material* material = CheckUserData<Material>(Name(), args->GetArg(0));
  MaterialPass* material_pass = CheckUserData<MaterialPass>(ScriptMaterialPass::Name(), args->GetArg(1));

  material->AddMaterialPass(*material_pass);
}

void sulphur::engine::ScriptMaterial::GetMaterialPass(ScriptableArgs* args)
{
  args->Check("UN");
  Material* material = CheckUserData<Material>(Name(), args->GetArg(0));

  int i = ScriptUtils::As<int>(args->GetArg(1));

  args->AddReturn(ScriptMaterialPass::Copy(args->script_system(), material->GetMaterialPass(i)));
}

void sulphur::engine::ScriptMaterial::SetMaterialPass(ScriptableArgs* args)
{
  args->Check("UNU");
  Material* material = CheckUserData<Material>(Name(), args->GetArg(0));
  MaterialPass* material_pass = CheckUserData<MaterialPass>(ScriptMaterialPass::Name(), args->GetArg(2));
  int i = ScriptUtils::As<int>(args->GetArg(1));

  material->SetMaterialPass(i, *material_pass);
}

void sulphur::engine::ScriptMaterial::RemoveMaterialPass(ScriptableArgs* args)
{
  args->Check("UN");
  Material* material = CheckUserData<Material>(Name(), args->GetArg(0));

  int i = ScriptUtils::As<int>(args->GetArg(1));

  material->RemoveMaterialPass(i);
}

void sulphur::engine::ScriptMaterial::NumMaterialPasses(ScriptableArgs* args)
{
  args->Check("U");
  Material* material = CheckUserData<Material>(Name(), args->GetArg(0));
  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), material->NumMaterialPasses()));
}
