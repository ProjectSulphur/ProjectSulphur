#include "world_pipeline.h"


namespace sulphur
{
  namespace builder
  {
    foundation::String WorldPipeline::GetCacheName() const
    {
      return "world_package";
    }
    foundation::String WorldPipeline::GetPackageExtension() const
    {
      return "sbw";
    }
    bool WorldPipeline::Register(const foundation::Path& path, uint64_t& id)
    {
      foundation::AssetName name = path.GetFileName();
      foundation::Path package_path;

      // todo: when the engine side serialization is done add a check if the accompanied binary file was created.
      RegisterAsset(path, name, package_path, id);
      ExportCache();
      return true;
    }
  }
}