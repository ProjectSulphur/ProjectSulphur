#include "material.h"

namespace sulphur 
{
	namespace foundation 
  {
    //--------------------------------------------------------------------------------
	  void UniformBufferData::Write(BinaryWriter& binary_writer) const
	  {
      binary_writer.Write(data);
	  }

    //--------------------------------------------------------------------------------
	  void UniformBufferData::Read(BinaryReader& binary_reader)
	  {
      data = binary_reader.ReadVector<byte>();
	  }

    //--------------------------------------------------------------------------------
	  void MaterialData::Write(BinaryWriter& binary_writer) const
	  {
      binary_writer.Write(vertex_shader_id);
      binary_writer.Write(geometry_shader_id);
      binary_writer.Write(pixel_shader_id);
      binary_writer.Write(uniform_buffers);
      binary_writer.Write(separate_images);
      binary_writer.Write(separate_samplers);
      binary_writer.Write(wireframe);
      binary_writer.Write(backface_culling);
      binary_writer.Write(blend_function);
	  }

    //--------------------------------------------------------------------------------
	  void MaterialData::Read(BinaryReader& binary_reader)
	  {
     vertex_shader_id = binary_reader.Read<AssetID>();
     geometry_shader_id = binary_reader.Read<AssetID>();
     pixel_shader_id = binary_reader.Read<AssetID>();
     uniform_buffers = binary_reader.ReadVector<UniformBufferData>();
     separate_images = binary_reader.ReadVector<foundation::AssetID>();
     separate_samplers = binary_reader.ReadVector<SamplerData>();
     wireframe = binary_reader.ReadBoolean();
     backface_culling = binary_reader.ReadBoolean();
     blend_function = binary_reader.Read<BlendMode>();
	  }
	}
}
