#include "compute_shader_manager.h"

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    ComputeShader* ComputeShaderManager::ImportAsset(const foundation::Path& /*asset_file*/)
    {
      PS_LOG(Error, "Compute shaders cannot be loaded because they aren't packaged.");
      return nullptr;
    }
  }
}
