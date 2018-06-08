#include "tools/builder/pipelines/skeleton_pipeline.h"
#include "tools/builder/shared/util.h"
#include "tools/builder/pipelines/scene_loader.h"
#include <foundation/pipeline-assets/skeleton.h>
#include <EASTL/functional.h>
#include <assimp/scene.h>

namespace sulphur
{
  namespace builder
  {
    //--------------------------------------------------------------------------------
    bool SkeletonPipeline::Create(const foundation::Path& file, SceneLoader& scene_loader,
      foundation::Vector<foundation::SkeletonAsset>& skeletons) const
    {
      if (ValidatePath(file) == false)
      {
        PS_LOG_BUILDER(Error,
          "Invalid file path passed. The path %s does not point to a location in the project directory %s", file.path().c_str(),
          project_dir().path().c_str());
        return false;
      }

      foundation::Path file_path = file.is_relative_path() ? project_dir() + file : file;


      const aiScene* scene = scene_loader.LoadScene(file_path);
      if (scene == nullptr)
      {
        PS_LOG_BUILDER(Error,
          "Unable to load scene from file %s.", file_path.GetString().c_str());
        return false;
      }

      if (FindSkeletonsRecursivly(scene, scene->mRootNode, skeletons) == false)
      {
        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    bool SkeletonPipeline::Create(const aiScene* scene, const aiNode* node,
      foundation::SkeletonAsset& skeleton) const
    {
      bool bones_found = false;
      skeleton.name = node->mName.C_Str();

      for (unsigned i = 0; i < node->mNumMeshes; ++i)
      {
        const aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
        if (ai_mesh->HasBones() == true)
        {
          if (GetBonesFromMesh(ai_mesh, skeleton.data) == false)
          {
            return false;
          }

          bones_found = true;
        }
      }

      if (bones_found == false)
      {
        return false;
      }

      eastl::function<unsigned(aiNode*, unsigned int)> ConstructSkeletalNode;

      // Lamdba that recursively creates skeletal nodes for all scene nodes (node name, node transform, node parent)
      ConstructSkeletalNode = 
        [&skeleton, &ConstructSkeletalNode](aiNode* node, unsigned int parent)->unsigned int
      {
        foundation::SkeletalNode skeletal_node = {};

        skeletal_node.name = node->mName.C_Str();
        skeletal_node.transform = 
          glm::transpose(*reinterpret_cast<glm::mat4*>(&node->mTransformation));
        skeletal_node.parent = parent;

        unsigned int skeletal_node_index = static_cast<unsigned int>(skeleton.data.nodes.size());

        skeleton.data.nodes.push_back(skeletal_node);

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
          skeleton.data.nodes[skeletal_node_index].children.push_back(
            ConstructSkeletalNode(node->mChildren[i], skeletal_node_index)
          );
        }

        return skeletal_node_index;
      };

      // Call lambda, construct skeletal nodes for every node in the scene
      skeleton.data.root_node = ConstructSkeletalNode(scene->mRootNode, UINT_MAX);

      return true;
    }

    //--------------------------------------------------------------------------------
    bool SkeletonPipeline::PackageSkeleton(const foundation::Path& asset_origin,
      foundation::SkeletonAsset& skeleton)
    {
      if (ValidatePath(asset_origin) == false)
      {
        PS_LOG_BUILDER(Error,
          "Invalid file path passed. The path %s does not point to a location in the project directory %s", asset_origin.path().c_str(),
          project_dir().path().c_str());
        return false;
      }

      foundation::Path origin = CreateProjectRelativePath(asset_origin);

      if (skeleton.name.get_length() == 0)
      {
        PS_LOG_BUILDER(Error,
          "Skeleton name not initialized. The skeleton will not be packaged.");
        return false;
      }

      if (skeleton.data.bones.empty() == true)
      {
        PS_LOG_BUILDER(Error,
          "Skeleton holds no bone data. The skeleton will not be packaged.");
        return false;
      }

      foundation::Path output_file = "";
      if (RegisterAsset(origin, skeleton.name, output_file, skeleton.id) == false)
      {
        PS_LOG_BUILDER(Error,
          "Failed to register skeleton. The skeleton will not be packaged.");

        return false;
      }

      foundation::BinaryWriter writer(output_file);

      writer.Write(skeleton.data);

      if (writer.SaveCompressed(foundation::CompressionType::kHighCompression) == false)
      {
        PS_LOG_BUILDER(Error,
          "Failed to package skeleton.");
        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    foundation::String SkeletonPipeline::GetPackageExtension() const
    {
      return "ssk";
    }

    //--------------------------------------------------------------------------------
    foundation::String SkeletonPipeline::GetCacheName() const
    {
      return "skeleton_package";
    }

    //--------------------------------------------------------------------------------
    bool SkeletonPipeline::FindSkeletonsRecursivly(const aiScene* scene, const aiNode* node,
      foundation::Vector<foundation::SkeletonAsset>& skeletons) const
    {
      foundation::SkeletonAsset skeleton = {};
      if (Create(scene, node, skeleton) == true)
      {
        skeletons.push_back(skeleton);
      }

      for (unsigned i = 0; i < node->mNumChildren; ++i)
      {
        if (FindSkeletonsRecursivly(scene, node->mChildren[i], skeletons) == false)
          return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    bool SkeletonPipeline::GetBonesFromMesh(const aiMesh* ai_mesh,
      foundation::SkeletonData& skeleton) const
    {
      if (ai_mesh == nullptr)
      {
        PS_LOG_BUILDER(Error,
          "mesh == nullptr. No skeletons created.");
        return false;
      }

      for (unsigned i = 0; i < ai_mesh->mNumBones; ++i)
      {
        aiBone* ai_bone = ai_mesh->mBones[i];

        unsigned bone_id = 0;
        foundation::Map<foundation::String, unsigned>::iterator it =
          skeleton.bone_names.find(ai_bone->mName.C_Str());
        if (it == skeleton.bone_names.end())
        {
          bone_id = static_cast<unsigned>(skeleton.bones.size());
          skeleton.bone_names[ai_bone->mName.C_Str()] = bone_id;
        }
        else
        {
          // This bone is already part of the skeleton
          continue;
        }

        const glm::mat4 offset_matrix =
          glm::transpose(*reinterpret_cast<glm::mat4*>(&ai_bone->mOffsetMatrix));

        foundation::Bone bone = {};
        bone.transform = offset_matrix;
        skeleton.bones.push_back(bone);
      }

      return true;
    }
  }
}