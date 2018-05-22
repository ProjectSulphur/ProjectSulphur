#pragma once 
#include "engine/scripting/scriptable_object.h"
#include "engine/scripting/script_utils.h"
#include "engine/core/entity_system.h"
#include "foundation/containers/map.h"
#include "engine/core/handle_base.h"
namespace sulphur 
{
  namespace engine 
  {
    class ScriptEntity : public ScriptableObject
    {
    public:
      static void New(ScriptableArgs* args);
      static void Destroy(ScriptableArgs* args);

      static foundation::SharedPointer<ScriptableValue> Instantiate(ScriptSystem* script_system, const Entity& entity);

      static void RegisterComponentList(ScriptSystem* state);

      static void Add(ScriptableArgs* args);
      static void Get(ScriptableArgs* args);
      static void Remove(ScriptableArgs* args);
      static void Has(ScriptableArgs* args);
      /**
      * @brief Get the name of the scriptable class
      * @returns (const char*) the name
      */
      static const char* Name()
      {
        return "Entity";
      }

      /**
      * @brief Get the function register of the scriptable class
      * @returns (sulphur::foundation::Vector <sulphur ::engine::FunctionRegister>) The function register
      */
      static foundation::Vector<ScriptableFunctionRegister> FunctionRegister()
      {
        return{ { "New", New} };
      }

      /**
      * @brief Get the meta table register of the scriptable class
      * @returns (sulphur::foundation::Vector <sulphur::engine::FunctionRegister>) The meta table register
      */
      static foundation::Vector<ScriptableFunctionRegister> MetaRegister()
      {
        return{ 
          { "Get", Get }, 
          { "Add", Add},
          { "Remove", Remove},
          { "Has", Has}
        };
      }

      static void SetEntitySystem(EntitySystem& system)
      {
        entity_system_ = &system;
      }

    protected:
      static const char* components_[];

      static EntitySystem* entity_system_;
    };

  }
}