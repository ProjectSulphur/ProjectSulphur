#include "engine/assets/compute_shader.h"
#include "engine/assets/asset_system.h"

#include <foundation/pipeline-assets/shader.h>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    ComputeShader::ComputeShader(ShaderProgramHandle compute_shader):
      compute_shader_(compute_shader)
    {
      assert(compute_shader->IsValid() && compute_shader->type() == ShaderType::kCompute);
    }

    //--------------------------------------------------------------------------------
    const glm::uvec3& ComputeShader::GetWorkGroupSize() const
    {
      return compute_shader_->work_group_size();
    }

    //--------------------------------------------------------------------------------
    const UniformBuffer& ComputeShader::GetUniformBuffer() const
    {
      return compute_shader_->uniform_buffer();
    }

    //--------------------------------------------------------------------------------
    const foundation::Vector<TextureInfo>& ComputeShader::GetTextureInfo() const
    {
      return compute_shader_->GetTextureInfo();
    }

    //--------------------------------------------------------------------------------
    const foundation::Vector<TextureInfo>& ComputeShader::GetUAVInfo() const
    {
      return compute_shader_->GetUAVInfo();
    }
  }
}