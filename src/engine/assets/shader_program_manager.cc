#include "shader_program_manager.h"
#include "shader.h"
#include "uniform_buffer.h"

#include <foundation/memory/memory.h>
#include <foundation/pipeline-assets/shader.h>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    ShaderProgram* ShaderProgramManager::ImportAsset(const foundation::Path& asset_file)
    {
      foundation::BinaryReader reader(
        foundation::Path(application_->project_directory()) + asset_file);
      if (reader.is_ok())
      {
        foundation::ShaderData shader_data = reader.Read<foundation::ShaderData>();

        return foundation::Memory::Construct<ShaderProgram>(shader_data);
      }

      return nullptr;
    }
  }
}
