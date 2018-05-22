#pragma once
#include "engine/scripting/scriptable_object.h"
#include "engine/assets/shader.h"
namespace sulphur 
{
  namespace engine 
  {
    class ScriptShaderProgram : public ScriptableObject
    {
    public:
      static void New(ScriptableArgs* args);


      static foundation::SharedPointer<ScriptableValue>
        Copy(ScriptSystem* script_system, const ShaderProgram& shader);

      /**
      * @brief Get the name of the scriptable class
      * @returns (const char *) the name
      */
      static const char * Name()
      {
        return "ShaderProgram";
      }

      /**
      * @brief Get the function register of the scriptable class
      * @returns (sulphur::foundation::Vector <sulphur::engine::FunctionRegister>) The function register
      */
      static foundation::Vector<ScriptableFunctionRegister> FunctionRegister()
      {
        return
        {
          { "New", New }
        };
      }
    };


  }
}