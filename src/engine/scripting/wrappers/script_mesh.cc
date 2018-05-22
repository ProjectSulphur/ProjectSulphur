#include "engine/scripting/wrappers/script_mesh.h"
#include "engine/scripting/script_system.h"
#include "engine/scripting/script_utils.h"
#include "engine/scripting/scriptable_values/scriptable_args.h"


void sulphur::engine::ScriptMesh::New(ScriptableArgs* args)
{
  args->Check("");
  Mesh* mesh = reinterpret_cast<Mesh*>(lua_newuserdata(args->script_system()->lua_state(), sizeof(Mesh)));
  mesh = new(mesh) Mesh();
  foundation::SharedPointer<ScriptableValue> script_mesh = args->script_system()->GetFromStack(-1);
  SetMetaTable( script_mesh, Name());
  args->AddReturn(script_mesh);
}

sulphur::foundation::SharedPointer<sulphur::engine::ScriptableValue>
  sulphur::engine::ScriptMesh::Copy(ScriptSystem* script_system, const Mesh& mesh)
{
  Mesh* lua_mesh = reinterpret_cast<Mesh*>(lua_newuserdata(script_system->lua_state(), sizeof(Mesh)));
  lua_mesh = new(lua_mesh) Mesh(mesh);
  foundation::SharedPointer<ScriptableValue> script_mesh = script_system->GetFromStack(-1);
  SetMetaTable(script_mesh, Name());
  return script_mesh;
}


void sulphur::engine::ScriptMesh::CalculateBounds(ScriptableArgs* args)
{
  args->Check("U");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  mesh->CalculateBounds();
}

void sulphur::engine::ScriptMesh::RecalculateNormals(ScriptableArgs* args)
{
  args->Check("U");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  mesh->RecalculateNormals();
}

void sulphur::engine::ScriptMesh::RecalculateTangents(ScriptableArgs* args)
{
  args->Check("U");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  mesh->RecalculateTangents();
}

void sulphur::engine::ScriptMesh::FlipNormals(ScriptableArgs* args)
{
  args->Check("U");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  mesh->FlipNormals();
  args->AddReturn(args->GetArg(0));
}

void sulphur::engine::ScriptMesh::AttachMesh(ScriptableArgs* args)
{
  args->Check("UU");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  Mesh* other = CheckUserData<Mesh>(Name(), args->GetArg(1));
  mesh->AttachMesh(*other);
}

void sulphur::engine::ScriptMesh::Validate(ScriptableArgs* args)
{
  args->Check("U");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  mesh->Validate();
}

void sulphur::engine::ScriptMesh::Optimize(ScriptableArgs* args)
{
  args->Check("U");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  mesh->Optimize();
}

void sulphur::engine::ScriptMesh::Clear(ScriptableArgs* args)
{
  args->Check("U");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  mesh->Clear(ScriptUtils::As<bool>(args->GetArg(1)));
}

void sulphur::engine::ScriptMesh::Shade(ScriptableArgs* args)
{
  args->Check("UN");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  mesh->Shade(foundation::Color::FromARGB(ScriptUtils::As<int>(args->GetArg(1))));
}

void sulphur::engine::ScriptMesh::SetColor(ScriptableArgs* args)
{
  args->Check("UN");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  mesh->SetColor(foundation::Color::FromARGB(ScriptUtils::As<int>(args->GetArg(1))));
}

void sulphur::engine::ScriptMesh::MarkDynamic(ScriptableArgs* args)
{
  args->Check("UB");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  mesh->MarkDynamic(ScriptUtils::As<bool>(args->GetArg(1)));
}

void sulphur::engine::ScriptMesh::HasChanged(ScriptableArgs* args)
{
  args->Check("U");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), mesh->HasChanged()));
}

void sulphur::engine::ScriptMesh::ShouldUpdateIndexBuffer(ScriptableArgs* args)
{
  args->Check("U");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), mesh->ShouldUpdateIndexBuffer()));
}

void sulphur::engine::ScriptMesh::ShouldUpdatePositionBuffer(ScriptableArgs* args)
{
  args->Check("U");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), mesh->ShouldUpdatePositionBuffer()));
}

void sulphur::engine::ScriptMesh::ShouldUpdateColorBuffer(ScriptableArgs* args)
{
  args->Check("U");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), mesh->ShouldUpdateColorBuffer()));
}

void sulphur::engine::ScriptMesh::ShouldUpdateDataBuffer(ScriptableArgs* args)
{
  args->Check("U");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), mesh->ShouldUpdateDataBuffer()));
}

void sulphur::engine::ScriptMesh::IsStatic(ScriptableArgs* args)
{
  args->Check("U");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), mesh->IsStatic()));
}

void sulphur::engine::ScriptMesh::IsEmpty(ScriptableArgs* args)
{
  args->Check("U");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), mesh->IsEmpty()));
}

void sulphur::engine::ScriptMesh::IsAlwaysOnTop(ScriptableArgs* args)
{
  args->Check("U");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), mesh->IsAlwaysOnTop()));
}

void sulphur::engine::ScriptMesh::SetAlwaysOnTop(ScriptableArgs* args)
{
  args->Check("UB");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  mesh->SetAlwaysOnTop(ScriptUtils::As<bool>(args->GetArg(1)));
}

void sulphur::engine::ScriptMesh::GetVertexCount(ScriptableArgs* args)
{
  args->Check("U");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), mesh->GetVertexCount()));
}

void sulphur::engine::ScriptMesh::GetIndexCount(ScriptableArgs* args)
{
  args->Check("U");
  Mesh* mesh = CheckUserData<Mesh>(Name(), args->GetArg(0));
  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), mesh->GetIndexCount()));
}

void sulphur::engine::ScriptMesh::CreatePoint(ScriptableArgs* args)
{
  args->Check("");
  args->AddReturn(Copy(args->script_system(), Mesh::CreatePoint()));
}

void sulphur::engine::ScriptMesh::CreateTriangle(ScriptableArgs* args)
{
  args->Check("");
  args->AddReturn(Copy(args->script_system(), Mesh::CreateTriangle()));
}

void sulphur::engine::ScriptMesh::CreateQuad(ScriptableArgs* args)
{
  args->Check("");
  args->AddReturn(Copy(args->script_system(), Mesh::CreateQuad()));
}

void sulphur::engine::ScriptMesh::CreateCube(ScriptableArgs* args)
{
  args->Check("");
  args->AddReturn(Copy(args->script_system(), Mesh::CreateCube()));
}

void sulphur::engine::ScriptMesh::CreatePlane(ScriptableArgs* args)
{
  args->Check("");
  args->AddReturn(Copy(args->script_system(), Mesh::CreatePlane()));
}

void sulphur::engine::ScriptMesh::CreatePyramid(ScriptableArgs* args)
{
  args->Check("");
  args->AddReturn(Copy(args->script_system(), Mesh::CreatePyramid()));
}

void sulphur::engine::ScriptMesh::CreateCone(ScriptableArgs* args)
{
  args->Check("");
  args->AddReturn(Copy(args->script_system(), Mesh::CreateCone()));
}

void sulphur::engine::ScriptMesh::CreateCylinder(ScriptableArgs* args)
{
  args->Check("");
  args->AddReturn(Copy(args->script_system(), Mesh::CreateCylinder()));
}

void sulphur::engine::ScriptMesh::CreateCircle(ScriptableArgs* args)
{
  args->Check("");
  args->AddReturn(Copy(args->script_system(), Mesh::CreateCircle()));
}

void sulphur::engine::ScriptMesh::CreateHexPointy(ScriptableArgs* args)
{
  args->Check("");
  args->AddReturn(Copy(args->script_system(), Mesh::CreateHexPointy()));
}

void sulphur::engine::ScriptMesh::CreateHexFlat(ScriptableArgs* args)
{
  args->Check("");
  args->AddReturn(Copy(args->script_system(), Mesh::CreateHexFlat()));
}

void sulphur::engine::ScriptMesh::CreateSphere(ScriptableArgs* args)
{
  args->Check("");
  args->AddReturn(Copy(args->script_system(), Mesh::CreateSphere()));
}
