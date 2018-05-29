#pragma once

#include "foundation/containers/vector.h"
#include "foundation/utils/asset_definitions.h"
#include "foundation/utils/shapes.h"

namespace sulphur 
{
  namespace foundation 
  {
    /**
     * @brief Enum with the possible primitive types of a vertex buffer.
     */
    enum struct PrimitiveType : uint8_t
    {
      kNone,  //!< Invalid primitve type.
      kPoint,         
      kLine,          
      kTriangle,      
      kLineStrip,     
      kTriangleStrip  
    };

    /**
    * @struct sulphur::foundation::VertexBones
    * @brief Bone data for a single vertex.
    * @author Timo van Hees
    */
    struct VertexBones
    {
      float bone_weights[4];    //!< The bone weights of the vertex.
      uint16_t bone_indices[4]; //!< The bone indices of the vertex.
    };

    /**
    * @struct sulphur::foundation::VertexTextured
    * @brief Texture data for a single vertex.
    * @author Timo van Hees
    */
    struct VertexTextured
    {
      glm::vec2 uv;       //!< The texture coordinate of the vertex.
      glm::vec3 tangent;  //!< The tangent vector of the vertex.
    };

    /**
    * @struct sulphur::foundation::VertexColor
    * @brief Color data for a single vertex.
    * @author Timo van Hees
    */
    struct VertexColor
    {
      glm::vec4 color;  //!< The color of the vertex.
    };

    /**
    * @struct sulphur::foundation::VertexBase
    * @brief Base data for a single vertex.
    * @author Timo van Hees
    */
    struct VertexBase
    {
      glm::vec3 position; //!< The position of the vertex.
      glm::vec3 normal;   //!< The normal vector of the vertex.
    };

    /**
    * \@brief Enum with the possible vertex data configurations.
    */
    enum struct VertexConfig : uint8_t
    {
      kNone = 0,                //!< No vertex data. Invalid.
      kVertexBase = 1 << 0,     //!< Only base vertex data.
      kVertexColor = 1 << 1,    //!< Only color vertex data. Invalid.
      kVertexTextured = 1 << 2, //!< Only texture vertex data. Invalid.
      kVertexBones = 1 << 3,    //!< Only bone vertex data. Invalid.
      kBaseColor = kVertexBase | kVertexColor,                            //!< Base vertex data with color data.
      kBaseTextured = kVertexBase | kVertexTextured,                      //!< Base vertex data with texture data.
      kBaseBones = kVertexBase | kVertexBones,                            //!< Base vertex data with bone data.
      kBaseColorTextured = kVertexBase | kVertexColor | kVertexTextured,  //!< Base vertex data with color and texture data.
      kBaseColorBones = kVertexBase | kVertexColor | kVertexBones,        //!< Base vertex data with color and bone data.
      kBaseTexturedBones = kVertexBase | kVertexTextured | kVertexBones,  //!< Base vertex data with texture and bone data.
      kAll = 0xFF //!< Base vertex data with color, texture and bone data.
    };

    /**
     * @brief Or assignement operator for sulphur::foundation::VertexConfig.
     * @param[in|out] c1 (sulphur::foundation::VertexConfig&) Left operand.
     * @param[in] c2 (sulphur::foundation::VertexConfig&) Right operand.
     */
    inline void operator|=(VertexConfig& c1, const VertexConfig& c2)
    {
      c1 = VertexConfig(int(c1) | int(c2));
    }

    /**
    * @class sulphur::foundation::SubMesh : sulphur::foundation::IBinarySerializable
    * @brief Sub-mesh of a mesh. Contains the vertex data.
    * @author Timo van Hees
    */
    class SubMesh : public IBinarySerializable
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

      VertexConfig vertex_config;               //!< The vertex data configuration.
      Vector<VertexBase> vertices_base;         //!< The base vertex data.
      Vector<VertexColor> vertices_color;       //!< The color vertex data.
      Vector<VertexTextured> vertices_textured; //!< The texture vertex data.
      Vector<VertexBones> vertices_bones;       //!< The bone vertex data.
      Vector<uint32_t> indices;                 //!< The index data.
      PrimitiveType primitive_type;             //!< The primitive type stored in the vertex data.
      AABB bounding_box;                        //!< The bounding box of the sub-mesh.
      Sphere bounding_sphere;                   //!< The bounding sphere of the sub-mesh.
      glm::mat4 root_transform;                 //!< The offset matrix of the sub-mesh from the root.
    };

    /**
    * @class sulphur::foundation::MeshData : sulphur::foundation::IBinarySerializable
    * @brief Mesh data to store in the package.
    * @author Timo van Hees
    */
    class MeshData : public IBinarySerializable
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

      Vector<SubMesh> sub_meshes; //!< List of sub-meshes.   
      AABB bounding_box; //!< The bounding box of the mesh.
      Sphere bounding_sphere; //!< The bounding sphere of the mesh.
    };

    /**
    * @struct sulphur::foundation::MeshAsset
    * @brief Mesh loaded from a file.
    * @author Timo van Hees
    */
    struct MeshAsset
    {
      AssetName name; //!< The name of the mesh.
      AssetID id;     //!< The ID of the mesh.
      MeshData data;              //!< The mesh data of the mesh.
    };
  }
}
