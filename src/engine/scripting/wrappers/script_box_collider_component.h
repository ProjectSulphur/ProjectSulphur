#pragma once
#include "engine/scripting/scriptable_object.h"
#include "engine/systems/components/collider_system.h"
#include "engine/scripting/script_system.h"
namespace sulphur 
{
  namespace engine 
  {
    class ScriptBoxColliderComponent : public ScriptableObject
    {
    public:
      static foundation::SharedPointer<ScriptableValue>
        Instantiate(ScriptSystem* script_system, BoxColliderComponent box_collider_component);
      /**
      * @brief Get the name of the scriptable class
      * @returns (const char *) the name
      */
      static const char* Name()
      {
        return "BoxColliderComponent";
      }

      /**
      * @brief Get the function register of the scriptable class
      * @returns (sulphur::foundation::Vector <sulphur::engine::FunctionRegister>) The function register
      */
      static foundation::Vector<ScriptableFunctionRegister> FunctionRegister()
      {
        return {};
      }

      /**
      * @brief Get the meta table register of the scriptable class
      * @returns (sulphur::foundation::Vector <sulphur::engine::FunctionRegister>) The meta table register
      */
      static foundation::Vector<ScriptableFunctionRegister> MetaRegister()
      {
        return
        {
          { "SetExtents", SetExtents },
          { "Extents", Extents }
        };
      }

     static void SetExtents(ScriptableArgs* args);
     static void Extents(ScriptableArgs* args);
    };
  }
}

