#include "post_process_material_manager.h"

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    PostProcessMaterial* PostProcessMaterialManager::ImportAsset(const foundation::Path& /*asset_file*/)
    {
      PS_LOG(Error, "PostProcessMaterials cannot be loaded because they aren't packaged.");
      return nullptr;
    }
  }
}
