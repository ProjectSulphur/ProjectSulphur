#include "shader_manager.h"
#include "shader.h"
#include "uniform_buffer.h"

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    Shader* ShaderManager::ImportAsset(const foundation::Path& /*asset_file*/)
    {
      PS_LOG(Error, "Shaders cannot be loaded because they aren't packaged.");
      return nullptr;
    }
  }
}
