#include "tools/builder/pipelines/shader_pipeline.h"

#ifndef PS_PS4_TOOLS
namespace sulphur
{
  namespace builder
  {
    foundation::String ShaderPipeline::GetShaderDefines()
    {
      return  
        "#define PS_CONSTANTBUFFER cbuffer  \n" \
        "#define PS_VSOUTPUT SV_POSITION    \n" \
        "#define PS_PSOUTPUT SV_Target      \n";
    }
  }
}
#endif