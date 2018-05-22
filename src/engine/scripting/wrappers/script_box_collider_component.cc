#include "engine/scripting/wrappers/script_box_collider_component.h"
#include "engine/scripting/script_utils.h"
#include "engine/scripting/scriptable_values/scriptable_args.h"

namespace sulphur
{
  namespace engine
  {
    foundation::SharedPointer<ScriptableValue>
      ScriptBoxColliderComponent::Instantiate(ScriptSystem* script_system, BoxColliderComponent box_collider_component)
    {
      World& world = script_system->application().GetService<WorldProviderSystem>().GetWorld();

      BoxColliderComponent * component
        = reinterpret_cast<BoxColliderComponent*>(lua_newuserdata(script_system->lua_state(), sizeof(BoxColliderComponent)));

      // Construct component in-place so it has a valid handle and reference to its system
      new (component) BoxColliderComponent(world.GetSystem<BoxColliderSystem>(), box_collider_component.handle);

      foundation::SharedPointer<ScriptableValue> script_component = script_system->GetFromStack(-1);
      SetMetaTable(script_component, Name());

      return script_component;
    }

    void ScriptBoxColliderComponent::SetExtents(ScriptableArgs* args)
    {
      args->Check("UNNN");
      BoxColliderComponent* component = CheckUserData<BoxColliderComponent>(Name(), args->GetArg(0));
      component->SetExtents(glm::vec3(
        ScriptUtils::As<float>(args->GetArg(1)),
        ScriptUtils::As<float>(args->GetArg(2)),
        ScriptUtils::As<float>(args->GetArg(3))
      ));
    }

    void ScriptBoxColliderComponent::Extents(ScriptableArgs* args)
    {
      args->Check("U");
      BoxColliderComponent* component = CheckUserData<BoxColliderComponent>(Name(), args->GetArg(0));
      glm::vec3 ret = component->Extents();

      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }
  }
}
