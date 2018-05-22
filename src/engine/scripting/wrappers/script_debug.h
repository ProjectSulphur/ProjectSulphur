#pragma once 
#include "engine/scripting/scriptable_object.h"

namespace sulphur
{
  namespace engine 
  {

    class ScriptDebug : public ScriptableObject
    {
    public:
      
      static void StackDump(ScriptableArgs* args);
      static void Log(ScriptableArgs* args);
      static void ErrorLog(ScriptableArgs* args);
      /**
      * @see sulphur::engine::ScriptableObject::Name
      */
      static const char* Name()
      {
        return "Debug";
      }

      /**
      * @see sulphur::engine::ScriptableObject::FunctionRegister
      */
      static foundation::Vector<ScriptableFunctionRegister> FunctionRegister()
      {
        return{
          { "StackDump", StackDump },
          { "Log", Log },
          { "ErrorLog", ErrorLog }
        };
      }
    };

    
  }
}