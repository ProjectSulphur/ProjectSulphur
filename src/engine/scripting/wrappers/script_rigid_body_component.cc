#include "engine/scripting/wrappers/script_rigid_body_component.h"
#include "engine/scripting/script_utils.h"
#include "glm/vec3.hpp"
#include "engine/scripting/scriptable_values/scriptable_args.h"

namespace sulphur
{
  namespace engine
  {
    foundation::SharedPointer<ScriptableValue>
      ScriptRigidBodyComponent::Instantiate(ScriptSystem * script_system, RigidBodyComponent rigidbody_component)
    {
      World& world = script_system->application().GetService<WorldProviderSystem>().GetWorld();

      RigidBodyComponent* component
        = reinterpret_cast<RigidBodyComponent*>(lua_newuserdata(script_system->lua_state(), sizeof(RigidBodyComponent)));

      // Construct component in-place so it has a valid handle and reference to its system
      new (component) RigidBodyComponent(world.GetSystem<RigidBodySystem>(), rigidbody_component.handle);

      foundation::SharedPointer<ScriptableValue> script_component = script_system->GetFromStack(-1);
      SetMetaTable(script_component, Name());

      return script_component;
    }

    void ScriptRigidBodyComponent::ApplyImpulse(ScriptableArgs* args)
    {
      args->Check("UNNN");
      RigidBodyComponent* component = CheckUserData<RigidBodyComponent>(Name(), args->GetArg(0));

      glm::vec3 impulse = glm::vec3(
        ScriptUtils::As<float>(args->GetArg(1)),
        ScriptUtils::As<float>(args->GetArg(2)),
        ScriptUtils::As<float>(args->GetArg(3))
      );

      component->ForceWake();
      component->ApplyImpulse(impulse);

    }

    void ScriptRigidBodyComponent::ApplyForce(ScriptableArgs* args)
    {
      args->Check("UNNN");
      RigidBodyComponent* component = CheckUserData<RigidBodyComponent>(Name(), args->GetArg(0));

      glm::vec3 force = glm::vec3(
        ScriptUtils::As<float>(args->GetArg(1)),
        ScriptUtils::As<float>(args->GetArg(2)),
        ScriptUtils::As<float>(args->GetArg(3))
      );
      component->ApplyForce(force);
    }

    void ScriptRigidBodyComponent::SetMass(ScriptableArgs* args)
    {
      args->Check("UN");
      RigidBodyComponent* component = CheckUserData<RigidBodyComponent>(Name(), args->GetArg(0));

      float mass = ScriptUtils::As<float>(args->GetArg(1));
      component->SetMass(mass);

    }

    void ScriptRigidBodyComponent::GetLinearVelocity(ScriptableArgs* args)
    {
      args->Check("U");
      RigidBodyComponent* component = CheckUserData<RigidBodyComponent>(Name(), args->GetArg(0));

      glm::vec3 ret = component->GetLinearVelocity();
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }

    void ScriptRigidBodyComponent::SetLinearVelocity(ScriptableArgs* args)
    {
      args->Check("UNNN");
      RigidBodyComponent* component = CheckUserData<RigidBodyComponent>(Name(), args->GetArg(0));

      glm::vec3 velocity = glm::vec3(
        ScriptUtils::As<float>(args->GetArg(1)),
        ScriptUtils::As<float>(args->GetArg(2)),
        ScriptUtils::As<float>(args->GetArg(3))
      );

      component->SetLinearVelocity(velocity);
    }

    void ScriptRigidBodyComponent::GetAngularVelocity(ScriptableArgs* args)
    {
      args->Check("U");
      RigidBodyComponent* component = CheckUserData<RigidBodyComponent>(Name(), args->GetArg(0));

      glm::vec3 ret = component->GetAngularVelocity();
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.x));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.y));
      args->AddReturn(ScriptUtils::Instantiate(args->script_system(), ret.z));
    }

    void ScriptRigidBodyComponent::SetAngularVelocity(ScriptableArgs* args)
    {
      args->Check("UNNN");
      RigidBodyComponent* component = CheckUserData<RigidBodyComponent>(Name(), args->GetArg(0));

      glm::vec3 velocity = glm::vec3(
        ScriptUtils::As<float>(args->GetArg(1)),
        ScriptUtils::As<float>(args->GetArg(2)),
        ScriptUtils::As<float>(args->GetArg(3))
      );

      component->SetAngularVelocity(velocity);
    }
  }
}
