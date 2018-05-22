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
    TextureType ConvertTextureType(foundation::ShaderResource::Image type)
    {
      switch (type.dimension)
      {
      case foundation::ShaderResource::Image::Dimensions::kCubed:
        return TextureType::kCubed;
      case foundation::ShaderResource::Image::Dimensions::k2D:
        return TextureType::kTexture2D;
      case foundation::ShaderResource::Image::Dimensions::k3D:
        return TextureType::kTexture3D;
      default:
        return TextureType::kTexture2D;
      }
    }

    //--------------------------------------------------------------------------------
    ShaderProgram::ShaderProgram(foundation::ShaderData& shader_data):
#ifdef PS_PS4
      shader_byte_code_(shader_data.pssl_data),
#else
      shader_byte_code_(shader_data.hlsl_data),
#endif
      uniform_buffer_(shader_data),
      textures_(shader_data.separate_images.size())
    {
      for (int i = 0; i < shader_data.separate_images.size(); ++i)
      {
        textures_[i].name = shader_data.separate_images[i].name;
        textures_[i].type = ConvertTextureType(shader_data.separate_images[i].image);
      }
    }
  }
}