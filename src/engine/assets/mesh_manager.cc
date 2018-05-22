#include "mesh_manager.h"
#include "Mesh.h"

#include <foundation/pipeline-assets/mesh.h>

#include <foundation/io/binary_reader.h>
#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    Mesh* MeshManager::ImportAsset(const foundation::String& asset_file)
    {
      foundation::BinaryReader reader(asset_file);
      if (reader.is_ok())
      {
        foundation::MeshData asset_mesh = reader.Read<foundation::MeshData>();

        foundation::Vector<glm::vec3> vertex_positions;
        foundation::Vector<glm::vec3> vertex_normals;
        foundation::Vector<foundation::Color> vertex_colors;
        foundation::Vector<glm::vec2> vertex_uvs;
        foundation::Vector<glm::vec3> vertex_tangents;
        foundation::Vector<uint32_t> indices;
        for (int i = 0; i < asset_mesh.sub_meshes.size(); ++i)
        {
          const foundation::SubMesh& sub_mesh = asset_mesh.sub_meshes[i];
          for (int j = 0; j < sub_mesh.vertices_base.size(); ++j)
          {
            vertex_positions.push_back(sub_mesh.vertices_base[j].position);
            vertex_normals.push_back(sub_mesh.vertices_base[j].normal);
          }

          for (int j = 0; j < sub_mesh.vertices_color.size(); ++j)
          {
            vertex_colors.push_back(sub_mesh.vertices_color[j].color);
          }

          for (int j = 0; j < sub_mesh.vertices_textured.size(); ++j)
          {
            vertex_uvs.push_back(sub_mesh.vertices_textured[j].uv);
            vertex_tangents.push_back(sub_mesh.vertices_textured[j].tangent);
          }

          const uint32_t offset = static_cast<uint32_t>(indices.size());
          for (int j = 0; j < sub_mesh.indices.size(); ++j)
          {
            indices.push_back(sub_mesh.indices[j] + offset);
          }
        }

        Mesh* mesh = foundation::Memory::Construct<Mesh>();
        mesh->SetVertices(std::move(vertex_positions));
        mesh->SetNormals(std::move(vertex_normals));
        mesh->SetColors(std::move(vertex_colors));
        mesh->SetUVs(std::move(vertex_uvs));
        mesh->SetTangents(std::move(vertex_tangents));
        mesh->SetIndices(std::move(indices));

        return mesh;
      }

      return nullptr;
    }
  }
}
