#include "engine/scripting/wrappers/script_entity.h"
#include "engine/scripting/wrappers/script_transform_component.h"
#include "engine/scripting/wrappers/script_mesh_renderer_component.h"
#include "engine/scripting/wrappers/script_rigid_body_component.h"
#include "engine/scripting/wrappers/script_box_collider_component.h"
#include "engine/scripting/scriptable_values/scriptable_args.h"

#include "engine/core/entity_system.h"

namespace sulphur
{
  namespace engine
  {
    EntitySystem* ScriptEntity::entity_system_;

    //-------------------------------------------------------------------------
    const char* ScriptEntity::components_[] =
    {
      "TransformComponent",
      "MeshRenderComponent",
      "RigidBodyComponent",
      "BoxColliderComponent",
      "CameraComponent",
      "DebugRenderComponent",
      NULL
    };

    //-------------------------------------------------------------------------
    void ScriptEntity::New(ScriptableArgs* args)
    {
      Entity entity = entity_system_->Create();
      Entity* luaEntity = reinterpret_cast<Entity*>(lua_newuserdata(args->script_system()->lua_state(), sizeof(entity)));
      luaEntity->handle = entity.handle;
      foundation::SharedPointer<ScriptableValue> scriptEntity = args->script_system()->GetFromStack(-1);
      SetMetaTable(scriptEntity, Name());

      args->AddReturn(scriptEntity);
    }

    //-------------------------------------------------------------------------
    void ScriptEntity::Destroy(ScriptableArgs* args)
    {
      args->Check("U");
      Entity* entity = CheckUserData<Entity>(Name(), args->GetArg(0));

      entity_system_->Destroy(*entity);
    }

    foundation::SharedPointer<ScriptableValue>
      ScriptEntity::Instantiate(ScriptSystem* script_system, const Entity & entity)
    {
      Entity* luaEntity = reinterpret_cast<Entity*>(lua_newuserdata(script_system->lua_state(), sizeof(entity)));
      luaEntity->handle = entity.handle;
      foundation::SharedPointer<ScriptableValue> scriptEntity = script_system->GetFromStack(-1);
      SetMetaTable(scriptEntity, Name());

      return scriptEntity;
    }

    //-------------------------------------------------------------------------
    void ScriptEntity::RegisterComponentList(ScriptSystem* state)
    {
      foundation::SharedPointer<ScriptableValue> components = ScriptUtils::CreateTable(state);
      const char* component_name = components_[0];
      int i = 0;
      while (component_name != NULL)
      {
        ScriptUtils::SetObjectValue(
          components,
          ScriptUtils::Instantiate(state, component_name),
          ScriptUtils::Instantiate(state, i)
        );

        ++i;
        component_name = components_[i];
      }

      state->RegisterValue("Components", components);
    }

    //-------------------------------------------------------------------------
    void ScriptEntity::Add(ScriptableArgs* args)
    {
      args->Check("UN");
      Entity* entity = CheckUserData<Entity>(Name(), args->GetArg(0));
      int component = ScriptUtils::As<int>(args->GetArg(1));

      switch (component)
      {
      case 0:
      {
        TransformComponent tcomponent = entity->Add<TransformComponent>();
        args->AddReturn(ScriptTransformComponent::Instantiate(args->script_system(), tcomponent));
        return;
      }
      case 1:
      {
        MeshRenderComponent mcomponent = entity->Add<MeshRenderComponent>();
        args->AddReturn(ScriptMeshRendererComponent::Instantiate(args->script_system(), mcomponent));
        return;
      }
      case 2:
      {
        RigidBodyComponent rcomponent = entity->Add<RigidBodyComponent>();
        args->AddReturn(ScriptRigidBodyComponent::Instantiate(args->script_system(), rcomponent));
        return;
      }
      case 3:
      {
        BoxColliderComponent bcomponent = entity->Add<BoxColliderComponent>();
        args->AddReturn(ScriptBoxColliderComponent::Instantiate(args->script_system(), bcomponent));
        return;
      }
      };
    }

    //-------------------------------------------------------------------------
    void ScriptEntity::Get(ScriptableArgs* args)
    {
      args->Check("UN");
      Entity* entity = CheckUserData<Entity>(Name(), args->GetArg(0));
      int component = ScriptUtils::As<int>(args->GetArg(1));

      switch (component)
      {
      case 0:
      {
        TransformComponent tcomponent = entity->Get<TransformComponent>();
        args->AddReturn(ScriptTransformComponent::Instantiate(args->script_system(), tcomponent));
        return;
      }
      case 1:
      {
        MeshRenderComponent mcomponent = entity->Get<MeshRenderComponent>();
        args->AddReturn(ScriptMeshRendererComponent::Instantiate(args->script_system(), mcomponent));
        return;
      }
      case 2:
      {
        RigidBodyComponent rcomponent = entity->Get<RigidBodyComponent>();
        args->AddReturn(ScriptRigidBodyComponent::Instantiate(args->script_system(), rcomponent));
        return;
      }
      case 3:
      {
        BoxColliderComponent bcomponent = entity->Get<BoxColliderComponent>();
        args->AddReturn(ScriptBoxColliderComponent::Instantiate(args->script_system(), bcomponent));
        return;
      }
      };
    }

    //-------------------------------------------------------------------------
    void ScriptEntity::Remove(ScriptableArgs*)
    {
    }

    //-------------------------------------------------------------------------
    void ScriptEntity::Has(ScriptableArgs*)
    {
    }
  }
}
