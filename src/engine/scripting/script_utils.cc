#include "script_utils.h"

namespace sulphur 
{
  namespace engine 
  {
    foundation::SharedPointer<ScriptableValue> ScriptUtils::CreateFunction(ScriptSystem* state, ScriptFunction function, bool is_lib)
    {
      return foundation::Memory::ConstructShared<ScriptableFunction>(state, function, is_lib);
    }

    //------------------------------------------------------------------------------
    foundation::SharedPointer<ScriptableValue> ScriptUtils::CreateTable(ScriptSystem* state)
    {
      return foundation::Memory::ConstructShared<ScriptableTable>(
        &foundation::Memory::default_allocator(), 
        state
        );
    }

    //------------------------------------------------------------------------------
    void ScriptUtils::SetObjectValue(
      foundation::SharedPointer<ScriptableValue> object, 
      foundation::SharedPointer<ScriptableValue>  key, 
      foundation::SharedPointer<ScriptableValue>  value
    )
    {
      object->Push();
      key->Push();
      value->Push();

      lua_settable(object->script_system()->lua_state(), -3);

      lua_pop(object->script_system()->lua_state(), -1);
    }

    foundation::Map<
      foundation::SharedPointer<ScriptableValue>, 
      foundation::SharedPointer<ScriptableValue>
    > ScriptUtils::GetObject(foundation::SharedPointer<ScriptableValue> value)
    {
      value->Push();

      foundation::Map<
        foundation::SharedPointer<ScriptableValue>, foundation::SharedPointer<ScriptableValue>
      > result = foundation::Map<
        foundation::SharedPointer<ScriptableValue>,
        foundation::SharedPointer<ScriptableValue>
      >();

      lua_pushnil(value->script_system()->lua_state());

      while (lua_next(value->script_system()->lua_state(), -2))
      {

        foundation::SharedPointer<ScriptableValue> key =
          foundation::Memory::ConstructShared<ScriptableValue>(value->script_system(), -2);
        foundation::SharedPointer<ScriptableValue> mvalue =
          foundation::Memory::ConstructShared<ScriptableValue>(value->script_system(), -1);

        result.insert({ { key, mvalue } });

        lua_pop(value->script_system()->lua_state(), 1);
      }

      lua_pop(value->script_system()->lua_state(), -1);

      return result;
    }

    //------------------------------------------------------------------------------
    ScriptableValueType ScriptUtils::GetType(foundation::SharedPointer<ScriptableValue> value)
    {
      return value->GetType();
    }

    //------------------------------------------------------------------------------
    foundation::SharedPointer<ScriptableValue> ScriptUtils::Instantiate(ScriptSystem* state)
    {
      return foundation::Memory::ConstructShared<ScriptableValue>(state);
    }
  }
}