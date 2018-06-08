using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace sulphur.editor.data
{
  /**
  *@class sulphur.editor.data.ShaderResource :  sulphur.editor.IBinarySerializable
  *@brief data structure for holding shader reflection data
  *@remarks This class is a copy of the data structure from sulphur::foundation::ShaderResource
  *@author Jelle de Haan
  */
  public class ShaderResource : IBinarySerializable
  {
    /**
    * @brief types of resources
    */
    public enum Types : byte
    {
      kUniformBuffer, //!< uniform buffer
      kInput,//!< shader inputs
      kOutput,//!< shader outputs
      kStorageImage,//!< shader used images
      kSampledImage,//!< shader used samplers
      kAtomicCounter,//!< shader atomic counters
      kPushConstantBuffer,//!< shader push constant buffers
      kSeparateImage,  //!< hlsl texture2d
      kSeparateSampler  //!< hlsl samplers
    };

    /**
     * @brief Types of shader resources.
     */
    public enum ConcreteTypes : byte
    {
      kStruct,
      kBool,
      kFloat,
      kInt,
      kUInt,
      kDouble,
      kUnknown,
      kChar,
      kVec2,
      kVec3,
      kVec4,
      kMat4x4,
      kMat4x3,
      kMat3x3
    };

    /**
    * @brief Describes the dimensions of an image.
    */
    public class Image : IBinarySerializable
    {
      /**
      * @brief Types of images.
      */
      public enum Dimensions : byte
      {
        k1D,
        k2D,
        k3D,
        kCubed
      };

      /**
      *@brief The type of the image.
      */
      public Dimensions dimension { get; private set; }

      /**
      *@brief If the image is an array.
      */
      public bool arrayed { get; private set; }

      /**
      *@brief implementation of the sulphur.editor.IBinarySerializable interface. this will read an entry from a binary file and fill this struct with its contents.
      *@param[in] reader (sulphur.editor.BinaryReader) reader containing the cache data.
      */
      public void Read(BinaryReader reader)
      {
        dimension = (Dimensions)reader.ReadByte();
        arrayed = reader.ReadBoolean();
      }
    };


    /**
    *@brief Is resource arrayed
    */
    public bool is_array { get; private set; }

    /**
    *@brief Array sizes. Array of arrays supported through keeping a list of array sizes
    */
    public List<utils.UInt32_s> array_size { get; private set; }

    /**
    *@brief resource binding
    */
    public utils.UInt32_s binding { get; private set; }

    /**
    *@brief resource descriptor set
    */
    public utils.UInt32_s desc_set { get; private set; }

    /**
    *@brief The dimensions of the image.
    */
    private Image image_ = new Image();
    public Image image { get { return image_; } private set { image_ = value; } }

    /**
    *@brief Name of the resource.
    */
    public String name { get; private set; }

    /**
    *@brief base type of this resource. this type refers to the top most type ie. if a struct has members the base type for those members will be struct.
    */
    public Types base_type { get; private set; }

    /**
    *@brief Concrete type of the resource. the actual data type of the resource.
    */
    public ConcreteTypes concrete_type { get; private set; }

    /**
    *@brief Resource width ie. number of rows in a matrix.
    */
    public utils.UInt32_s cols { get; private set; }

    /**
    *@brief Resource height ie. number of cols in a matrix.
    */
    public utils.UInt32_s vec_size { get; private set; }

    /**
    *@brief Memory offset in bytes.
    */
    public utils.UInt64_s offset { get; private set; }

    /**
    *@brief Size of the resource in bytes.
    */
    public utils.UInt64_s size { get; private set; }

    /**
    *@brief Member fields of a resource ie. members of a struct.
    */
    public List<ShaderResource> members { get; private set; }

    /**
     *@brief implementation of the sulphur.editor.IBinarySerializable interface. this will read an entry from a binary file and fill this struct with its contents.
     *@param[in] reader (sulphur.editor.BinaryReader) reader containing the cache data.
     */
    public void Read(BinaryReader reader)
    {
      is_array = reader.ReadBoolean();

      List<utils.UInt32_s> temp;
      reader.ReadList(out temp);
      array_size = temp;

      binding = reader.ReadU32();
      desc_set = reader.ReadU32();
      reader.ReadSerializable(image_);
      name = reader.ReadString();
      base_type = (Types)reader.ReadByte();
      concrete_type = (ConcreteTypes)reader.ReadByte();
      cols = reader.ReadU32();
      vec_size = reader.ReadU32();
      offset = reader.ReadU64();
      size = reader.ReadU64();

      List<ShaderResource> temp2;
      reader.ReadList(out temp2);
      members = temp2;
    }
  }

  /**
  *@class sulphur.editor.data.ShaderData :  sulphur.editor.IBinarySerializable
  *@brief structure for storing data about a loaded/converted shader
  *@remarks This class is a copy of the data structure from sulphur::foundation::ShaderData
  *@author Jelle de Haan
  */
  public class ShaderData : IBinarySerializable
  {
    /**
      * @brief supported shader stages
      * @see https://msdn.microsoft.com/en-us/library/windows/desktop/bb205146(v=vs.85).aspx)
      */
    public enum ShaderStage : byte
    {
      kVertex = 0, //!< vertex shader. .vert file extesion
      kDomain,     //!< domain shader .doma file extension
      kHull,       //!< hull shader .hull file extension
      kGeometry,   //!< geom shader .geom file extension
      kPixel,      //!< pixel shader  .pixe file extensions
      kCompute     //!< compute shader .comp file extension
    };

    /**
    *@brief Shader name created from original fine name
    */
    public String name { get; private set; }

    /**
    *@brief Specific shader stage
    */
    public ShaderStage stage { get; private set; }

    /**
    *@brief Workgroup size use in a compute shader
    */
    private utils.Vec3U WorkgroupSize_ = new utils.Vec3U();
    public utils.Vec3U WorkgroupSize { get { return WorkgroupSize_; } private set { WorkgroupSize_ = value; } }

    /**
    *@brief Uniform buffers used in shader
    */
    public List<ShaderResource> uniform_buffers { get; private set; }

    /**
    *@brief Shader input resources
    */
    public List<ShaderResource> inputs { get; private set; }

    /**
    *@brief Shader output resources
    */
    public List<ShaderResource> outputs { get; private set; }

    /**
    *@brief Shader image resources
    */
    public List<ShaderResource> storage_images { get; private set; }

    /**
    *@brief shader sampled images resources
    */
    public List<ShaderResource> sampled_images { get; private set; }

    /**
    *@brief Shader atomic counters
    */
    public List<ShaderResource> atomic_counters { get; private set; }

    /**
    *@brief Shader push constant buffers
    */
    public List<ShaderResource> push_constant_buffers { get; private set; }

    /**
    *@brief HLSL texture2d resources
    */
    public List<ShaderResource> separate_images { get; private set; }

    /**
    *@brief HLSL sampler resources
    */
    public List<ShaderResource> separate_samplers { get; private set; }
    
    /**
     *@brief implementation of the sulphur.editor.IBinarySerializable interface. this will read an entry from a binary file and fill this struct with its contents.
     *@param[in] reader (sulphur.editor.BinaryReader) reader containing the cache data.
     */
    public void Read(BinaryReader reader)
    {
      stage = (ShaderStage)reader.ReadByte();
      
      reader.ReadSerializable(WorkgroupSize_);

      List<ShaderResource> temp;
      reader.ReadList(out temp);
      uniform_buffers = temp;

      List<ShaderResource> temp1;
      reader.ReadList(out temp1);
      inputs = temp1;

      List<ShaderResource> temp2;
      reader.ReadList(out temp2);
      outputs = temp2;

      List<ShaderResource> temp3;
      reader.ReadList(out temp3);
      storage_images = temp3;

      List<ShaderResource> temp4;
      reader.ReadList(out temp4);
      sampled_images = temp4;

      List<ShaderResource> temp5;
      reader.ReadList(out temp5);
      atomic_counters = temp5;

      List<ShaderResource> temp6;
      reader.ReadList(out temp6);
      push_constant_buffers = temp6;

      List<ShaderResource> temp7;
      reader.ReadList(out temp7);
      separate_images = temp7;

      List<ShaderResource> temp8;
      reader.ReadList(out temp8);
      separate_samplers = temp8;
    }
  }
}
