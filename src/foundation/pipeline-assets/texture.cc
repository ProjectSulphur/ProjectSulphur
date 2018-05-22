#include "texture.h"

namespace sulphur 
{
  namespace foundation 
  {
    //--------------------------------------------------------------------------------
    void TextureData::Write(BinaryWriter& binary_writer) const
    {
      binary_writer.Write(pixel_data);
      binary_writer.Write(width);
      binary_writer.Write(height);
      binary_writer.Write(depth);
      binary_writer.Write(mips);
      binary_writer.Write(type);
      binary_writer.Write(format);
      binary_writer.Write(compression);
    }

    //--------------------------------------------------------------------------------
    void TextureData::Read(BinaryReader& binary_reader)
    {
      pixel_data = binary_reader.ReadVector<byte>();
      width = binary_reader.ReadInt32();
      height = binary_reader.ReadInt32();
      depth = binary_reader.ReadInt32();
      mips = binary_reader.ReadInt32();
      type = binary_reader.Read<TextureType>();
      format = binary_reader.Read<TexelFormat>();
      compression = binary_reader.Read<TextureCompressionType>();
    }
  }
}
