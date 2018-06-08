#include "shader.h"
#include "foundation/utils/type_definitions.h"
#include "foundation/io/binary_writer.h"
#include "foundation/io/binary_reader.h"

namespace sulphur 
{
  namespace foundation 
  {
    //--------------------------------------------------------------------------------
    void ShaderResource::Write(BinaryWriter& binary_writer) const
    {
      binary_writer.Write(is_array);
      binary_writer.Write(array_size);
      binary_writer.Write(binding);
      binary_writer.Write(desc_set);
      binary_writer.Write(image);
      binary_writer.Write(name);
      binary_writer.Write(base_type);
      binary_writer.Write(concrete_type);
      binary_writer.Write(cols);
      binary_writer.Write(vec_size);
      binary_writer.Write(offset);
      binary_writer.Write(size);
      binary_writer.Write(members);
    }

    //--------------------------------------------------------------------------------
    void ShaderResource::Read(BinaryReader& binary_reader)
    {
     is_array = binary_reader.ReadBoolean();
     array_size = binary_reader.ReadVector<uint32_t>();
     binding = binary_reader.ReadUnsigned32();
     desc_set = binary_reader.ReadUnsigned32();
     image = binary_reader.Read<Image>();
     name = binary_reader.ReadString();
     base_type = binary_reader.Read<Types>();
     concrete_type = binary_reader.Read<ConcreteType>();
     cols = binary_reader.ReadUnsigned32();
     vec_size = binary_reader.ReadUnsigned32();
     offset = binary_reader.ReadUnsigned64();
     size = binary_reader.ReadUnsigned64();
     members = binary_reader.ReadVector<ShaderResource>();
    }

    //--------------------------------------------------------------------------------
    bool ShaderResource::LinkerCheck(const ShaderResource& resource) const
    {
      if(binding == resource.binding || name == resource.name)
      {
        // The resources should be identical. Check if they are.

        if(binding != resource.binding)
        {
          return false;
        }

        if (desc_set != resource.desc_set)
        {
          return false;
        }

        if(name != resource.name)
        {
          return false;
        }

        if(array_size != resource.array_size)
        {
          return false;
        }

        if(base_type != resource.base_type)
        {
          return false;
        }

        if(concrete_type != resource.concrete_type)
        {
          return false;
        }

        if(size != resource.size)
        {
          return false;
        }
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    void ShaderData::Write(BinaryWriter& binary_writer) const
    {
      binary_writer.Write(stage);
      binary_writer.Write(workgroup_size);
      binary_writer.Write(uniform_buffers);
      binary_writer.Write(inputs);
      binary_writer.Write(outputs);
      binary_writer.Write(storage_images);
      binary_writer.Write(sampled_images);
      binary_writer.Write(atomic_counters);
      binary_writer.Write(push_constant_buffers);
      binary_writer.Write(separate_images);
      binary_writer.Write(separate_samplers);
      binary_writer.Write(spirv_data);
      binary_writer.Write(hlsl_data);
      binary_writer.Write(pssl_data);
    }

    //--------------------------------------------------------------------------------
    void ShaderData::Read(BinaryReader& binary_reader)
    {
      stage = binary_reader.Read<ShaderStage>();
      workgroup_size = binary_reader.Read<glm::uvec3>();
      uniform_buffers = binary_reader.ReadVector<ShaderResource>();
      inputs = binary_reader.ReadVector<ShaderResource>();
      outputs = binary_reader.ReadVector<ShaderResource>();
      storage_images = binary_reader.ReadVector<ShaderResource>();
      sampled_images = binary_reader.ReadVector<ShaderResource>();
      atomic_counters = binary_reader.ReadVector<ShaderResource>();
      push_constant_buffers = binary_reader.ReadVector<ShaderResource>();
      separate_images = binary_reader.ReadVector<ShaderResource>();
      separate_samplers = binary_reader.ReadVector<ShaderResource>();
      spirv_data = binary_reader.ReadVector<byte>();
      hlsl_data = binary_reader.ReadVector<byte>();
      pssl_data = binary_reader.ReadVector<byte>();
    }
  }
}