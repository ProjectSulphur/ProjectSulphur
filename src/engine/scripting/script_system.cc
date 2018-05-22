#include "engine/scripting/script_system.h"
#include "engine/scripting/script_utils.h"
#include "engine/scripting/scriptable_values/scriptable_callback.h"

#include <foundation/memory/memory.h>
#include <foundation/containers/string.h>
#include <lua.hpp>

// Uncomment this if you want to try and use custom memory allocation
//void *l_alloc(void *ud, void *ptr, size_t osize, size_t nsize)
//{
//	sulphur::foundation::Memory memory = foundation::Memory();
//	//printf("nsize: %i", nsize);
//	if (nsize == 0 && ptr == 0)
//	{
//		return nullptr;
//	}
//	if (nsize == 0)
//	{
//		memory.Deallocate(ptr);
//		return nullptr;
//	}
//	else if (ptr == nullptr)
//	{
//		void * ptr = memory.Allocate(&memory.DefaultAllocator(), nsize);
//		return ptr;
//	}
//	else if (osize >= nsize)
//	{
//		void *p = memory.Allocate(&memory.DefaultAllocator(), nsize);
//		memcpy(p, ptr, nsize);
//		memory.Deallocate(ptr);
//		return ptr;
//	}
//	else
//	{
//		void *p = memory.Allocate(&memory.DefaultAllocator(), nsize);
//		memcpy(p, ptr, osize);
//		memory.Deallocate(ptr);
//		return ptr;
//	}
//}

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------
    ScriptSystem::ScriptSystem()
      : IServiceSystem("ScriptSystem")
    {
    }

    //------------------------------------------------------------------------------
    void ScriptSystem::OnInitialize(Application& app, foundation::JobGraph&)
    {
      application_ = &app;

      // Note that we are not using custom memory allocation
      // this is because luajit comes with ps4 allocation and 
      // it's garbage collector prevents us from using custom 
      // when compiled 64 bit
      lua_state_ = luaL_newstate();
      luaL_openlibs(lua_state_);
    }

    //------------------------------------------------------------------------------
    void ScriptSystem::OnTerminate()
    {
      if (lua_state_)
      {
        lua_close(lua_state_);
      }
    }

    //------------------------------------------------------------------------------
    void ScriptSystem::OnUpdate(float delta_time)
    {
      if (update_.get() != nullptr)
      {
        update_->Call({ ScriptUtils::Instantiate<float>(this, delta_time) });
      }
    }

    //------------------------------------------------------------------------------
    void ScriptSystem::OnFixedUpdate()
    {
      if (fixed_update_.get() != nullptr)
      {
        fixed_update_->Call({});
      }
    }

    //------------------------------------------------------------------------------
    void ScriptSystem::RegisterCallbacks()
    {

    }

    //------------------------------------------------------------------------------
    void ScriptSystem::CollectGarbage()
    {
      lua_gc(lua_state_, LUA_GCCOLLECT, 0);
    }

    //------------------------------------------------------------------------------
    bool ScriptSystem::CompileAndRun(
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

      update_ = foundation::Memory::ConstructShared<ScriptableCallback>();

      if (update_->FromGlobal(this, "Update") == false)
      {
        ScriptUtils::ScriptLog(
          foundation::Verbosity::kError,
          foundation::String(context) + ": does not contain a function update"
        );
      }

      fixed_update_ = foundation::Memory::ConstructShared<ScriptableCallback>();

      if (fixed_update_->FromGlobal(this, "FixedUpdate") == false)
      {
        ScriptUtils::ScriptLog(
          foundation::Verbosity::kError,
          foundation::String(context) + ": does not contain a function fixed update"
        );
      }

      return true;
    }

    //------------------------------------------------------------------------------
    void ScriptSystem::RegisterValue(
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
      else if (value->script_system()->lua_state() != lua_state_)
      {
        PS_LOG(Error, "Trying to register a global value that was created in a different script state");
        return;
      }

      value->Push();
      lua_setglobal(value->script_system()->lua_state(), name);

      lua_unref(lua_state_, value->ref_);
      value->global_ = true;
      value->key_ = name;
      value->ref_ = LUA_REFNIL;
      globals_.insert({ {name, value} });
    }

    //------------------------------------------------------------------------------
    sulphur::foundation::SharedPointer<ScriptableValue> ScriptSystem::GetValue(const char* name)    
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
    sulphur::foundation::SharedPointer<ScriptableValue> ScriptSystem::GetFromStack(int idx)
    {
      return foundation::Memory::ConstructShared<ScriptableValue>(this, idx);
    }

  }
}
