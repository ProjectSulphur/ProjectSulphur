#pragma once
#include <foundation/containers/vector.h>
#include <foundation/io/binary_serializable.h>
#include <foundation/containers/string.h>
#include <foundation/utils/asset_definitions.h>
#include <glm/glm.hpp>

namespace sulphur 
{
  namespace foundation 
  {
    /**
    * @class sulphur::foundation::ShaderResource : sulphur::foundation::IBinarySerializable
    * @brief data structure for holding shader reflection data
    * @author Stan Pepels, Timo van Hees
    */
    class ShaderResource : public IBinarySerializable
    {
    public:
      /*
       * @see sulphur::foundation::IBinarySerializable
       */
      void Write(BinaryWriter& binary_writer) const override;
      /*
      * @see sulphur::foundation::IBinarySerializable
      */
      void Read(BinaryReader& binary_reader) override;

      /**
       * @brief Equals operator.
       * @param[in] rhs (const sulphur::foundation::Resource&) The resource to compare with.
       * @return (bool) True if the resources are equal to each other. 
       */
      bool operator==(const ShaderResource& rhs) const;

      /**
       * @brief Checks if two shader resources from different shader stages would 
       * cause a linker error.
       * @param[in] resource (const sulphur::foundation::ShaderResource&) 
       * The shader resource to check against.
       * @return (bool) True if the resources won't cause a linker error.
       */
      bool LinkerCheck(const ShaderResource& resource) const;

      /**
      * @brief types of resources
      */
      enum struct Types : uint8_t
      {
        kUniformBuffer, //!< uniform buffer
        kInput,//!< shader inputs
        kOutput,//!< shader outputs
        kStorageImage,//!< shader used images
        kSampledImage,//!< shader used samplers
        kAtomicCounter,//!< shader atomic counters
        kPushConstantBuffer,//!< shader push constant bufers
        kSeparateImage,  //!< hlsl texture2d
        kSeparateSampler  //!< hlsl samplers
      };

      /**
       * @brief Types of shader resources.
       */
      enum struct ConcreteType : uint8_t
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
      struct Image
      {
        /**
         * @brief Types of images.
         */
        enum struct Dimensions : uint8_t
        {
          k1D,
          k2D,
          k3D,
          kCubed
        };

        Dimensions dimension; //!< The type of the image.
        bool arrayed; //!< If the image is an array.
      };

      bool is_array; //!< is resource arrayed
      Vector<uint32_t> array_size; //!< array sizes. array of arrays supported through keeping a list of array sizes
      uint32_t binding; //!< resource binding
      uint32_t desc_set; //!< resource descriptor set
      Image image;  //!< The dimensions of the image.

      String name; //!< name of the resource
      Types base_type; //!< base type of this resource. this type refers to the top most type ie if a struct has members the base type for those members will be struct
      ConcreteType concrete_type; //!< concrete type of the resource. the actual data type of the resource
      uint32_t cols;  //!< resource width ie number of rows in a matrix
      uint32_t vec_size; //!< resource height ie. number of cols in a matrix
      size_t offset; //!< memory offset in bytes
      size_t size; //!< size of the resource in bytes
      Vector<ShaderResource> members; //!< member fields of a resource i.e. members of a struct
    };

    /**
    * @class sulphur::foundation::ShaderData : sulphur::foundation::IBinarySerializable
    * @brief structure for storing data about a loaded/converted shader
    * @author Stan Pepels, Timo van Hees
    * @TODO remove redundant shader resources
    */
    class ShaderData : public foundation::IBinarySerializable
    {
    public:
      /*
      * @see sulphur::foundation::IBinarySerializable
      */
      void Write(BinaryWriter& binary_writer) const override;
      /*
      * @see sulphur::foundation::IBinarySerializable
      */
      void Read(BinaryReader& binary_reader) override;

      /**
      * @brief supported shader stages
      * @see https://msdn.microsoft.com/en-us/library/windows/desktop/bb205146(v=vs.85).aspx)
      */
      enum struct ShaderStage : uint8_t
      {
        kVertex = 0, //!< vertex shader. .vert file extesion
        kDomain,      //!< domain shader .doma file extension
        kHull,        //!< hull shader .hull file extension
        kGeometry,    //!< geom shader .geom file extension
        kPixel,       //!< pixel shader  .pixe file extensions
        kCompute      //!< compute shader .comp file extension
      };

      String name;                                  //!< shader name created from original fine name
      ShaderStage stage;                            //!< specific shader stage
      
      glm::uvec3 workgroup_size;                    //!< Workgroup size use in a compute shader
      Vector<ShaderResource> uniform_buffers;       //!< uniform buffers used in shader
      Vector<ShaderResource> inputs;                //!< shader input resources
      Vector<ShaderResource> outputs;               //!< shader output resources
      Vector<ShaderResource> storage_images;        //!< shader image resources
      Vector<ShaderResource> sampled_images;        //!< shader sampled images resources
      Vector<ShaderResource> atomic_counters;       //!< shader atomic counters
      Vector<ShaderResource> push_constant_buffers; //!< shader push constant buffers
      Vector<ShaderResource> separate_images;       //!< hlsl texture2d resources
      Vector<ShaderResource> separate_samplers;     //!< hlsl sampler resources

      Vector<uint8_t> spirv_data; //!< Spirv binary
      Vector<uint8_t> hlsl_data;  //!< HLSL binary
      Vector<uint8_t> pssl_data;  //!< PSSL binary
    };

    /**
    * @struct sulphur::foundation::ShaderAsset
    * @brief structure for storing shader assets
    * @author Stan Pepels, Timo van Hees
    */
    struct ShaderAsset
    {
      AssetName name;   //!< shader name created from original file name
      AssetID id;       //!< the ID of the shader
      ShaderData data;  //!< the shader data
    };
  }
}
