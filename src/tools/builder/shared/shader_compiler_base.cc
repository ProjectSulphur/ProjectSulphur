#include "tools/builder/shared/shader_compiler_base.h"

namespace sulphur 
{
  namespace builder 
  {
    //-----------------------------------------------------------------------------------------------
    ShaderCompilerBase::ShaderCompilerBase(Target target, const ShaderPipelineOptions& options):
      target_(target),
      options_(options)
    {
    }

    //-----------------------------------------------------------------------------------------------
    ShaderCompilerBase::~ShaderCompilerBase()
    {
    }

    //-----------------------------------------------------------------------------------------------
    ShaderCompilerBase::Target ShaderCompilerBase::target()
    {
      return target_;
    }

    //-----------------------------------------------------------------------------------------------
    const ShaderPipelineOptions& ShaderCompilerBase::options() const
    {
      return options_;
    }
  }
}
