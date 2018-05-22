#pragma once
#include "engine/scripting/scriptable_object.h"
#include "engine/assets/material.h"
namespace sulphur 
{
  namespace engine 
  {
    class ScriptMaterialPass : public ScriptableObject
    {
    public:
      static void New(ScriptableArgs* args);
      static foundation::SharedPointer<ScriptableValue>
        Copy(ScriptSystem* script_system, const MaterialPass& material_pass);

      /**
      * @brief Get the name of the scriptable class
      * @returns (const char *) the name
      */
      static const char * Name()
      {
        return "MaterialPass";
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
          { "SetShader" , SetShader },
          { "shader" , shader },
          { "textures" , textures },
          { "GetTexture" , GetTexture },
          { "SetTexture" , SetTexture },
          { "NumTextures" , NumTextures }
        };
      }
      static void SetShader(ScriptableArgs* args);
      static void shader(ScriptableArgs* args);
      static void textures(ScriptableArgs* args);
      static void GetTexture(ScriptableArgs* args);
      static void SetTexture(ScriptableArgs* args);
      static void NumTextures(ScriptableArgs* args);
    };


  }
}