#include "engine/assets/texture.h"

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    Texture::Texture() :
      size_(256u, 256u),
      data_(GenerateTextureData()),
      format_(TextureFormat::kR8G8B8A8_UNORM),
      creation_flags_(TextureCreateFlags::kDefault)
    {}

    //--------------------------------------------------------------------------------
    Texture::Texture(const foundation::Vector<byte>& pixel_data, uint width,
       uint height, TextureFormat format, TextureCreateFlags create_flags) :
      Texture(pixel_data, glm::u32vec2(width, height), format, create_flags)
    {}

    //--------------------------------------------------------------------------------
    Texture::Texture(const foundation::Vector<byte>& pixel_data,
        const glm::u32vec2& size, TextureFormat format, 
        TextureCreateFlags create_flags) :
      size_(size),
      data_(pixel_data),
      format_(format),
      creation_flags_(create_flags)
    {}

    //--------------------------------------------------------------------------------
    Texture::Texture(uint width, uint height, TextureFormat format,
        TextureCreateFlags create_flags) :
      Texture(glm::u32vec2(width, height), format, create_flags)
    {}

    //--------------------------------------------------------------------------------
    Texture::Texture(const glm::u32vec2& size, TextureFormat format, TextureCreateFlags create_flags) :
      size_(size),
      format_(format),
      creation_flags_(create_flags)
    {
      uint elem_size = 1;
      switch (format)
      {
      case TextureFormat::kR8G8B8A8_UNORM:
      case TextureFormat::kR24G8_TYPELESS:
      case TextureFormat::kR32_TYPELESS:
        elem_size = 1;
        break;
      case TextureFormat::kR16G16B16A16_FLOAT:
        elem_size = 2;
        break;
      case TextureFormat::kR32G32B32A32_FLOAT:
        elem_size = 4;
        break;
      }

      data_.resize(size_.x * size_.y * elem_size * 4u, 0u);
    }

    // TODO: REMOVE TEMP TEXTURE GENERATION
    //--------------------------------------------------------------------------------
    foundation::Vector<byte> Texture::GenerateTextureData()
    {
      const uint pixel_byte_size = 4u;

      const uint row_pitch = size_.x * pixel_byte_size;
      const uint cell_pitch = row_pitch >> 3u;		// The width of a cell in the checkerboard texture.
      const uint cell_height = size_.x >> 3u;	// The height of a cell in the checkerboard texture.
      const uint texture_size = row_pitch * size_.y;

      foundation::Vector<byte> data(texture_size);
      byte* p_data = &data[0];

      for (uint n = 0; n < texture_size; n += pixel_byte_size)
      {
        uint x = n % row_pitch;
        uint y = n / row_pitch;
        uint i = x / cell_pitch;
        uint j = y / cell_height;

        if (i % 2 == j % 2)
        {
          p_data[n] = 0x00;		// R
          p_data[n + 1] = 0x00;	// G
          p_data[n + 2] = 0x00;	// B
          p_data[n + 3] = 0xff;	// A
        }
        else
        {
          p_data[n] = 0xff;		// R
          p_data[n + 1] = 0xff;	// G
          p_data[n + 2] = 0xff;	// B
          p_data[n + 3] = 0xff;	// A
        }
      }

      return data;
    }
  }
}