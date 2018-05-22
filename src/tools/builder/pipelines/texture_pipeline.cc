#include "tools/builder/pipelines/texture_pipeline.h"
#include "tools/builder/pipelines/texture_processor.h"
#include "tools/builder/shared/util.h"
#include <foundation/containers/vector.h>
#include <foundation/io/binary_reader.h>
#include <foundation/logging/logger.h>

#define STB_NO_STDIO
#include <stb_image.h>
#include <nvtt/nvtt.h>
#include <nvimage/Image.h>
#include <nvimage/DirectDrawSurface.h>
#include <nvimage/BlockDXT.h>
#include <nvimage/ColorBlock.h>
#include <nvcore/StdStream.h>

namespace sulphur 
{
  namespace builder 
  {
    // ------------------------------------------------------------------------
    bool TexturePipeline::Create(const foundation::String& image_file,
      foundation::TextureAsset& texture) const
    {
      if(image_file.empty() == true)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error, 
          "image_file is empty. Texture should be discarded.");
        return false;
      }

      if(LoadImage(image_file, texture) == false)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "Failed to load the image file. Texture should be discarded.");
        return false;
      }

      return true;
    }

    // ------------------------------------------------------------------------
    bool TexturePipeline::PackageTexture(const foundation::String& asset_origin, 
      foundation::TextureAsset& texture)
    {
      if (texture.name.get_length() == 0)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Warning,
          "Texture name not initialized. The texture will not be packaged.");
        return false;
      }

      if (texture.data.pixel_data.empty() == true)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "Texture holds no pixel data. The texture will not be packaged.");
        return false;
      }

      foundation::String output_file = "";
      if (RegisterAsset(asset_origin, texture.name, output_file, texture.id) == false)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Warning,
          "Failed to register texture. It will not be packaged.");

        return false;
      }

      foundation::BinaryWriter writer(output_file);

      writer.Write(texture.data.pixel_data);
      writer.Write(texture.data.width);
      writer.Write(texture.data.height);
      writer.Write(texture.data.depth);
      writer.Write(texture.data.mips);
      writer.Write(texture.data.type);
      writer.Write(texture.data.format);
      writer.Write(texture.data.compression);

      if (writer.Save() == false)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Warning,
          "Failed to package texture.");
        return false;
      }

      return true;
    }

    // ------------------------------------------------------------------------
    foundation::String TexturePipeline::GetCacheName() const
    {
      return "texture_package";
    }

    // ------------------------------------------------------------------------
    foundation::String TexturePipeline::GetPackageExtension() const
    {
      return "ste";
    }

    // ------------------------------------------------------------------------
    bool TexturePipeline::PackageDefaultAssets()
    {
      foundation::TextureAsset asset = {};
      asset.name = "ps_default_texture";
      asset.data.pixel_data = { 255, 0, 255, 255 };
      asset.data.width = 1;
      asset.data.height = 1;
      asset.data.depth = 0;
      asset.data.mips = 0;
      asset.data.type = foundation::TextureType::k2D;
      asset.data.format = foundation::TexelFormat::kRGBA;
      asset.data.compression = foundation::TextureCompressionType::kNone;

      if (PackageTexture(ASSET_ORIGIN_USER, asset) == false)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "Failed to package default asset.");
        return false;
      }

      return true;
    }

    // ------------------------------------------------------------------------
    bool TexturePipeline::LoadImage(const foundation::String& image_file,
      foundation::TextureAsset& texture) const
    {
      if (image_file.empty() == true)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "image_file is empty. Image couldn't be loaded.");
        return false;
      }

      foundation::String file = image_file;
      eastl::replace(file.begin(), file.end(), '\\', '/');
      size_t extension_start = file.find_last_of('.');
      size_t name_start = file.find_last_of('/') + 1;
      foundation::String file_name = file.substr(name_start, extension_start - name_start);
      foundation::String file_extension = file.substr(extension_start);

      char8_t(*to_lower_case)(char in) = eastl::CharToLower;
      eastl::transform(file_extension.begin(), file_extension.end(), file_extension.begin(), to_lower_case);

      foundation::BinaryReader reader(file);
      if(reader.is_ok() == false)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "Failed to open image file. Image couldn't be loaded. file: %s", file.c_str());
        return false;
      }

      const unsigned char* image_data = reader.data().data();
      const int size = static_cast<int>(reader.data().size());

      if(file_extension == ".png" ||
        file_extension == ".jpg" ||
        file_extension == ".jpeg" ||
        file_extension == ".bmp" ||
        file_extension == ".tga")
      {
        if (LoadImageSTBI(file, texture, image_data, size) == false)
        {
          PS_LOG_WITH(foundation::LineAndFileLogger, Error,
            "Failed to load image with STBI.");
          return false;
        }
      }
      else if(file_extension == ".dds")
      {
        if (LoadImageNVTT(file, texture, image_data, size) == false)
        {
          PS_LOG_WITH(foundation::LineAndFileLogger, Error,
            "Failed to load image with NVTT.");
          return false;
        }
      }

      texture.name = file_name;

      return true;
    }

    // ------------------------------------------------------------------------
    bool TexturePipeline::LoadImageSTBI(const foundation::String& image_file,
      foundation::TextureAsset& texture,
      const unsigned char* image_data,
      const int size) const
    {
      int width = 0;
      int height = 0;
      int num_channels = 0;

      stbi_info_from_memory(image_data, size, &width, &height, &num_channels);
      if (num_channels == 3)
      {
        // RGB images are not supported. Force RGBA.
        num_channels = 4;
      }

      int real_num_channels = 0;
      unsigned char* texture_data = stbi_load_from_memory(image_data, size, &width, &height,
        &real_num_channels, num_channels);
      if (texture_data == nullptr)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "Failed to load texture data from image. file: %s", image_file.c_str());
        return false;
      }

      const int texture_data_size = width * height * num_channels;
      texture.data.pixel_data.resize(texture_data_size);
      memcpy_s(texture.data.pixel_data.data(), texture_data_size, texture_data,
        texture_data_size);

      stbi_image_free(texture_data);

      // Flip the image
      for(int y = 0; y < height / 2; ++y)
      {
        for(int x = 0; x < width; ++x)
        {
          for (int c = 0; c < num_channels; ++c)
          {
            foundation::Vector<byte>& pd = texture.data.pixel_data;
            eastl::swap(pd[(y * width + x) * num_channels + c], 
              pd[((height - y - 1) * width + x) * num_channels + c]);
          }
        }
      }

      texture.data.width = width;
      texture.data.height = height;
      texture.data.depth = 1;
      texture.data.mips = 1;
      texture.data.type = foundation::TextureType::k2D;
      texture.data.format = foundation::TexelFormat::kRGBA;
      texture.data.compression = foundation::TextureCompressionType::kNone;

      return true;
    }

    // ------------------------------------------------------------------------
    bool TexturePipeline::LoadImageNVTT(const foundation::String& image_file,
      foundation::TextureAsset& texture,
      const unsigned char* /*image_data*/,
      const int /*size*/) const
    {
      nv::DirectDrawSurface dds = {};
      if (dds.load(image_file.c_str()) == false)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "Failed to load texture data from image. file: %s", image_file.c_str());
        return false;
      }

      uint32_t width = dds.width();
      uint32_t height = dds.height();
      uint32_t depth = dds.depth();
      int mips = dds.mipmapCount();
      int array_size = dds.arraySize();

      foundation::TextureType type = {};
      if(dds.isTexture1D())
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "1D textures are not supported.");
        return false;
      }
      else if(dds.isTexture2D())
      {
        type = foundation::TextureType::k2D;

        if(LoadSurface(dds, 0, 0, texture.data.pixel_data) == false)
        {
          PS_LOG_WITH(foundation::LineAndFileLogger, Error,
            "Couldn't load the surface.");
          return false;
        }
      }
      else if(dds.isTexture3D())
      {
        type = foundation::TextureType::k3D;

        for (int i = 0; i < static_cast<int>(depth); ++i)
        {
          foundation::Vector<byte> pixel_data = {};
          if (LoadSurface(dds, i, 0, pixel_data) == false)
          {
            PS_LOG_WITH(foundation::LineAndFileLogger, Error,
              "Couldn't load surface %i.", i);
            return false;
          }

          texture.data.pixel_data.insert(texture.data.pixel_data.end(), pixel_data.begin(), pixel_data.end());
        }
      }
      else if(dds.isTextureArray())
      {
        type = foundation::TextureType::kArray;

        for (int i = 0; i < array_size; ++i)
        {
          foundation::Vector<byte> pixel_data = {};
          if (LoadSurface(dds, i, 0, pixel_data) == false)
          {
            PS_LOG_WITH(foundation::LineAndFileLogger, Error,
              "Couldn't load surface %i.", i);
            return false;
          }

          texture.data.pixel_data.insert(texture.data.pixel_data.end(), pixel_data.begin(), pixel_data.end());
        }
      }
      else if(dds.isTextureCube())
      {
        type = foundation::TextureType::kCube;

        for (int i = 0; i < 6; ++i)
        {
          foundation::Vector<byte> pixel_data = {};
          if (LoadSurface(dds, i, 0, pixel_data) == false)
          {
            PS_LOG_WITH(foundation::LineAndFileLogger, Error,
              "Couldn't load surface %i.", i);
            return false;
          }

          texture.data.pixel_data.insert(texture.data.pixel_data.end(), pixel_data.begin(), pixel_data.end());
        }
      }

      texture.data.width = width;
      texture.data.height = height;
      texture.data.depth = depth;
      texture.data.mips = mips;
      texture.data.format = foundation::TexelFormat::kRGBA;
      texture.data.compression = foundation::TextureCompressionType::kNone;

      return true;
    }

    // ------------------------------------------------------------------------
    bool TexturePipeline::LoadSurface(nv::DirectDrawSurface& dds, int face,
      int mip, foundation::Vector<byte>& pixel_data) const
    {
      int width = dds.surfaceWidth(mip);
      int height = dds.surfaceHeight(mip);
      uint data_size = dds.surfaceSize(mip);
      foundation::Vector<byte> data(data_size);
      if (dds.readSurface(face, mip, data.data(), data_size) == false)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "Failed to read surface from loaded texture.");
        return false;
      }

      if (dds.header.isBlockFormat())
      {
        pixel_data.resize(width * height * 4);

        const int block_width = (width + 3) / 4;
        const int block_height = (height + 3) / 4;

        nv::FOURCC cc = nv::FOURCC(dds.header.d3d9Format());
        int block_size = (cc == nv::FOURCC_DXT1) ? 8 : 16;
        const byte* block_ptr = data.data();

        for (int block_y = 0; block_y < block_height; ++block_y)
        {
          for (int block_x = 0; block_x < block_width; ++block_x)
          {
            nv::ColorBlock colors = {};

            if (cc == nv::FOURCC_DXT1)
            {
              const nv::BlockDXT1 * block = reinterpret_cast<const nv::BlockDXT1*>(block_ptr);
              block->decodeBlock(&colors, false);
            }
            else if (cc == nv::FOURCC_DXT3)
            {
              const nv::BlockDXT3 * block = reinterpret_cast<const nv::BlockDXT3*>(block_ptr);
              block->decodeBlock(&colors, false);
            }
            else if (cc == nv::FOURCC_DXT5)
            {
              const nv::BlockDXT5 * block = reinterpret_cast<const nv::BlockDXT5*>(block_ptr);
              block->decodeBlock(&colors, false);
            }
            else
            {
              PS_LOG_WITH(foundation::LineAndFileLogger, Error,
                "Only DXT1, DXT3 and DXT5 textures are supported.");
              return false;
            }

            for (int y = 0; y < 4; ++y)
            {
              for (int x = 0; x < 4; ++x)
              {
                nv::Color32 color = colors.color(x, y);
                uint offset = ((block_y * 4 + y) * width + (block_x * 4 + x)) * 4;
                pixel_data[offset + 0] = color.r;
                pixel_data[offset + 1] = color.g;
                pixel_data[offset + 2] = color.b;
                pixel_data[offset + 3] = color.a;
              }
            }

            block_ptr += block_size;
          }
        }
      }
      else
      {
        pixel_data.resize(data_size);
        foundation::Vector<byte>& pd = pixel_data;

        // Creates lookup table that is used to go from ARGB or BGRA to RGBA
        uint channel_offset[4] =
        {
          uint(log(dds.header.pf.rmask) / log(256)),
          uint(log(dds.header.pf.gmask) / log(256)),
          uint(log(dds.header.pf.bmask) / log(256)),
          uint(log(dds.header.pf.amask) / log(256)),
        };

        for (uint y = 0u; y < static_cast<uint>(height); ++y)
        {
          for (uint x = 0u; x < static_cast<uint>(width); ++x)
          {
            uint offset = (y * width + x) * 4;
            pd[offset + 0] = (dds.header.pf.rmask != 0) ? data[offset + channel_offset[0]] : 0u;
            pd[offset + 1] = (dds.header.pf.gmask != 0) ? data[offset + channel_offset[1]] : 0u;
            pd[offset + 2] = (dds.header.pf.bmask != 0) ? data[offset + channel_offset[2]] : 0u;
            pd[offset + 3] = (dds.header.pf.amask != 0) ? data[offset + channel_offset[3]] : 255u;
          }
        }
      }

      return true;
    }
  }
}
