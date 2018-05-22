#pragma once
#include "engine/scripting/scriptable_object.h"
#include "engine/assets/texture.h"
namespace sulphur 
{
  namespace engine 
  {
    class ScriptTexture : public ScriptableObject
    {
    public:
      static void New(ScriptableArgs* args);
      static foundation::SharedPointer<ScriptableValue>
        Copy(ScriptSystem* script_system, const Texture& material);
      /**
      * @brief Get the name of the scriptable class
      * @returns (const char *) the name
      */
      static const char * Name()
      {
        return "Texture";
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