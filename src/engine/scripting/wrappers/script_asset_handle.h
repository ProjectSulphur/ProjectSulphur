#pragma once
#include "engine/scripting/scriptable_object.h"
#include "engine/assets/asset_interfaces.h"
#include "engine/scripting/script_system.h"
#include "engine/scripting/script_utils.h"
#include "engine/scripting/scriptable_values/scriptable_args.h"

#include "engine/scripting/wrappers/script_model.h"
#include "engine/scripting/wrappers/script_mesh.h"
#include "engine/scripting/wrappers/script_texture.h"
#include "engine/scripting/wrappers/script_material.h"
#include "engine/scripting/wrappers/script_shader.h"
#include "engine/scripting/wrappers/script_shader_program.h"

namespace sulphur 
{
  namespace engine 
  {
    class ScriptAssetHandle : public ScriptableObject
    {
    public:
      template <typename AssetT>
      static foundation::SharedPointer<ScriptableValue> Instantiate(ScriptSystem* state, const AssetHandle<AssetT>& asset_handle);
      
      template <typename AssetT>
      static AssetHandle<AssetT>* Get(foundation::SharedPointer<ScriptableValue> handle);
 
      static void GetRaw(ScriptableArgs* args)
      {
        args->Check("UN");
        int type = ScriptUtils::As<int>(args->GetArg(1));
        
        switch (type)
        {
        case 1:
          args->AddReturn(ScriptModel::Copy(args->script_system(), *GetRawT(*Get<Model>(args->GetArg(0)))));
          break;
        case 2:
          args->AddReturn(ScriptMesh::Copy(args->script_system(), *GetRawT(*Get<Mesh>(args->GetArg(0)))));
          break;
        case 3:
          args->AddReturn(ScriptShader::Copy(args->script_system(), *GetRawT(*Get<Shader>(args->GetArg(0)))));
          break;
        case 4:
          args->AddReturn(ScriptShaderProgram::Copy(args->script_system(), *GetRawT(*Get<ShaderProgram>(args->GetArg(0)))));
          break;
        case 5:
          args->AddReturn(ScriptMaterial::Copy(args->script_system(), *GetRawT(*Get<Material>(args->GetArg(0)))));
          break;
        case 6:
          args->AddReturn(ScriptTexture::Copy(args->script_system(), *GetRawT(*Get<Texture>(args->GetArg(0)))));
          break;
        }
      }

      template <typename AssetT>
      static AssetT* GetRawT(const AssetHandle<AssetT>& asset_handle);

      inline static void Release(ScriptableArgs*)
      {
        // Commented oout because of memory issue that should be fixed when automating binding generating
        // CheckUserData<BaseAssetHandle>(args->script_system()->lua_state(), Name(), args->GetArg(0))->Release();
      }

      /**
      * @brief Get the name of the scriptable class
      * @returns (const char *) the name
      */
      static const char * Name()
      {
        return "AssetHandle";
      }

      static foundation::Vector<ScriptableFunctionRegister> MetaRegister()
      {
        return
        {
          { "GetRaw", GetRaw },
          { "Release", Release },
          //{ "__gc", Release }
        };
      }
    };


    template <typename AssetT>
    foundation::SharedPointer<ScriptableValue> 
      ScriptAssetHandle::Instantiate(ScriptSystem* state, const AssetHandle<AssetT>& asset_handle)
    {
      AssetHandle<AssetT>* lua_handle 
        = reinterpret_cast<AssetHandle<AssetT>*>(
            lua_newuserdata(
              state->lua_state(), 
              sizeof(AssetHandle<AssetT>)
            )
          );

      lua_handle = new(lua_handle)AssetHandle<AssetT>(asset_handle);
      foundation::SharedPointer<ScriptableValue> ret = state->GetFromStack(-1);
      SetMetaTable(ret, Name());
      return ret;
    }

    template <typename AssetT>
    AssetHandle<AssetT>* 
      ScriptAssetHandle::Get(
        foundation::SharedPointer<ScriptableValue> handle
      )
    {
      return CheckUserData<AssetHandle<AssetT>>(Name(), handle);
    }

    template <typename AssetT>
    AssetT* ScriptAssetHandle::GetRawT(const AssetHandle<AssetT>& asset_handle)
    {
      return asset_handle.GetRaw();
    }
  }
}