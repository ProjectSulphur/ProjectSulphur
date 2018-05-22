#pragma once
#include "engine/scripting/scriptable_object.h"
#include "engine/assets/model.h"
namespace sulphur 
{
  namespace engine 
  {
    class ScriptModel : public ScriptableObject
    {
    public:
      static void New(ScriptableArgs* args);

      static foundation::SharedPointer<ScriptableValue> Copy(ScriptSystem* script_system, const Model& mesh);

      /**
      * @brief Get the name of the scriptable class
      * @returns (const char *) the name
      */
      static const char* Name()
      {
        return "Model";
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

      /**
      * @brief Get the meta table register of the scriptable class
      * @returns (sulphur::foundation::Vector <sulphur::engine::FunctionRegister>) The meta table register
      */
      static foundation::Vector<ScriptableFunctionRegister> MetaRegister()
      {
        return
        {
          { "mesh_handle", mesh_handle },
          { "material_handles", material_handles },
          { "GetMaterialHandle", GetMaterialHandle },
          { "GetMaterialHandlesSize", GetMaterialHandlesSize }
        };
      }

      static void mesh_handle(ScriptableArgs* args);
      static void material_handles(ScriptableArgs* args);
      static void GetMaterialHandle(ScriptableArgs* args);
      static void GetMaterialHandlesSize(ScriptableArgs* args);

    };


  }
}