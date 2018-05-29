#include "engine/core/script_debug.h"

#include <foundation/logging/logger.h>

#include <lua-classes/script_debug.lua.cc>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    void ScriptDebug::Log(foundation::String msg)
    {
      PS_LOG(Debug, msg.c_str());
    }

    //--------------------------------------------------------------------------
    void ScriptDebug::LogInfo(foundation::String msg)
    {
      PS_LOG(Info, msg.c_str());
    }

    //--------------------------------------------------------------------------
    void ScriptDebug::LogWarning(foundation::String msg)
    {
      PS_LOG(Warning, msg.c_str());
    }

    //--------------------------------------------------------------------------
    void ScriptDebug::LogError(foundation::String msg)
    {
      PS_LOG(Error, msg.c_str());
    }

    //--------------------------------------------------------------------------
    void ScriptDebug::LogFatal(foundation::String msg)
    {
      PS_LOG(Fatal, msg.c_str());
    }

    //--------------------------------------------------------------------------
    void ScriptDebug::Assert(bool expr, foundation::String msg)
    {
      if (expr == false)
      {
        PS_LOG(Error, msg.c_str());
        assert(false);
      }
    }
  }
}