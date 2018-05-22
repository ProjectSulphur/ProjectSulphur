#pragma once
#include "foundation/utils/asset_definitions.h"
#include "foundation/utils/type_definitions.h"

namespace sulphur 
{
  namespace foundation 
  {    
    /**
     * @brief Texture formats supported in our texture pipeline.
     */
    enum struct TexelFormat
    {
      kRGBA = 0,  //nvtt::InputFormat_BGRA_8UB,   Normalized [0, 1] 8 bit fixed point.
      kHDR16 = 1, //nvtt::InputFormat_RGBA_16F,   16 bit floating point.
      kHDR = 2,   //nvtt::InputFormat_RGBA_32F,   32 bit floating point.
      kR32 = 3,   //nvtt::InputFormat_R_32F,      Single channel 32 bit floating point.
    };

    /**
     * @brief Common texture types.
     */
    enum struct TextureType
    {
      k2D = 0,    //nvtt::TextureType_2D,
      kCube = 1,  //nvtt::TextureType_Cube,
      k3D = 2,    //nvtt::TextureType_3D,
      kArray = 3, //nvtt::TextureType_Array
    };

    /**
     * @brief Common texture compression types.
     */
    enum struct TextureCompressionType
    {
      kNone = 0,      //nvtt::Format_RGBA,
      kBC1 = 1,       //nvtt::Format_BC1,
      kBC2 = 3,       //nvtt::Format_BC2,
      kBC3 = 4,       //nvtt::Format_BC3,
      kBC4 = 6,       //nvtt::Format_BC4,
      kBC5 = 6,       //nvtt::Format_BC4,
      kBC6 = 10,      //nvtt::Format_BC6,
      kBC7 = 11,      //nvtt::Format_BC7,
      kBC3_RGBM = 12, //nvtt::Format_BC3_RGBM

      //kBC1a = nvtt::Format_DXT1a,
      //kBC3n = nvtt::Format_DXT5n,
    };

    /**
     * @brief Returns the number of bytes needed to store a single texel/pixel.
     * @param[in] format (sulphur::foundation::TexelFormat) The texel format.
     * @return (size_t) The number of bytes needed to store a single texel/pixel.
     */
    inline size_t GetBytesPerTexel(TexelFormat format)
    {
      switch (format)
      {
      case TexelFormat::kRGBA:
        return 1 * 4;
      case TexelFormat::kHDR16:
        return 2 * 4;
      case TexelFormat::kHDR:
        return 4 * 4;
      case TexelFormat::kR32:
        return 4;
      default:
        return 0;
      }
    }

    /**
     * @class sulphur::foundation::TextureData : sulphur::foundation::IBinarySerializable
     * @brief Describes the pixel data of a texture asset.
     * @author Timo van Hees
     */
    class TextureData : public IBinarySerializable
    {
    public:
      /*
      * @see sulphur::foundation::IBinarySerializable::Write
      */
      void Write(BinaryWriter& binary_writer) const override;
      /*
      * @see sulphur::foundation::IBinarySerializable::Read
      */
      void Read(BinaryReader& binary_reader) override;

      Vector<byte> pixel_data;  //!< The pixel data
      int width;  //!< The width of the texture
      int height; //!< The height of the texture
      int depth; //!< The depth of the image or the number of slices (array image)
      int mips; //!< The number of mip levels
      TextureType type; //!< The texture type
      TexelFormat format; //!< The texel format
      TextureCompressionType compression; //!< The compression type
    };

    /**
     * @class sulphur::foundation::TextureAsset
     * @brief Describes a texture stored in a package.
     * @author Timo van Hees
     */
    struct TextureAsset
    {
      AssetName name; //!< The name of the texture
      AssetID id; //!< The id of the texture
      TextureData data; //!< The pixel data of the texture
    };
  }
}
