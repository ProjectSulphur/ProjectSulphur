#pragma once
#include "tools/builder/pipelines/pipeline_base.h"

namespace sulphur
{
  namespace builder
  {
    class WorldPipeline : public PipelineBase
    {
    public:
      /**
      * @see sulphur::builder::PipelineBase::GetCacheName
      */
      virtual foundation::String GetCacheName() const override final;

      /**
      * @see sulphur::builder::PipelineBase::GetPackageExtension
      * @remark The extension returned is the filetype serialized by the engine.
      */
      virtual foundation::String GetPackageExtension() const override final;

      bool Register(const foundation::Path& path, uint64_t& id);
    private:
    };
  }
}
