#pragma once

#include "engine/scripting/scriptable_object.h"

namespace sulphur
{
  namespace engine
  {
    /**
    * @class sulphur::engine::ScriptDebug : public sulphur::engine::ScriptableObject
    * @brief Used to expose debugging functionalities to the script state
    * @author Daniel Konings
    */
    SCRIPT_CLASS() class ScriptDebug : public ScriptableObject
    {

    public:

      SCRIPT_NAME(Debug);

      /**
      * @brief Log a regular message
      * @param[in] msg (sulphur::foundation::String) The message to log
      */
      SCRIPT_FUNC(static) void Log(foundation::String msg);
      
      /**
      * @brief Log an informative message
      * @param[in] msg (sulphur::foundation::String) The message to log
      */
      SCRIPT_FUNC(static) void LogInfo(foundation::String msg);

      /**
      * @brief Log a warning message
      * @param[in] msg (sulphur::foundation::String) The message to log
      */
      SCRIPT_FUNC(static) void LogWarning(foundation::String msg);
      
      /**
      * @brief Log an error message
      * @param[in] msg (sulphur::foundation::String) The message to log
      */
      SCRIPT_FUNC(static) void LogError(foundation::String msg);

      /**
      * @brief Log a fatal message
      * @param[in] msg (sulphur::foundation::String) The message to log
      */
      SCRIPT_FUNC(static) void LogFatal(foundation::String msg);

      /**
      * @brief Asserts an expression and logs a message if the assertion failed
      * @param[in] expr (bool) The expression to evaluate
      * @param[in] msg (foundation::String) The message to log if the assertion failed
      */
      SCRIPT_FUNC(static) void Assert(bool expr, foundation::String msg);

    };
  }
}