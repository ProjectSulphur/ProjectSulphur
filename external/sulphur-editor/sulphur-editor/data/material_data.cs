using sulphur.editor;
using sulphur.editor.utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace sulphur.editor.data
{
  /**
  * @brief Common filtering modes for texture sampling.
  * @remarks This enum is a copy of the data structure from sulphur::foundation::FilterMode
  */
  public enum FilterMode
  {
    kPoint = 0,
    kLinearMipmapPoint = 0x1,
    kPointMipmapLinear = 0x14,
    kLinear = 0x15,
    kAnisotrpic = 0x55,
  };

  /**
  * @brief Common addressing modes for texture sampling.
  * @remarks This enum is a copy of the data structure from sulphur::foundation::AddressMode
  */
  public enum AddressMode
  {
    kWrap = 1,
    kMirror = 2,
    kClamp = 3,
    kBorder = 4,
    kMirrorOnce = 5
  };

  /**
  * @brief Common blend modes for texture sampling.
  * @remarks This enum is a copy of the data structure from sulphur::foundation::BlendMode
  */
  public enum BlendMode
  {
    kNone = 0,    //!< No alpha blending
    kDefault = 1, //!< SourceColor*SourceAlpha + DestColor*(1-SourceAlpha)
    kAdditive = 2,//!< SourceColor*1 + DestColor*1
  };

  /**
  * @class sulphur.editor.data.UniformBufferData : IBinarySerializable
  * @brief Uniform data used by a material stored in the package.
  * @remarks This class is a copy of the data structure from sulphur::foundation::BlendMode
  */
  public class UniformBufferData : IBinarySerializable
  {
    /*
    * @see sulphur::foundation::IBinarySerializable::Read
    */
    public void Read(BinaryReader reader)
    {
      int size = (int)reader.ReadU64();
      data = new byte[size];
      reader.ReadBytes(ref data);
    }

    public byte[] data;  //!< The data of the uniform buffer in bytes.
  };

  /**
  * @struct sulphur.editor.data.SamplerData
  * @brief Settings for a sampler used by a material stored in the package.
  * @remarks This class is a copy of the data structure from sulphur::foundation::BlendMode
  */
  public struct SamplerData : IBinarySerializable
  {
    public FilterMode filter_mode;   //!< The filter mode of the sampler.
    public byte max_anisotropy;      //!< The anisotropic level of the sampler. [1, 16]
    public AddressMode address_mode; //!< The address mode of the sampler.

    public void Read(BinaryReader reader)
    {
      filter_mode = (FilterMode)reader.ReadU32();
      max_anisotropy = reader.ReadByte();
      address_mode = (AddressMode)reader.ReadU32();
    }
  };

  /**
  * @class sulphur.editor.data.MaterialData :  sulphur.editor.IBinarySerializable
  * @brief Material data to store in the package.
  * @remarks This class is a copy of the data structure from sulphur::foundation::MaterialData
  * @author Jelle de Haan
  */
  public class MaterialData : IBinarySerializable
  {
    public UInt64_s vertex_shader_id;   //!< The ID of the vertex shader asset.
    public UInt64_s geometry_shader_id; //!< The ID of the geometry shader asset.
    public UInt64_s pixel_shader_id;    //!< The ID of the pixel shader asset.

    public List<UniformBufferData> uniform_buffers;    //!< Data for each uniform buffer in the shader
    public List<UInt64_s> separate_images;              //!< Textures for each image in the shader
    public List<SamplerData> separate_samplers;        //!< Settings for each sampler in the shader

    public bool wireframe;           //!< If the mesh should be rendered in wireframe mode.
    public bool backface_culling;    //!< If backface culling should be enabled.
    public BlendMode blend_function; //!< The blend function to use when rendering a mesh.

    public void Read(BinaryReader reader)
    {
      vertex_shader_id = reader.ReadU64();
      geometry_shader_id = reader.ReadU64();
      pixel_shader_id = reader.ReadU64();

      List<UniformBufferData> temp;
      reader.ReadList(out temp);
      uniform_buffers = temp;

      List<UInt64_s> temp1;
      reader.ReadList(out temp1);
      separate_images = temp1;

      List<SamplerData> temp2;
      reader.ReadList(out temp2);
      separate_samplers = temp2;

      wireframe = reader.ReadBoolean();
      backface_culling = reader.ReadBoolean();
      blend_function = (BlendMode)reader.ReadU32();
    }
  }
}
