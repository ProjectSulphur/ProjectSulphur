#include "tools/builder/pipelines/mesh_pipeline.h"
#include "tools/builder/shared/util.h"
#include "tools/builder/pipelines/skeleton_pipeline.h"
#include <foundation/io/binary_writer.h>
#include <foundation/logging/logger.h>
#include <foundation/pipeline-assets/skeleton.h>
#include <glm/gtx/norm.hpp>
#include <assimp/scene.h>

namespace sulphur 
{
  namespace builder 
  {
    //--------------------------------------------------------------------------------
    bool MeshPipeline::Create(const aiScene* scene, bool single_mesh,
      const SkeletonPipeline& skeleton_pipeline,
      foundation::Vector<foundation::MeshAsset>& meshes,
      foundation::Vector<foundation::SkeletonAsset>& skeletons) const
    {
      if (scene == nullptr)
      {
        PS_LOG_BUILDER(Error,
          "scene == nullptr. No meshes created.");
        return false;
      }

      const size_t initial_size = meshes.size();
      unsigned int count = scene->mRootNode->mNumChildren;
      if (scene->mRootNode->mNumMeshes > 0 || single_mesh == true)
      {
        count = 1;
      }
      
      meshes.reserve(initial_size + count);
      for (unsigned int i = 0u; i < count; ++i)
      {
        const aiNode* node = nullptr; 
        if (scene->mRootNode->mNumMeshes > 0 || single_mesh == true)
        {
          node = scene->mRootNode;
        }
        else
        {
          node = scene->mRootNode->mChildren[i];
        }

        foundation::MeshAsset mesh = {};

        // Set the name
        foundation::String name = node->mName.C_Str();
        if(node->mNumMeshes == 1)
        {
          if (scene->mMeshes[node->mMeshes[0]]->mName.length != 0)
          {
            name = scene->mMeshes[node->mMeshes[0]]->mName.C_Str();
          }
        }

        size_t dot_pos = name.find_last_of('.');
        size_t assimp_pos = name.find("_$Assimp");
        name = name.substr(0, glm::min(dot_pos, assimp_pos));

        mesh.name = name;

        const bool result = LoadSubMeshes(scene, node, glm::mat4(1.0f), 
          mesh.data, skeleton_pipeline, skeletons);

        if (result == true && mesh.data.sub_meshes.empty() == false)
        {
          CalculateBoundingShapes(mesh.data);
          meshes.push_back(mesh);
        }
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    bool MeshPipeline::PackageMesh(const foundation::Path& asset_origin, foundation::MeshAsset& mesh)
    {
      if (mesh.name.get_length() == 0)
      {
        PS_LOG_BUILDER(Error, 
          "Mesh name not initialized. The mesh will not be packaged.");
        return false;
      }

      if (mesh.data.sub_meshes.empty() == true)
      {
        PS_LOG_BUILDER(Error,
          "Mesh holds no vertex data. The mesh will not be packaged.");
        return false;
      }

      foundation::Path output_file = "";
      if(RegisterAsset(asset_origin, mesh.name, output_file, mesh.id) == false)
      {
        PS_LOG_BUILDER(Error,
          "Failed to register mesh. The mesh will not be packaged.");

        return false;
      }

      foundation::BinaryWriter writer(output_file);

      writer.Write(mesh.data);

      if (writer.SaveCompressed(foundation::CompressionType::kHighCompression) == false)
      {
        PS_LOG_BUILDER(Error, 
          "Failed to package mesh.");
        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    foundation::String MeshPipeline::GetPackageExtension() const
    {
      return "sme";
    }

    //--------------------------------------------------------------------------------
    foundation::String MeshPipeline::GetCacheName() const
    {
      return "mesh_package";
    }

    //--------------------------------------------------------------------------------
    bool MeshPipeline::LoadSubMeshes(const aiScene* scene, const aiNode* node,
      const glm::mat4& parent_transform, foundation::MeshData& mesh,
      const SkeletonPipeline& skeleton_pipeline,
      foundation::Vector<foundation::SkeletonAsset>& skeletons)
    {
      glm::mat4 root_transform = parent_transform * glm::inverse(glm::transpose(
        *reinterpret_cast<const glm::mat4*>(&node->mTransformation)));

      foundation::SkeletonAsset skeleton = {};
      const bool skeleton_result = skeleton_pipeline.Create(scene, node, skeleton);
      if(skeleton_result == true)
      {
        skeletons.push_back(skeleton);
      }

      for (unsigned int i = 0u; i < node->mNumMeshes; i++)
      {
        const aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
        foundation::SubMesh sub_mesh = {};

        switch (ai_mesh->mPrimitiveTypes)
        {
        case aiPrimitiveType_POINT:
          sub_mesh.primitive_type = foundation::PrimitiveType::kPoint;
          break;
        case aiPrimitiveType_LINE:
          sub_mesh.primitive_type = foundation::PrimitiveType::kLine;
          break;
        case aiPrimitiveType_TRIANGLE:
          sub_mesh.primitive_type = foundation::PrimitiveType::kTriangle;
          // Copy the indices
          sub_mesh.indices.resize(ai_mesh->mNumFaces * 3);
          for (unsigned int f = 0u; f < ai_mesh->mNumFaces; ++f)
          {
            for (int j = 0; j < 3; j++)
            {
              sub_mesh.indices[f * 3 + j] = ai_mesh->mFaces[f].mIndices[j];
            }
          }
          break;
        default:
          {
          PS_LOG_BUILDER(Assert,
            "Unknown primitve type.");
            return false;
          }
        }

        if (ai_mesh->HasPositions() == false)
        {
          PS_LOG_BUILDER(Error,
            "Mesh has no vertex positions.");
          return false;
        }

        sub_mesh.vertices_base.resize(ai_mesh->mNumVertices);
        sub_mesh.vertex_config |= foundation::VertexConfig::kVertexBase;

        // Copy vertex positions
        for (unsigned int v = 0u; v < ai_mesh->mNumVertices; ++v)
        {
          glm::vec3& position = sub_mesh.vertices_base[v].position;
          position = *reinterpret_cast<glm::vec3*>(&ai_mesh->mVertices[v]);
        }

        if (ai_mesh->HasNormals() == true)
        {
          // Copy vertex normals
          for (unsigned int v = 0u; v < ai_mesh->mNumVertices; ++v)
          {
            glm::vec3& normal = sub_mesh.vertices_base[v].normal;
            normal = *reinterpret_cast<glm::vec3*>(&ai_mesh->mNormals[v]);
          }
        }

        if (ai_mesh->HasVertexColors(0) == true)
        {
          sub_mesh.vertices_color.resize(ai_mesh->mNumVertices);
          sub_mesh.vertex_config |= foundation::VertexConfig::kVertexColor;

          // Copy vertex colors
          for (unsigned int v = 0u; v < ai_mesh->mNumVertices; ++v)
          {
            glm::vec4& color = sub_mesh.vertices_color[v].color;
            color = *reinterpret_cast<glm::vec4*>(&ai_mesh->mColors[0][v]);
          }
        }

        if (ai_mesh->HasTextureCoords(0) == true)
        {
          sub_mesh.vertices_textured.resize(ai_mesh->mNumVertices);
          sub_mesh.vertex_config |= foundation::VertexConfig::kVertexTextured;
          // Copy vertex UVs
          for (unsigned int v = 0u; v < ai_mesh->mNumVertices; ++v)
          {
            glm::vec2& uv = sub_mesh.vertices_textured[v].uv;
            uv = *reinterpret_cast<glm::vec2*>(&ai_mesh->mTextureCoords[0][v]);
          }
        }

        if (ai_mesh->HasTangentsAndBitangents() == true)
        {
          if (ai_mesh->HasTextureCoords(0) == false)
          {
            sub_mesh.vertices_textured.resize(ai_mesh->mNumVertices);
            sub_mesh.vertex_config |= foundation::VertexConfig::kVertexTextured;
          }

          // Copy tangents and bitangents
          for (unsigned int v = 0u; v < ai_mesh->mNumVertices; ++v)
          {
            glm::vec3& tangent = sub_mesh.vertices_textured[v].tangent;
            tangent = *reinterpret_cast<glm::vec3*>(&ai_mesh->mTangents[v]);
          }
        }

        if (ai_mesh->HasBones() == true && skeleton_result == true)
        {
          sub_mesh.vertices_bones.resize(ai_mesh->mNumVertices);
          sub_mesh.vertex_config |= foundation::VertexConfig::kVertexBones;
          foundation::Vector<uint8_t> bone_offsets(ai_mesh->mNumVertices, 0);

          // Copy bone weights
          for (uint16_t b = 0u; b < ai_mesh->mNumBones; ++b)
          {
            const aiBone* ai_bone = ai_mesh->mBones[b];
            for (unsigned int w = 0u; w < ai_bone->mNumWeights; ++w)
            {
              const aiVertexWeight& ai_weight = ai_bone->mWeights[w];
              foundation::VertexBones& vertex = sub_mesh.vertices_bones[ai_weight.mVertexId];
              uint8_t& offset = bone_offsets[ai_weight.mVertexId];

              if (offset == 4)
              {
                PS_LOG_BUILDER(Assert,
                  "Vertex has more than 4 bone weights.");
                return false;
              }

              const foundation::Map<foundation::String, unsigned>::const_iterator it = 
                skeleton.data.bone_names.find(ai_bone->mName.C_Str());
              if(it != skeleton.data.bone_names.end())
              {
                vertex.bone_indices[offset] = uint16_t(it->second); // Set the bone index at offset
                vertex.bone_weights[offset] = ai_weight.mWeight; // Set the bone weight at offset
                ++offset;
              }
            }
          }
        }

        CalculateBoundingShapes(sub_mesh);

        sub_mesh.root_transform = root_transform;

        if (sub_mesh.vertex_config != foundation::VertexConfig::kNone)
        {
          mesh.sub_meshes.push_back(sub_mesh);
        }
      }

      // Recursivly add the sub meshes in the child nodes
      for (unsigned int i = 0u; i < node->mNumChildren; ++i)
      {
        if (LoadSubMeshes(scene, node->mChildren[i], root_transform, mesh, 
          skeleton_pipeline, skeletons) == false)
        {
          return false;
        }
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    void MeshPipeline::CalculateBoundingShapes(foundation::SubMesh& sub_mesh)
    {
      if (sub_mesh.vertices_base.empty() == true)
      {
        return;
      }

      glm::vec3& bb_min = sub_mesh.bounding_box.min;
      glm::vec3& bb_max = sub_mesh.bounding_box.max;
      glm::vec3& bs_center = sub_mesh.bounding_sphere.center;
      float& bs_radius = sub_mesh.bounding_sphere.radius;

      bb_min = glm::vec3(FLT_MAX);
      bb_max = glm::vec3(-FLT_MAX);

      glm::vec3 a = sub_mesh.vertices_base[0].position; // Initial point
      glm::vec3 b = a; // Point furthest from a

      for (int i = 0; i < sub_mesh.vertices_base.size(); ++i)
      {
        const glm::vec3& v = sub_mesh.vertices_base[i].position;
        bb_min = glm::min(bb_min, v);
        bb_max = glm::max(bb_max, v);

        if (glm::distance2(a, b) < glm::distance2(a, v))
        {
          b = v;
        }
      }

      glm::vec3 c = b; // Point furthest from b

      for (int i = 0; i < sub_mesh.vertices_base.size(); ++i)
      {
        const glm::vec3& v = sub_mesh.vertices_base[i].position;

        if (glm::distance2(b, c) < glm::distance2(b, v))
        {
          c = v;
        }
      }

      bs_center = (b + c) * 0.5f;
      bs_radius = glm::distance(b, c) * 0.5f;
    }

    void MeshPipeline::CalculateBoundingShapes(foundation::MeshData& mesh)
    {
      if(mesh.sub_meshes.empty() == true)
      {
        return;
      }

      mesh.bounding_box = mesh.sub_meshes[0].bounding_box;
      mesh.bounding_sphere = mesh.sub_meshes[0].bounding_sphere;

      for(int i = 1; i < mesh.sub_meshes.size(); ++i)
      {
        mesh.bounding_box += mesh.sub_meshes[i].bounding_box;
        mesh.bounding_sphere += mesh.sub_meshes[i].bounding_sphere;
      }
    }
  }
}
