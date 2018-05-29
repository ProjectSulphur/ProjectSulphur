#include "tools/builder/pipelines/shader_pipeline.h"

namespace sulphur
{
  namespace builder
  {
    foundation::String ShaderPipeline::GetShaderDefines()
    {
      return
        "#define PS_CONSTANTBUFFER cbuffer              \n" \
        "#define PS_VSOUTPUT SV_POSITION                \n" \
        "#define PS_PSOUTPUT SV_Target                  \n" \
        "#define PS_PRECISE precise                     \n" \
        "#define PS_MAXVERTEXCOUNT maxvertexcount       \n" \
        "#define PS_GSOUTPUT_TRIANGLE TriangleStream    \n" \
        "#define PS_GSOUTPUT_POINT PointStream          \n" \
        "#define PS_GSOUTPUT_LINE LineStream            \n" \
        "#define PS_GSINPUT_POINT point                 \n" \
        "#define PS_GSINPUT_LINE line                   \n" \
        "#define PS_GSINPUT_ADJLINE lineadj             \n" \
        "#define PS_GSINPUT_TRIANGLE triangle           \n" \
        "#define PS_GSINPUT_ADJTRIANGLE triangleadj     \n" \
        "#define PS_SAMPLECMPLOD0 SampleCmpLevelZero    \n" \
        "#define PS_SAMPLELOD SampleLevel               \n";


    }
  }
}