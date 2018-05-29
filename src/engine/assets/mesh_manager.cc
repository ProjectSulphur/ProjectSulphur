#include "mesh_manager.h"
#include "Mesh.h"

#include <foundation/pipeline-assets/mesh.h>

#include <foundation/io/binary_reader.h>
#include <foundation/memory/memory.h>
#include <foundation/io/filesystem.h>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------------
    Mesh* MeshManager::ImportAsset(const foundation::Path& asset_file)
    {
      foundation::BinaryReader reader(
        foundation::Path(application_->project_directory()) + asset_file);
      if (reader.is_ok())
      {
        foundation::MeshData asset_mesh = reader.Read<foundation::MeshData>();

        foundation::Vector<glm::vec3> vertex_positions;
        foundation::Vector<glm::vec3> vertex_normals;
        foundation::Vector<foundation::Color> vertex_colors;
        foundation::Vector<glm::vec2> vertex_uvs;
        foundation::Vector<glm::vec3> vertex_tangents;
        foundation::Vector<uint32_t> indices;
        foundation::Vector<glm::vec4> vertex_bone_weights;
        foundation::Vector<glm::vec<4, uint32_t>> vertex_bone_indices;

        Mesh* mesh = foundation::Memory::Construct<Mesh>();
        mesh->SetBoundingBox(asset_mesh.bounding_box);
        mesh->SetBoundingSphere(asset_mesh.bounding_sphere);

        for (int i = 0; i < asset_mesh.sub_meshes.size(); ++i)
        {
          const uint32_t offset = static_cast<uint32_t>(vertex_positions.size());
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

          for (int j = 0; j < sub_mesh.indices.size(); ++j)
          {
            indices.push_back(sub_mesh.indices[j] + offset);
          }

          for (int j = 0; j < sub_mesh.vertices_bones.size(); ++j)
          {
            const foundation::VertexBones& vertex_bone_data = sub_mesh.vertices_bones[j];
            vertex_bone_indices.push_back(
              {
                static_cast<uint32_t>(vertex_bone_data.bone_indices[0]),
                static_cast<uint32_t>(vertex_bone_data.bone_indices[1]),
                static_cast<uint32_t>(vertex_bone_data.bone_indices[2]),
                static_cast<uint32_t>(vertex_bone_data.bone_indices[3])
              }
            );

            vertex_bone_weights.push_back(
              {
                vertex_bone_data.bone_weights[0],
                vertex_bone_data.bone_weights[1],
                vertex_bone_data.bone_weights[2],
                vertex_bone_data.bone_weights[3],
              }
            );
          }

          mesh->SetIndices(std::move(indices), i);
          indices.clear();
        }

        mesh->SetVertices(std::move(vertex_positions));
        mesh->SetNormals(std::move(vertex_normals));
        mesh->SetColors(std::move(vertex_colors));
        mesh->SetUVs(std::move(vertex_uvs));
        mesh->SetTangents(std::move(vertex_tangents));
        mesh->SetBoneWeights(std::move(vertex_bone_weights));
        mesh->SetBoneIndices(std::move(vertex_bone_indices));
        
        return mesh;
      }

      return nullptr;
    }
  }
}
