#pragma once
#include "tools/builder/pipelines/pipeline_base.h"
#include <foundation/io/filesystem.h>

namespace sulphur 
{
  namespace foundation 
  {
    struct ScriptAsset;
  }

  namespace builder 
  {

    /**
     * @class sulphur::builder::ScriptPipeline : sulphur::builder::PipelineBase
     * @brief Pipeline that handles the creation, packaging and management of scripts.
     * @author Timo van Hees
     */
    class ScriptPipeline : public PipelineBase
    {
    public:
      ScriptPipeline() = default;
      ~ScriptPipeline() = default;

      /**
       * @brief Creates binary version of script in lua file.
       * @param[in] file (const sulphur::foundation::Path&) The lua script.
       * @param[out] script (sulphur::foundation::ScriptAsset&) The compiled script.
       * @return (bool) True if the script compiled succesfully.
       * @remark When false, the script should be discarded.
       */
      bool Create(const foundation::Path& file, foundation::ScriptAsset& script) const;

      /**
       * @brief Adds a script to the package.
       * @param[in] asset_origin (const sulphur::foundation::Path&) The file the asset was
       * created from. Should be ASSET_ORIGIN_USER when the asset is created by the user.
       * @param[in] script (sulphur::foundation::ScriptAsset&) The script to add to the package.
       * @return (bool) True if the script was added to the package succesfully.
       */
      bool PackageScript(const foundation::Path& asset_origin, 
        foundation::ScriptAsset& script);

      /**
       * @see sulphur::builder::PipelineBase::GetPackageExtension
       */
      foundation::String GetPackageExtension() const override;

      /**
       * @see sulphur::builder::PipelineBase::GetCacheName
       */
      foundation::String GetCacheName() const override; 
    };
  }
}
