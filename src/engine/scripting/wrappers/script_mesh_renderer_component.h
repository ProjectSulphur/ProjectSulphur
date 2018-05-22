#pragma once
#include "engine/scripting/scriptable_object.h"
#include "engine/systems/components/mesh_render_system.h"
#include "engine/scripting/script_system.h"
namespace sulphur 
{
  namespace engine 
  {
    class ScriptMeshRendererComponent : public ScriptableObject
    {
    public:
      static foundation::SharedPointer<ScriptableValue>
        Instantiate(ScriptSystem* script_system, MeshRenderComponent mesh_renderer_component);
      /**
      * @brief Get the name of the scriptable class
      * @returns (const char*) the name
      */
      static const char* Name()
      {
        return "MeshRenderComponent";
      }

      /**
      * @brief Get the function register of the scriptable class
      * @returns (foundation::Vector <FunctionRegister>) The function register
      */
      static foundation::Vector<ScriptableFunctionRegister> FunctionRegister()
      {
        return{};
      }

      /**
      * @brief Get the meta table register of the scriptable class
      * @returns (foundation::Vector <FunctionRegister>) The meta table register
      */
      static foundation::Vector<ScriptableFunctionRegister> MetaRegister()
      {
        return
        {
          { "SetMesh" , SetMesh },
          { "GetMesh" , GetMesh },
          { "SetMaterial" , SetMaterial },
          { "GetMaterial" , GetMaterial },
          { "SetMaterials" , SetMaterials },
          { "GetMaterials" , GetMaterials },
          { "SetVisible" , SetVisible },
          { "IsVisible" , IsVisible },
          { "SetCastShadows" , SetCastShadows },
          { "CastsShadows" , CastsShadows }
        };
      }

      static void SetMesh(ScriptableArgs* args);
      static void GetMesh(ScriptableArgs* args);
      static void SetMaterial(ScriptableArgs* args);
      static void GetMaterial(ScriptableArgs* args);
      static void SetMaterials(ScriptableArgs* args);
      static void GetMaterials(ScriptableArgs* args);
      static void SetVisible(ScriptableArgs* args);
      static void IsVisible(ScriptableArgs* args);
      static void SetCastShadows(ScriptableArgs* args);
      static void CastsShadows(ScriptableArgs* args);
    };
  }
}