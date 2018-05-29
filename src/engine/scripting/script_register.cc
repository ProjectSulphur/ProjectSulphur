#include "engine/scripting/script_register.h"

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------
    const int ScriptRegister::kIdOffset_ = 256;
    int ScriptRegister::current_id_ = -1;

    //------------------------------------------------------------------------------
    ScriptRegister::ScriptRegister(ScriptState* script_state)
      : script_state_(script_state)
    {

    }

    //------------------------------------------------------------------------------
    void ScriptRegister::RegisterTypes()
    {
      foundation::SharedPointer<ScriptableTable> types = ScriptUtils::CreateTable(script_state_);
      for (
        foundation::Map<foundation::String, int>::iterator it = id_map_.begin();
        it != id_map_.end();
        ++it
      )
      {
        types->Insert(
         it->first.c_str(),
         it->second
        );
      }


      script_state_->RegisterValue("Types", types);
    }
  }
}