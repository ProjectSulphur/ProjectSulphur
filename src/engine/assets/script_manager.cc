#include "script_manager.h"

#include <foundation/io/binary_reader.h>
#include <foundation/memory/memory.h>
#include <foundation/io/filesystem.h>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    Script* ScriptManager::ImportAsset(const foundation::Path& asset_file)
    {
      foundation::BinaryReader reader(
        foundation::Path(application_->project_directory()) + asset_file);

      if (reader.is_ok())
      {
        foundation::ScriptData asset_script = reader.Read<foundation::ScriptData>();
        Script* script =
          foundation::Memory::Construct<Script>();
        script->binary = asset_script.binary;

        return script;
      }

      return nullptr;
    }
  }
}
