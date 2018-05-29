#include "engine/scripting/scriptable_values/scriptable_args.h"
#include "engine/scripting/script_utils.h"

sulphur::engine::ScriptableArgs::ScriptableArgs(
  foundation::Vector<sulphur::foundation::SharedPointer<sulphur::engine::ScriptableValue>>& args,
  ScriptState* script_state) :
  args_(args)
{
  script_state_ = script_state;
}

//------------------------------------------------------------------------------
void sulphur::engine::ScriptableArgs::AddReturn(foundation::SharedPointer<ScriptableValue> return_value)
{
  return_values_.push_back(return_value);
}

//------------------------------------------------------------------------------
sulphur::engine::ScriptState* sulphur::engine::ScriptableArgs::script_state()
{
  return script_state_;
}

//------------------------------------------------------------------------------
int sulphur::engine::ScriptableArgs::NumArgs()
{
  return static_cast<int>(args_.size());
}

//------------------------------------------------------------------------------
void ErrorIfUnexpected(
  sulphur::engine::ScriptableValueType type,
  sulphur::engine::ScriptableValueType expected,
  lua_State* lua_state,
  const char * function_name,
  size_t arg)
{
  if (type != expected)
  {
    if (
      (type == sulphur::engine::ScriptableValueType::kNumber &&
      expected == sulphur::engine::ScriptableValueType::kUserdata) ||
      (type == sulphur::engine::ScriptableValueType::kObject &&
        expected == sulphur::engine::ScriptableValueType::kUserdata)
      )
    {
      return;
    }
    
    if (type != sulphur::engine::ScriptableValueType::kString)
    {
      luaL_error(
        lua_state,
        "function %s argument %d expected %s but got %s",
        function_name,
        arg,
        lua_typename(lua_state, static_cast<int>(expected)),
        lua_typename(lua_state, static_cast<int>(type))
      );
    }
  }
}

//------------------------------------------------------------------------------
void sulphur::engine::ScriptableArgs::Check(const char* format, int check_args)
{
  lua_State* lua_state = script_state_->lua_state();
  size_t size = strlen(format);

  lua_Debug function_info = lua_Debug();
  lua_getstack(lua_state, 0, &function_info);
  lua_getinfo(lua_state, "n", &function_info);
  const char* function_name = function_info.name;

  if (size != NumArgs() && check_args == -1)
  {
    luaL_error(
      lua_state,
      "function %s expected %d arguments but got %d\n",
      function_name,
      size,
      NumArgs()
    );
  }

  if (NumArgs() < check_args)
  {
    luaL_error(
      lua_state,
      "function %s expected at least %d arguments but got %d\n",
      function_name,
      size,
      NumArgs()
    );
  }

  if (check_args != -1)
  {
    size = check_args;
  }

  for (size_t i = 0; i < size; ++i)
  {
    int fmt = toupper(format[i]);
    ScriptableValueType type = ScriptUtils::GetType(args_[i]);
    switch (fmt)
    {
    case 'S':
      ErrorIfUnexpected(type, ScriptableValueType::kString, lua_state, function_name, i);
      break;
    case 'N':
      ErrorIfUnexpected(type, ScriptableValueType::kNumber, lua_state, function_name, i);
      break;
    case 'B':
      ErrorIfUnexpected(type, ScriptableValueType::kBoolean, lua_state, function_name, i);
      break;
    case 'O':
      ErrorIfUnexpected(type, ScriptableValueType::kObject, lua_state, function_name, i);
      break;
    case 'U':
      ErrorIfUnexpected(type, ScriptableValueType::kUserdata, lua_state, function_name, i);
      break;
    case 'T':
      ErrorIfUnexpected(type, ScriptableValueType::kThread, lua_state, function_name, i);
      break;
    case 'F':
      ErrorIfUnexpected(type, ScriptableValueType::kFunction, lua_state, function_name, i);
      break;
    }
  }
}

//------------------------------------------------------------------------------
sulphur::foundation::SharedPointer<sulphur::engine::ScriptableValue>
sulphur::engine::ScriptableArgs::GetArg(size_t idx)
{
  return args_[idx];
}

//------------------------------------------------------------------------------
void sulphur::engine::ScriptableArgs::Return()
{
  for (sulphur::foundation::SharedPointer<sulphur::engine::ScriptableValue> ret : return_values_)
  {
    ret->Push();
  }
}

//------------------------------------------------------------------------------
int sulphur::engine::ScriptableArgs::ReturnCount()
{
  return static_cast<int>(return_values_.size());
}