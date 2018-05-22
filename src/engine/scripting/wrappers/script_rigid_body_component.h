#pragma once
#include "engine/scripting/scriptable_object.h"
#include "engine/systems/components/rigid_body_system.h"
#include "engine/scripting/script_system.h"
namespace sulphur 
{
  namespace engine 
  {
    class ScriptRigidBodyComponent : public ScriptableObject
    {
    public:
      static foundation::SharedPointer<ScriptableValue>
        Instantiate(ScriptSystem* script_system, RigidBodyComponent rigidbody_component);
      /**
      * @brief Get the name of the scriptable class
      * @returns (const char *) the name
      */
      static const char * Name()
      {
        return "RigidBodyComponent";
      }

      /**
      * @brief Get the function register of the scriptable class
      * @returns (sulphur::foundation::Vector <sulphur::engine::FunctionRegister>) The function register
      */
      static foundation::Vector<ScriptableFunctionRegister> FunctionRegister()
      {
        return{};
      }

      /**
      * @brief Get the meta table register of the scriptable class
      * @returns (sulphur::foundation::Vector <sulphur::engine::FunctionRegister>) The meta table register
      */
      static foundation::Vector<ScriptableFunctionRegister> MetaRegister()
      {
        return
        {
          { "ApplyImpulse" , ApplyImpulse },
          { "ApplyForce", ApplyForce },
          { "SetMass", SetMass },
          { "GetLinearVelocity", GetLinearVelocity },
          { "SetLinearVelocity", SetLinearVelocity },
          { "GetAngularVelocity", GetAngularVelocity },
          { "SetAngularVelocity", SetAngularVelocity }
        };
      }

      static void ApplyImpulse(ScriptableArgs* args);
      static void ApplyForce(ScriptableArgs* args);
      static void SetMass(ScriptableArgs* args);
      static void GetLinearVelocity(ScriptableArgs* args);
      static void SetLinearVelocity(ScriptableArgs* args);
      static void GetAngularVelocity(ScriptableArgs* args);
      static void SetAngularVelocity(ScriptableArgs* args);
    };
  }
}

