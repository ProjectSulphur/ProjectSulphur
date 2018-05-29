#include "mesh.h"

namespace sulphur 
{
  namespace foundation 
  {
    //--------------------------------------------------------------------------------
    void SubMesh::Write(BinaryWriter& binary_writer) const
    {
      binary_writer.Write(vertex_config);
      binary_writer.Write(vertices_base);
      binary_writer.Write(vertices_color);
      binary_writer.Write(vertices_textured);
      binary_writer.Write(vertices_bones);
      binary_writer.Write(indices);
      binary_writer.Write(primitive_type);
      binary_writer.Write(bounding_box);
      binary_writer.Write(bounding_sphere);
      binary_writer.Write(root_transform);
    }

    //--------------------------------------------------------------------------------
    void SubMesh::Read(BinaryReader& binary_reader)
    {
      vertex_config = binary_reader.Read<VertexConfig>();
      vertices_base = binary_reader.ReadVector<VertexBase>();
      vertices_color = binary_reader.ReadVector<VertexColor>();
      vertices_textured = binary_reader.ReadVector<VertexTextured>();
      vertices_bones = binary_reader.ReadVector<VertexBones>();
      indices = binary_reader.ReadVector<uint32_t>();
      primitive_type = binary_reader.Read<PrimitiveType>();
      bounding_box = binary_reader.Read<AABB>();
      bounding_sphere = binary_reader.Read<Sphere>();
      root_transform = binary_reader.Read<glm::mat4>();
    }

    //--------------------------------------------------------------------------------
    void MeshData::Write(BinaryWriter& binary_writer) const
    {
      binary_writer.Write(sub_meshes);
      binary_writer.Write(bounding_box);
      binary_writer.Write(bounding_sphere);
    }

    //--------------------------------------------------------------------------------
    void MeshData::Read(BinaryReader& binary_reader)
    {
      sub_meshes = binary_reader.ReadVector<SubMesh>();
      bounding_box = binary_reader.Read<AABB>();
      bounding_sphere = binary_reader.Read<Sphere>();
    }
  }
}
