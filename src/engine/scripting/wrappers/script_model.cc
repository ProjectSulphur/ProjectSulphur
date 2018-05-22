#include "engine/scripting/wrappers/script_model.h"
#include "engine/scripting/wrappers/script_asset_handle.h"
#include "engine/scripting/wrappers/script_mesh.h"
#include "engine/scripting/script_utils.h"
#include "engine/scripting/scriptable_values/scriptable_args.h"

void sulphur::engine::ScriptModel::New(ScriptableArgs* args)
{
  args->Check("UO");
  MeshHandle* mesh_handle = ScriptAssetHandle::Get<Mesh>(args->GetArg(0));

  foundation::Map<
    foundation::SharedPointer<ScriptableValue>,
    foundation::SharedPointer<ScriptableValue>
  > materials = ScriptUtils::GetObject(args->GetArg(1));

  foundation::Vector<MaterialHandle> material_handles;

  for (eastl::pair<
    foundation::SharedPointer<ScriptableValue>,
    foundation::SharedPointer<ScriptableValue>
  > material : materials)
  {
    MaterialHandle* material_handle
      = ScriptAssetHandle::Get<Material>(material.second);
    material_handles.push_back(*material_handle);
  }

  Model* model = reinterpret_cast<Model*>(lua_newuserdata(args->script_system()->lua_state(), sizeof(Model)));
  model = new(model) Model(*mesh_handle, material_handles);
  foundation::SharedPointer<ScriptableValue> script_model = args->script_system()->GetFromStack(-1);
  SetMetaTable( script_model, Name());
  args->AddReturn(script_model);
}

sulphur::foundation::SharedPointer<sulphur::engine::ScriptableValue>
sulphur::engine::ScriptModel::Copy(ScriptSystem* script_system, const Model& model)
{
  Model* lua_model = reinterpret_cast<Model*>(lua_newuserdata(script_system->lua_state(), sizeof(Model)));
  lua_model = new(lua_model) Model(model);
  foundation::SharedPointer<ScriptableValue> script_model = script_system->GetFromStack(-1);
  SetMetaTable(script_model, Name());
  return script_model;
}

void sulphur::engine::ScriptModel::mesh_handle(ScriptableArgs* args)
{
  args->Check("U");
  Model* model = CheckUserData<Model>(Name(), args->GetArg(0));

  args->AddReturn(ScriptAssetHandle::Instantiate(args->script_system(), model->mesh_handle()));
}

void sulphur::engine::ScriptModel::material_handles(ScriptableArgs* args)
{
  args->Check("U");
  Model* model = CheckUserData<Model>(Name(), args->GetArg(0));

  foundation::SharedPointer<ScriptableValue> table
    = ScriptUtils::CreateTable(args->script_system());

  foundation::Vector<MaterialHandle> materials = model->material_handles();
  for (int i = 0; i < materials.size(); i++)
  {
    ScriptUtils::SetObjectValue(
      table,
      ScriptUtils::Instantiate(args->script_system(), i),
      ScriptAssetHandle::Instantiate(args->script_system(), materials[i])
    );
  }

  args->AddReturn(table);
}

void sulphur::engine::ScriptModel::GetMaterialHandle(ScriptableArgs* args)
{
  args->Check("UN");
  Model* model = CheckUserData<Model>(Name(), args->GetArg(0));

  int i = ScriptUtils::As<int>(args->GetArg(1));

  args->AddReturn(ScriptAssetHandle::Instantiate(args->script_system(), model->GetMaterialHandle(i)));
}

void sulphur::engine::ScriptModel::GetMaterialHandlesSize(ScriptableArgs* args)
{
  args->Check("U");
  Model* model = CheckUserData<Model>(Name(), args->GetArg(0));

  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), model->GetMaterialHandlesSize()));
}
