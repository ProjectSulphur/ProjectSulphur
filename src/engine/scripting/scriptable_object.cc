#include "engine/scripting/scriptable_object.h"
#include "engine/scripting/script_utils.h"

namespace sulphur
{
  namespace engine
  {
    void ScriptableObject::SetMetaTable(foundation::SharedPointer<ScriptableValue> value, const char * libname)
    {
      char metaname[128];
      sprintf_s(metaname, "sulphur.%s", libname);

      value->Push();

      luaL_getmetatable(value->script_system()->lua_state(), metaname);
      lua_setmetatable(value->script_system()->lua_state(), -2);

    }
  }
}
