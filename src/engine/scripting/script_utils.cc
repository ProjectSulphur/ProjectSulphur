#include "engine/scripting/script_utils.h"
#include "engine/systems/component_system.h"

#include "engine/scripting/scriptable_values/scriptable_table.h"
namespace sulphur 
{
  namespace engine 
  {
    foundation::SharedPointer<ScriptableValue> ScriptUtils::CreateFunction(ScriptState* state, ScriptFunction function, bool is_lib)
    {
      return foundation::Memory::ConstructShared<ScriptableFunction>(state, function, is_lib);
    }

    //------------------------------------------------------------------------------
    foundation::SharedPointer<ScriptableTable> ScriptUtils::CreateTable(ScriptState* state)
    {
      return foundation::Memory::ConstructShared<ScriptableTable>(
        &foundation::Memory::default_allocator(), 
        state);
    }

    //------------------------------------------------------------------------------
    foundation::SharedPointer<ScriptableTable> ScriptUtils::GetTable(foundation::SharedPointer<ScriptableValue> table)
    {
      if (table->GetType() != ScriptableValueType::kObject)
      {
        PS_LOG(Error, "Tried to get scriptable value as a table but it's not a table");
        return nullptr;
      }
       
      return eastl::static_pointer_cast<ScriptableTable, ScriptableValue>(table);
    }

    //------------------------------------------------------------------------------
    ScriptHandle ScriptUtils::InstantiateType(
      ScriptState* state, int id, void* handle)
    {
      ScriptState::Metatable mt = state->GetMetatable(id);
      void* mem = lua_newuserdata(state->lua_state(), mt.size);
      memcpy(mem, handle, mt.size);

      foundation::SharedPointer<ScriptableValue> ud = state->GetFromStack(-1);
      ScriptableObject::SetMetaTable(ud, mt.name.c_str());

      return ud;
    }

    //------------------------------------------------------------------------------
    ScriptableValueType ScriptUtils::GetType(foundation::SharedPointer<ScriptableValue> value)
    {
      return value->GetType();
    }

    //------------------------------------------------------------------------------
    foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate(ScriptState* state)
    {
      return foundation::Memory::ConstructShared<ScriptableValue>(state);
    }
  }
}