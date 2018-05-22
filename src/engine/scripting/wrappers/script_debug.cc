#include "script_debug.h"

#include "engine/scripting/wrappers/script_input.h"
#include "engine/scripting/script_utils.h"
#include "engine/scripting/scriptable_values/scriptable_args.h"

#include <string>

namespace sulphur
{
  namespace engine
  {
    void ScriptDebug::StackDump(ScriptableArgs* args)
    {
      lua_State* L = args->script_system()->lua_state();
      args->Check("");
      int i = lua_gettop(L);
      printf("[Lua] ----------------  Stack Dump ----------------\n");
      while (i) {
        int t = lua_type(L, i);
        switch (t) {
        case LUA_TSTRING:
          printf("[Lua] %d:`%s'\n", i, lua_tostring(L, i));
          break;
        case LUA_TBOOLEAN:
          printf("[Lua] %d: %s\n", i, lua_toboolean(L, i) ? "true" : "false");
          break;
        case LUA_TNUMBER:
          printf("[Lua] %d: %g\n", i, lua_tonumber(L, i));
          break;
        default: printf("[Lua] %d: %s\n", i, lua_typename(L, t)); break;
        }
        i--;
      }
      printf("[Lua] --------------- Stack Dump Finished ---------------\n");
    }

    //------------------------------------------------------------------------------
    void ScriptDebug::Log(ScriptableArgs* args)
    {
      lua_Debug ar;
      lua_getstack(args->script_system()->lua_state(), 1, &ar);
      lua_getinfo(args->script_system()->lua_state(), "S", &ar);
      const char* sourceFile = ar.source;

      args->Check("S", 1);
      foundation::String input = args->GetArgAs<const char*>(0);
      foundation::String output = "";

      int arg = 1;
      for (foundation::String::iterator it = input.begin(); it != input.end(); ++it) {
        if ((*it) == '%')
        {
          ++it;
          switch (*it)
          {
          case 's':
            output += ScriptUtils::As<const char*>(args->GetArg(arg));
            break;
          case 'd':
          case 'f':
            output += std::to_string(ScriptUtils::As<double>(args->GetArg(arg))).c_str();
            break;
          case 'b':
            output += ScriptUtils::As<bool>(args->GetArg(arg)) ? "true" : "false";
            break;
          default:
            output += lua_typename(args->script_system()->lua_state(), static_cast<int>(ScriptUtils::GetType(args->GetArg(arg))));
          }
          ++arg;
        }
        else
        {
          output += *it;
        }
      }

      ScriptUtils::ScriptLog(foundation::Verbosity::kDebug, "[%s] %s", sourceFile, output.c_str());
    }

    //------------------------------------------------------------------------------
    void ScriptDebug::ErrorLog(ScriptableArgs* args)
    {
      lua_Debug ar;
      lua_getstack(args->script_system()->lua_state(), 1, &ar);
      lua_getinfo(args->script_system()->lua_state(), "S", &ar);
      const char* sourceFile = ar.source;

      args->Check("S", 1);
      foundation::String input = args->GetArgAs<const char*>(0);
      foundation::String output = "";

      int arg = 1;
      for (foundation::String::iterator it = input.begin(); it != input.end(); ++it) {
        if ((*it) == '%')
        {
          ++it;
          switch (*it)
          {
          case 's':
            output += ScriptUtils::As<const char*>(args->GetArg(arg));
            break;
          case 'd':
          case 'f':
            output += std::to_string(ScriptUtils::As<double>(args->GetArg(arg))).c_str();
            break;
          case 'b':
            output += ScriptUtils::As<bool>(args->GetArg(arg)) ? "true" : "false";
            break;
          default:
            output += lua_typename(args->script_system()->lua_state(), static_cast<int>(ScriptUtils::GetType(args->GetArg(arg))));
          }
          ++arg;
        }
        else
        {
          output += *it;
        }
      }

      ScriptUtils::ScriptLog(foundation::Verbosity::kError, "[%s] %s", sourceFile, output.c_str());
    }

  }
}
