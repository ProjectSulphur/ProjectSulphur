#pragma once
#include "engine/scripting/scriptable_object.h"
#
namespace sulphur 
{
  namespace engine 
  {
    class ScriptAssetSystem : public ScriptableObject
    {
    public:
      static const char * AssetTypes[];

      static void RegisterEnum(ScriptSystem* script_system);

      static void AddAsset(ScriptableArgs* args);
      static void GetHandle(ScriptableArgs* args);
      static void Load(ScriptableArgs* args);
      static void Release(ScriptableArgs* args);

      static const char * Name()
      {
        return "AssetSystem";
      }

      static foundation::Vector<ScriptableFunctionRegister> FunctionRegister()
      {
        return
        {
          { "AddAsset", AddAsset },
          { "GetHandle", GetHandle },
          { "Load", Load },
          { "Release", Release }
        };
      }
    };
  }
}