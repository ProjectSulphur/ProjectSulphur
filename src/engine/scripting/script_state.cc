#include "script_state.h"

#include "engine/core/component_id.h"
#include "engine/scripting/script_utils.h"
#include "engine/scripting/scriptable_values/scriptable_callback.h"
#include "engine/scripting/scriptable_values/scriptable_function.h"
#include "engine/scripting/scriptable_values/scriptable_args.h"
#include "engine/application/application.h"
#include "engine/systems/component_system.h"

#include "engine/assets/asset_system.h"
#include "engine/assets/script.h"

#include <foundation/memory/memory.h>
#include <foundation/containers/string.h>
#include <foundation/io/binary_reader.h>
#include <foundation/io/filesystem.h>

#include <lua.hpp>

namespace sulphur
{
  namespace engine
  {
    ScriptState::ScriptState() :
      on_initialize_(nullptr),
      update_(nullptr),
      fixed_update_(nullptr),
      include_(nullptr),
      app_(nullptr)
    {
    }
    
    //------------------------------------------------------------------------------
    bool ScriptState::Initialize(Application* app)
    {
      // Note that we are not using custom memory allocation
      // this is because luajit comes with ps4 allocation and 
      // it's garbage collector prevents us from using custom 
      // when compiled 64 bit
      lua_state_ = luaL_newstate();
      luaL_openlibs(lua_state_);

      app_ = app;

      include_ = ScriptUtils::CreateFunction(this, &Include, true);
      RegisterValue("include", include_);
      return true;
    }

    //------------------------------------------------------------------------------
    void ScriptState::Shutdown()
    {
      if(lua_state_)
        lua_close(lua_state_);
    }

    //------------------------------------------------------------------------------
    void ScriptState::RegisterCallbacks()
    {
      on_initialize_ = foundation::Memory::ConstructShared<ScriptableCallback>();

      if (on_initialize_->FromGlobal(this, "OnInitialize") == false)
      {
        ScriptUtils::ScriptLog(
          foundation::Verbosity::kError,
          "@main.lua: does not contain a function 'OnInitialize'"
        );
      }

      update_ = foundation::Memory::ConstructShared<ScriptableCallback>();

      if (update_->FromGlobal(this, "Update") == false)
      {
        ScriptUtils::ScriptLog(
          foundation::Verbosity::kError,
          "@main.lua: does not contain a function 'Update'"
        );
      }

      fixed_update_ = foundation::Memory::ConstructShared<ScriptableCallback>();

      if (fixed_update_->FromGlobal(this, "FixedUpdate") == false)
      {
        ScriptUtils::ScriptLog(
          foundation::Verbosity::kError,
          "@main.lua: does not contain a function 'FixedUpdate'"
        );
      }
    }

    //------------------------------------------------------------------------------
    void ScriptState::OnInitialize(const foundation::String& project_dir)
    {
      if (on_initialize_.get() != nullptr)
      {
        on_initialize_->Call({ ScriptUtils::Instantiate<const char*>(this, project_dir.c_str()) });
      }
    }

    //------------------------------------------------------------------------------
    void ScriptState::Update(float delta_time)
    {
      if (update_.get() != nullptr)
      {
        update_->Call({ ScriptUtils::Instantiate<float>(this, delta_time) });
      }
    }

    //------------------------------------------------------------------------------
    void ScriptState::FixedUpdate()
    {
      if (fixed_update_.get() != nullptr)
      {
        fixed_update_->Call({});
      }
    }

    //------------------------------------------------------------------------------
    void ScriptState::CollectGarbage()
    {
      lua_gc(lua_state_, LUA_GCCOLLECT, 0);
    }

    //------------------------------------------------------------------------------
    bool ScriptState::CompileAndRun(
      const char* data,
      size_t size,
      const char* context)
    {

      foundation::String lua_context = context;
      lua_context = "@" + lua_context;
      int status = luaL_loadbuffer(lua_state_, data, size, lua_context.c_str());

      if (status) {
        ScriptUtils::ScriptLog(
            foundation::Verbosity::kError,
            foundation::String(lua_tostring(lua_state_, -1))
        );

        return false;
      }

      status = lua_pcall(lua_state_, 0, LUA_MULTRET, 0);

      if (status) {
        ScriptUtils::ScriptLog(
            foundation::Verbosity::kError,
            foundation::String(lua_tostring(lua_state_, -1))
          );
        return false;
      }
    
      return true;
    }

    //------------------------------------------------------------------------------
    void ScriptState::Include(ScriptableArgs* args)
    {
      args->Check("S");

      const char* module_name = ScriptUtils::As<const char*>(args->GetArg(0));
      sulphur::foundation::String lua_context = foundation::String(module_name) + ".lua";
      lua_context = "@" + lua_context;

      AssetHandle<Script> script = AssetSystem::Instance().Load<Script>(module_name);
      foundation::Vector<char> bin = script.GetRaw()->binary;


      int status = luaL_loadbuffer(
        args->script_state()->lua_state(), 
        bin.data(), bin.size(),
        lua_context.c_str()
      );

      if (status) {
        ScriptUtils::ScriptLog(
          foundation::Verbosity::kError,
          foundation::String(lua_tostring(args->script_state()->lua_state(), -1))
        );

        return;
      }

      status = lua_pcall(args->script_state()->lua_state(), 0, LUA_MULTRET, 0);

      if (status) {
        ScriptUtils::ScriptLog(
          foundation::Verbosity::kError,
          foundation::String(lua_tostring(args->script_state()->lua_state(), -1))
        );
        return;
      }
      
      int return_count = lua_gettop(args->script_state()->lua_state());

      for (int i = 1; i <= return_count; i++)
      {
        foundation::SharedPointer<ScriptableValue> arg
          = foundation::Memory::ConstructShared<ScriptableValue>(args->script_state(), i);
        args->AddReturn(arg);
      }
    }

    //------------------------------------------------------------------------------
    void ScriptState::RegisterValue(
      const char * name, 
      foundation::SharedPointer<ScriptableValue> value
    )
    {
      foundation::Map<
        foundation::String,
        foundation::SharedPointer<ScriptableValue>
      >::iterator it = globals_.find(name);
      if (value->global_ == true || it != globals_.end())
      {
        PS_LOG(Error, "Trying to register a global value that already exist, use GetValue and overwrite that instead");
        return;
      }
      else if (value->script_state()->lua_state() != lua_state_)
      {
        PS_LOG(Error, "Trying to register a global value that was created in a different script state");
        return;
      }

      value->Push();
      lua_setglobal(value->script_state()->lua_state(), name);

      lua_unref(lua_state_, value->ref_);
      value->global_ = true;
      value->key_ = name;
      value->ref_ = LUA_REFNIL;
      globals_.insert({ {name, value} });
    }

    //------------------------------------------------------------------------------
    sulphur::foundation::SharedPointer<ScriptableValue> ScriptState::GetValue(const char* name)    
    {
      foundation::Map<
        foundation::String, 
        foundation::SharedPointer<ScriptableValue>
      >::iterator it 
        = globals_.find(name);
      if (it != globals_.end())
      {
        return it->second;
      }
      else
      {
        return foundation::Memory::ConstructShared<ScriptableValue>(this, name);
      }
    }

    //------------------------------------------------------------------------------
    sulphur::foundation::SharedPointer<ScriptableValue> ScriptState::GetFromStack(int idx)
    {
      return foundation::Memory::ConstructShared<ScriptableValue>(this, idx);
    }

    //------------------------------------------------------------------------------
    void ScriptState::RegisterMetatable(int id, size_t size, const foundation::String& name)
    {
      metatables_.insert({ {id, {size, name} } });
    }

    //------------------------------------------------------------------------------
    void ScriptState::RegisterComponentID(foundation::typeid_t id, ComponentIDBase* base)
    {
      component_ids_.insert({ id, base });
    }

    //------------------------------------------------------------------------------
    ComponentIDBase* ScriptState::GetComponentIDRegister(foundation::typeid_t id)
    {
      return component_ids_.at(id);
    }

    //------------------------------------------------------------------------------
    ScriptState::Metatable ScriptState::GetMetatable(int id)
    {
      return metatables_.at(id);
    }

    //------------------------------------------------------------------------------
    ScriptState::~ScriptState()
    {
      for (foundation::Map<foundation::typeid_t, ComponentIDBase*>::iterator it =
        component_ids_.begin();
        it != component_ids_.end();
        ++it)
      {
        foundation::Memory::Destruct(it->second);
      }
    }
  }
}
