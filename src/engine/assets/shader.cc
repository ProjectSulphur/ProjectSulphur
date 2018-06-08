#include "engine/assets/shader.h"
#include "engine/assets/asset_system.h"

#include <foundation/pipeline-assets/shader.h>

#if defined(PS_PS4)
#include <graphics/gnm/ps4_default_shaders.h>
#endif

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    Shader::Shader()
    {
      DefaultVertexShader();
      DefaultPixelShader();
    }

    //--------------------------------------------------------------------------------
    Shader::Shader(ShaderProgramHandle vertex_shader,
      ShaderProgramHandle hull_shader,
      ShaderProgramHandle domain_shader,
      ShaderProgramHandle geometry_shader,
      ShaderProgramHandle pixel_shader):
      vertex_shader_(vertex_shader),
      hull_shader_(hull_shader),
      domain_shader_(domain_shader),
      geometry_shader_(geometry_shader),
      pixel_shader_(pixel_shader)
    {
      if (vertex_shader_)
      {
        assert(vertex_shader_->IsValid() && vertex_shader_->type() == ShaderType::kVertex);
      }
      if (hull_shader_)
      {
        assert(hull_shader_->IsValid() && hull_shader_->type() == ShaderType::kHull);
      }
      if (domain_shader_)
      {
        assert(domain_shader_->IsValid() && domain_shader_->type() == ShaderType::kDomain);
      }
      if (geometry_shader_)
      {
        assert(geometry_shader_->IsValid() && geometry_shader_->type() == ShaderType::kGeometry);
      }
      if (pixel_shader_)
      {
        assert(pixel_shader_->IsValid() && pixel_shader_->type() == ShaderType::kPixel);
      }
    }

    //--------------------------------------------------------------------------------
    const UniformBuffer& Shader::GetUniformBuffer(ShaderType type) const
    {
      switch (type)
      {
      case ShaderType::kVertex:
        return vertex_shader_->uniform_buffer();
      case ShaderType::kHull:
        return hull_shader_->uniform_buffer();
      case ShaderType::kDomain:
        return domain_shader_->uniform_buffer();
      case ShaderType::kGeometry:
        return geometry_shader_->uniform_buffer();
      case ShaderType::kPixel:
      default:
        return pixel_shader_->uniform_buffer();
      }
    }

    //--------------------------------------------------------------------------------
    const ShaderProgramHandle& Shader::GetShaderByType(ShaderType type) const
    {
      switch (type)
      {
      case ShaderType::kVertex:
        return vertex_shader_;
      case ShaderType::kHull:
        return hull_shader_;
      case ShaderType::kDomain:
        return domain_shader_;
      case ShaderType::kGeometry:
        return geometry_shader_;
      case ShaderType::kPixel:
      default:
        return pixel_shader_;
      }
    }

    //--------------------------------------------------------------------------------
    const foundation::Vector<TextureInfo>& Shader::GetTextureInfo(ShaderType type) const
    {
      switch (type)
      {
      case ShaderType::kVertex:
        return vertex_shader_->GetTextureInfo();
      case ShaderType::kHull:
        return hull_shader_->GetTextureInfo();
      case ShaderType::kDomain:
        return domain_shader_->GetTextureInfo();
      case ShaderType::kGeometry:
        return geometry_shader_->GetTextureInfo();
      case ShaderType::kPixel:
      default:
        return pixel_shader_->GetTextureInfo();
      }
    }

    //--------------------------------------------------------------------------------
    const foundation::Vector<TextureInfo>& Shader::GetUAVInfo(ShaderType type) const
    {
      switch (type)
      {
      case ShaderType::kVertex:
        return vertex_shader_->GetUAVInfo();
      case ShaderType::kHull:
        return hull_shader_->GetUAVInfo();
      case ShaderType::kDomain:
        return domain_shader_->GetUAVInfo();
      case ShaderType::kGeometry:
        return geometry_shader_->GetUAVInfo();
      case ShaderType::kPixel:
      default:
        return pixel_shader_->GetUAVInfo();
      }
    }

    //--------------------------------------------------------------------------------
    void Shader::DefaultVertexShader()
    {
      static bool init_ = false;

      AssetSystem& as = AssetSystem::Instance();
            
      if (!init_)
      {
        init_ = true;
        vertex_shader_ = as.Load<ShaderProgram>("ps_default_vertex_shader");
      }
      else
      {
        vertex_shader_ = as.GetHandle<ShaderProgram>("ps_default_vertex_shader");
      }
    }

    //--------------------------------------------------------------------------------
    void Shader::DefaultPixelShader()
    {
      static bool init_ = false;

      AssetSystem& as = AssetSystem::Instance();

      if (!init_)
      {
        init_ = true;
        pixel_shader_ = as.Load<ShaderProgram>("ps_default_pixel_shader");
      }
      else
      {
        pixel_shader_ = as.GetHandle<ShaderProgram>("ps_default_pixel_shader");
      }
    }
  }
}