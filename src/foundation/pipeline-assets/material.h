#pragma once
#include "foundation/utils/type_definitions.h"
#include "foundation/utils/asset_definitions.h"

namespace sulphur 
{
	namespace foundation 
  {
	  /**
     * @brief Common filtering modes for texture sampling.
     */
    enum struct FilterMode
    {
      kPoint = 0,
      kLinearMipmapPoint = 0x1,
      kPointMipmapLinear = 0x14,
      kLinear = 0x15,
      kAnisotrpic = 0x55,
    };

	  /**
     * @brief Common addressing modes for texture sampling.
     */
    enum struct AddressMode
    {
      kWrap = 1,
      kMirror = 2,
      kClamp = 3,
      kBorder = 4,
      kMirrorOnce = 5
    };

    enum struct BlendMode
    {
      kNone = 0,    //!< No alpha blending
      kDefault = 1, //!< SourceColor*SourceAlpha + DestColor*(1-SourceAlpha)
      kAdditive = 2,//!< SourceColor*1 + DestColor*1
    };

    /**
    * @class sulphur::foundation::UniformBufferData : sulphur::foundation::IBinarySerializable
    * @brief Uniform data used by a material stored in the package.
    * @author Timo van Hees
    */
    class UniformBufferData : public IBinarySerializable
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

      Vector<byte> data;  //!< The data of the uniform buffer in bytes.
    };

    /**
    * @struct sulphur::foundation::SamplerData
    * @brief Settings for a sampler used by a material stored in the package.
    * @author Timo van Hees
    */
    struct SamplerData
    {
      FilterMode filter_mode;   //!< The filter mode of the sampler.
      byte max_anisotropy;              //!< The anisotropic level of the sampler. [1, 16]
      AddressMode address_mode; //!< The address mode of the sampler.
    };

	  /**
     * @class sulphur::foundation::MaterialData : sulphur::foundation::IBinarySerializable
     * @brief Material data to store in the package.
     * @author Timo van Hees
     */
    class MaterialData : public IBinarySerializable
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
      AssetID vertex_shader_id;   //!< The ID of the vertex shader asset.
      AssetID geometry_shader_id; //!< The ID of the geometry shader asset.
      AssetID pixel_shader_id;    //!< The ID of the pixel shader asset.

      Vector<UniformBufferData> uniform_buffers;    //!< Data for each uniform buffer in the shader
      Vector<AssetID> separate_images;              //!< Textures for each image in the shader
      Vector<SamplerData> separate_samplers;        //!< Settings for each sampler in the shader

      bool wireframe;           //!< If the mesh should be rendered in wireframe mode.
      bool backface_culling;    //!< If backface culling should be enabled.
      BlendMode blend_function; //!< The blend function to use when rendering a mesh.
    };

	  /**
		 * @struct sulphur::foundation::MaterialAsset
     * @brief Material loaded from a file.
     * @author Timo van Hees
		 */
		struct MaterialAsset
		{
		  AssetName name; //!< The name of the material.
      AssetID id;     //!< The id of the material.
      MaterialData data;          //!< The material data of the material.
		};
	}
}
