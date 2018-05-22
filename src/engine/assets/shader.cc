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
      case ShaderType::kGemometry:
        return geometry_shader_->uniform_buffer();
      case ShaderType::kPixel:
      default:
        return pixel_shader_->uniform_buffer();
      }
    }

    //--------------------------------------------------------------------------------
    const ShaderProgramHandle Shader::GetShaderByType(ShaderType type) const
    {
      switch (type)
      {
      case ShaderType::kVertex:
        return vertex_shader_;
      case ShaderType::kHull:
        return hull_shader_;
      case ShaderType::kDomain:
        return domain_shader_;
      case ShaderType::kGemometry:
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
      case ShaderType::kGemometry:
        return geometry_shader_->GetTextureInfo();
      case ShaderType::kPixel:
      default:
        return pixel_shader_->GetTextureInfo();
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

#ifdef PS_WIN32
#include <graphics/d3d12/d3d12_default_vertex.h>
        const byte* p = default_vs;
        size_t len = sizeof(default_vs);
#elif defined(PS_PS4)
        const byte* p = default_vertex_shader;
        size_t len = sizeof(default_vertex_shader);
#endif
        vertex_shader_ = as.AddAsset<ShaderProgram>(
          foundation::Memory::Construct<ShaderProgram>(),
          "__default_vertex_shader");

        vertex_shader_->shader_byte_code_.assign(p, p + len);
      }
      else
      {
        vertex_shader_ = as.GetHandle<ShaderProgram>("__default_vertex_shader");
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

#ifdef PS_WIN32
#include <graphics/d3d12/d3d12_default_pixel.h>
        const byte* p = default_ps;
        size_t len = sizeof(default_ps);
#elif defined(PS_PS4)
        const byte* p = default_pixel_shader;
        size_t len = sizeof(default_pixel_shader);
#endif
        pixel_shader_ = as.AddAsset<ShaderProgram>(
          foundation::Memory::Construct<ShaderProgram>(),
          "__default_pixel_shader");
        
        pixel_shader_->textures_.resize(1);
        pixel_shader_->shader_byte_code_.assign(p, p + len);
      }
      else
      {
        pixel_shader_ = as.GetHandle<ShaderProgram>("__default_pixel_shader");
      }
    }
  }
}