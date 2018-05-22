#include "engine/scripting/wrappers/script_mesh_renderer_component.h"
#include "engine/scripting/wrappers/script_asset_handle.h"
#include "engine/scripting/scriptable_values/scriptable_args.h"

namespace sulphur
{
  namespace engine
  {
    foundation::SharedPointer<ScriptableValue>
      ScriptMeshRendererComponent::Instantiate(ScriptSystem* script_system, MeshRenderComponent mesh_renderer_component)
    {
      World& world = script_system->application().GetService<WorldProviderSystem>().GetWorld();

      MeshRenderComponent * component
        = reinterpret_cast<MeshRenderComponent*>(lua_newuserdata(script_system->lua_state(), sizeof(MeshRenderComponent)));

      // Construct component in-place so it has a valid handle and reference to its system
      new (component) MeshRenderComponent(world.GetSystem<MeshRenderSystem>(), mesh_renderer_component.handle);

      foundation::SharedPointer<ScriptableValue> script_component = script_system->GetFromStack(-1);
      SetMetaTable(script_component, Name());

      return script_component;
    }

    void ScriptMeshRendererComponent::SetMesh(ScriptableArgs* args)
    {
      args->Check("uu");
      MeshRenderComponent* component = CheckUserData<MeshRenderComponent>(Name(), args->GetArg(0));
      MeshHandle* mesh = ScriptAssetHandle::Get<Mesh>(args->GetArg(1));
      component->SetMesh(*mesh);
      args->AddReturn(args->GetArg(0));
    }

    void ScriptMeshRendererComponent::GetMesh(ScriptableArgs* args)
    {
      args->Check("u");
      MeshRenderComponent* component = CheckUserData<MeshRenderComponent>(Name(), args->GetArg(0));
      args->AddReturn(ScriptAssetHandle::Instantiate(args->script_system(), component->GetMesh()));
    }

    void ScriptMeshRendererComponent::SetMaterial(ScriptableArgs* args)
    {
      args->Check("uu");
      MeshRenderComponent* component = CheckUserData<MeshRenderComponent>(Name(), args->GetArg(0));
      MaterialHandle* material = ScriptAssetHandle::Get<Material>(args->GetArg(1));
      component->SetMaterial(*material);
      args->AddReturn(args->GetArg(0));
    }

    void ScriptMeshRendererComponent::GetMaterial(ScriptableArgs* args)
    {

      int index = 0;
      if (args->NumArgs() == 2)
      {
        args->Check("un");
        index = ScriptUtils::As<int>(args->GetArg(1));
      }
      else
      {
        args->Check("u");
      }
      MeshRenderComponent* component = CheckUserData<MeshRenderComponent>(Name(), args->GetArg(0));
      args->AddReturn(ScriptAssetHandle::Instantiate(args->script_system(), component->GetMaterial()));
    }

    void ScriptMeshRendererComponent::SetMaterials(ScriptableArgs* args)
    {
      args->Check("u", 1);

      MeshRenderComponent* component = CheckUserData<MeshRenderComponent>(Name(), args->GetArg(0));
      foundation::Vector<MaterialHandle> materials;

      foundation::SharedPointer<ScriptableValue> table = ScriptUtils::CreateTable(args->script_system());
      for (int i = 1; i < args->NumArgs(); ++i)
      {
        materials.push_back(*ScriptAssetHandle::Get<Material>(args->GetArg(i)));
      }
      component->SetMaterials(materials);
      args->AddReturn(args->GetArg(0));
    }

    void ScriptMeshRendererComponent::GetMaterials(ScriptableArgs* args)
    {
      args->Check("u");

      MeshRenderComponent* component = CheckUserData<MeshRenderComponent>(Name(), args->GetArg(0));
      foundation::Vector<MaterialHandle> materials = component->GetMaterials();
      foundation::SharedPointer<ScriptableValue> table = ScriptUtils::CreateTable(args->script_system());
      for (int i = 0; i < materials.size(); ++i)
      {
        ScriptUtils::SetObjectValue(
          table,
          ScriptUtils::Instantiate(args->script_system(), i),
          ScriptAssetHandle::Instantiate(args->script_system(), materials[i])
        );
      }

      args->AddReturn(table);
    }

    void ScriptMeshRendererComponent::SetVisible(ScriptableArgs* args)
    {
      args->Check("ub");
      MeshRenderComponent* component = CheckUserData<MeshRenderComponent>(Name(), args->GetArg(0));
      bool visible = ScriptUtils::As<bool>(args->GetArg(1));
      component->SetVisible(visible);
      args->AddReturn(args->GetArg(0));
    }

    void ScriptMeshRendererComponent::IsVisible(ScriptableArgs* args)
    {
      args->Check("u");
      MeshRenderComponent* component = CheckUserData<MeshRenderComponent>(Name(), args->GetArg(0));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), component->IsVisible()));
    }

    void ScriptMeshRendererComponent::SetCastShadows(ScriptableArgs* args)
    {
      args->Check("ub");
      MeshRenderComponent* component = CheckUserData<MeshRenderComponent>(Name(), args->GetArg(0));
      bool visible = ScriptUtils::As<bool>(args->GetArg(1));
      component->SetCastShadows(visible);
      args->AddReturn(args->GetArg(0));
    }

    void ScriptMeshRendererComponent::CastsShadows(ScriptableArgs* args)
    {
      args->Check("u");
      MeshRenderComponent* component = CheckUserData<MeshRenderComponent>(Name(), args->GetArg(0));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), component->CastsShadows()));
    }
  }
}
