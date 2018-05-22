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
      binary_writer.Write(primitve_type);
      binary_writer.Write(bounding_box);
      binary_writer.Write(bounding_sphere);
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
      primitve_type = binary_reader.Read<PrimitiveType>();
      bounding_box = binary_reader.Read<BoundingBox>();
      bounding_sphere = binary_reader.Read<BoundingSphere>();
	  }

    //--------------------------------------------------------------------------------
	  void MeshData::Write(BinaryWriter& binary_writer) const
	  {
      binary_writer.Write(sub_meshes);
	  }

    //--------------------------------------------------------------------------------
	  void MeshData::Read(BinaryReader& binary_reader)
	  {
      sub_meshes = binary_reader.ReadVector<SubMesh>();
	  }
	}
}
