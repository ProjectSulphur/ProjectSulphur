#pragma once 
#include "engine/scripting/scriptable_object.h"
#include "engine/input/input.h"

namespace sulphur 
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::ScriptInput : sulphur::engine::ScriptableObject
    * @brief Wraps Input to be exposed to Lua
    * @author Rodi Lankester
    */
    class ScriptInput : public ScriptableObject
    {
    public:
      /**
      * @brief Get if a key is going down from Lua
      * @param[in] args (sulphur::engine::ScriptableArgs*) The arguments passed from Lua
      */
      static void GetButtonFalling(ScriptableArgs* args);
      /**
      * @brief Get if a key is going up from Lua
      * @param[in] args (sulphur::engine::ScriptableArgs*) The arguments passed from Lua
      */
      static void GetButtonRising(ScriptableArgs* args);
      /**
      * @brief Get if a key is down from Lua
      * @param[in] args (sulphur::engine::ScriptableArgs*) The arguments passed from Lua
      */
      static void GetButtonDown(ScriptableArgs* args);
      /**
      * @brief Get the mouse position from Lua
      * @param[in] args (sulphur::engine::ScriptableArgs*) The arguments passed from Lua
      */
      static void GetMousePosition(ScriptableArgs* args);
      /**
      * @brief Get the mouse offset from Lua
      * @param[in] args (sulphur::engine::ScriptableArgs*) The arguments passed from Lua
      */
      static void GetMouseOffset(ScriptableArgs* args);
      /**
      * @brief Get the scroll offset from Lua
      * @param[in] args (sulphur::engine::ScriptableArgs*) The arguments passed from Lua
      */
      static void GetScrollOffset(ScriptableArgs* args);

      /**
      * @brief Set the input to get current input data from
      * @param[in] input (sulphur::engine::Input&) The input
      */
      static void SetInput(Input& input);

      /**
      * @see sulphur::engine::ScriptableObject::Name
      */
      static const char * Name()
      {
        return "Input";
      }

      /**
      * @see sulphur::engine::ScriptableObject::FunctionRegister
      */
      static foundation::Vector<ScriptableFunctionRegister> FunctionRegister()
      {
        return{ 
           { "GetButtonFalling", GetButtonFalling },
           { "GetButtonDown", GetButtonDown },
           { "GetButtonRising", GetButtonRising },
           { "GetMousePosition", GetMousePosition },
           { "GetMouseOffset", GetMouseOffset },
           { "GetScrollOffset", GetScrollOffset }
          };
      }
    protected:
      static Input* input_;
    };
  }
}