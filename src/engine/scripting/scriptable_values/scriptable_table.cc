#include "engine/scripting/scriptable_values/scriptable_table.h"
#include "engine/scripting/script_utils.h"


sulphur::foundation::SharedPointer<sulphur::engine::ScriptableValue>
sulphur::engine::ScriptableTable::Get(foundation::SharedPointer<ScriptableValue> key)
{
  Push();
  key->Push();
  lua_gettable(script_state_->lua_state(), -2);
  foundation::SharedPointer<ScriptableValue> value = script_state_->GetFromStack(-1);
  lua_pop(script_state_->lua_state(), -1);
  return value;
}

//---------------------------------------------------------------------------------------------
sulphur::foundation::SharedPointer<sulphur::engine::ScriptableValue>
sulphur::engine::ScriptableTable::Get(int key)
{
  return Get(ScriptUtils::Instantiate(script_state_, key));
}

//---------------------------------------------------------------------------------------------
sulphur::foundation::SharedPointer<sulphur::engine::ScriptableValue>
sulphur::engine::ScriptableTable::Get(const char* key)
{
  return Get(ScriptUtils::Instantiate(script_state_, key));
}

//---------------------------------------------------------------------------------------------
bool sulphur::engine::ScriptableTable::Contains(foundation::SharedPointer<ScriptableValue> key)
{
  return Get(key)->GetType() != ScriptableValueType::kNil;
}


//---------------------------------------------------------------------------------------------
bool sulphur::engine::ScriptableTable::Contains(const char* key)
{
  return Contains(ScriptUtils::Instantiate(script_state_, key));
}

//---------------------------------------------------------------------------------------------
bool sulphur::engine::ScriptableTable::Contains(int key)
{
  return Contains(ScriptUtils::Instantiate(script_state_, key));
}


//---------------------------------------------------------------------------------------------
void sulphur::engine::ScriptableTable::Insert(foundation::SharedPointer<ScriptableValue> key, foundation::SharedPointer<ScriptableValue> value)
{
  bool is_string = key->GetType() == ScriptableValueType::kString;
  Push();

  if (is_string == true)
  {
    key->Push();
  }

  value->Push();

  if (is_string == true)
  {
    lua_settable(script_state_->lua_state(), -3);
  }
  else
  {
    int idx = ScriptUtils::As<int>(key);
    lua_rawseti(script_state_->lua_state(), -2, idx);
  }

  lua_pop(script_state_->lua_state(), -1);
}

//---------------------------------------------------------------------------------------------
int sulphur::engine::ScriptableTable::GetSize()
{
  Push();

  int size = 0;
  
  lua_pushnil(script_state_->lua_state());

  while (lua_next(script_state_->lua_state(), -2))
  {
    size++;
    lua_pop(script_state_->lua_state(), 1);
  }

  lua_pop(script_state_->lua_state(), -1);

  return size;
}

//---------------------------------------------------------------------------------------------
sulphur::engine::ScriptableTable::ScriptableTable(ScriptState* script_state, bool is_lib)
  : ScriptableValue(script_state, is_lib)
{
  lua_newtable(script_state->lua_state());
  ref_ = luaL_ref(script_state->lua_state(), LUA_REGISTRYINDEX);
}
