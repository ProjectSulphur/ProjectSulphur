#pragma once
#include "engine/scripting/scriptable_object.h"
#include "engine/assets/material.h"
namespace sulphur 
{
  namespace engine 
  {
    class ScriptMaterial : public ScriptableObject
    {
    public:
      static void New(ScriptableArgs* args);
      static foundation::SharedPointer<ScriptableValue>
        Copy(ScriptSystem* script_system, const Material& material);
      /**
      * @brief Get the name of the scriptable class
      * @returns (const char *) the name
      */
      static const char * Name()
      {
        return "Material";
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

      static foundation::Vector<ScriptableFunctionRegister> MetaRegister()
      {
        return
        {
          {"material_passes", material_passes },
          {"AddMaterialPass", AddMaterialPass },
          {"GetMaterialPass", GetMaterialPass },
          {"SetMaterialPass", SetMaterialPass },
          {"RemoveMaterialPass", RemoveMaterialPass },
          {"NumMaterialPasses ", NumMaterialPasses }
        };
      }

      static void material_passes(ScriptableArgs* args);
      static void AddMaterialPass(ScriptableArgs* args);
      static void GetMaterialPass(ScriptableArgs* args);
      static void SetMaterialPass(ScriptableArgs* args);
      static void RemoveMaterialPass(ScriptableArgs* args);
      static void NumMaterialPasses(ScriptableArgs* args);
    };


  }
}