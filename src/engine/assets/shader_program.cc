#include "engine/assets/shader.h"
#include "engine/assets/asset_system.h"

#include <foundation/pipeline-assets/shader.h>

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
    ShaderType ConvertShaderType(foundation::ShaderData::ShaderStage stage)
    {
      switch (stage)
      {
      case foundation::ShaderData::ShaderStage::kCompute:
        return ShaderType::kCompute;
      case foundation::ShaderData::ShaderStage::kDomain:
        return ShaderType::kDomain;
      case foundation::ShaderData::ShaderStage::kGeometry:
        return ShaderType::kGeometry;
      case foundation::ShaderData::ShaderStage::kHull:
        return ShaderType::kHull;
      case foundation::ShaderData::ShaderStage::kPixel:
        return ShaderType::kPixel;
      case foundation::ShaderData::ShaderStage::kVertex:
        return ShaderType::kVertex;
      default:
        assert(false && "Invalid shader type found!");
        return ShaderType::kCompute;
      }
  }

    //--------------------------------------------------------------------------------
    ShaderProgram::ShaderProgram(foundation::ShaderData& shader_data) :
#ifdef PS_PS4
      shader_byte_code_(shader_data.pssl_data),
#else
      shader_byte_code_(shader_data.hlsl_data),
#endif
      uniform_buffer_(shader_data),
      textures_(shader_data.separate_images.size()),
      uavs_(shader_data.storage_images.size()),
      type_(ConvertShaderType(shader_data.stage)),
      work_group_size_(shader_data.workgroup_size)
    {
      for (int i = 0; i < shader_data.storage_images.size(); ++i)
      {
        uavs_[i].name = shader_data.storage_images[i].name;
        uavs_[i].type = ConvertTextureType(shader_data.storage_images[i].image);
      }

      for (int i = 0; i < shader_data.separate_images.size(); ++i)
      {
        textures_[i].name = shader_data.separate_images[i].name;
        textures_[i].type = ConvertTextureType(shader_data.separate_images[i].image);
      }
    }
  }
}