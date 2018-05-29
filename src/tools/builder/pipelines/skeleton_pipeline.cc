#include "tools/builder/pipelines/skeleton_pipeline.h"
#include "tools/builder/shared/util.h"
#include "tools/builder/pipelines/scene_loader.h"
#include <foundation/pipeline-assets/skeleton.h>
#include <assimp/scene.h>

namespace sulphur 
{
  namespace builder 
  {
    bool SkeletonPipeline::Create(const foundation::Path& file, SceneLoader& scene_loader,
      foundation::Vector<foundation::SkeletonAsset>& skeletons) const
    {
      const aiScene* scene = scene_loader.LoadScene(file);
      if(scene == nullptr)
      {
        PS_LOG_BUILDER(Error,
          "Unable to load scene from file %s.", file.GetString().c_str());
        return false;
      }

      if(FindSkeletonsRecursivly(scene, scene->mRootNode, skeletons) == false)
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

      for (unsigned i = 0; i < node->mNumMeshes; ++i)
      {
        const aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
        if (ai_mesh->HasBones() == true)
        {
          if (LinkBones(scene, ai_mesh, skeleton.data) == false)
          {
            return false;
          }
        }
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    bool SkeletonPipeline::PackageSkeleton(const foundation::Path& asset_origin,
      foundation::SkeletonAsset& skeleton)
    {
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
      if (RegisterAsset(asset_origin, skeleton.name, output_file, skeleton.id) == false)
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
      if(Create(scene, node, skeleton) == true)
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
        bone.parent = UINT_MAX;
        bone.transform = offset_matrix;
        skeleton.bones.push_back(bone);
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    bool SkeletonPipeline::LinkBones(const aiScene* scene, const aiMesh* ai_mesh,
      foundation::SkeletonData& skeleton) const
    {
      for (unsigned i = 0; i < ai_mesh->mNumBones; ++i)
      {
        aiBone* ai_bone = ai_mesh->mBones[i];

        const foundation::String parent_name =
          scene->mRootNode->FindNode(ai_bone->mName)->mParent->mName.C_Str();

        foundation::Map<foundation::String, unsigned>::iterator it =
          skeleton.bone_names.find(parent_name);
        if (it != skeleton.bone_names.end())
        {
          unsigned child_index = skeleton.bone_names[ai_bone->mName.C_Str()];
          skeleton.bones[child_index].parent = it->second;
          skeleton.bones[it->second].children.push_back(child_index);
        }
      }

      if (skeleton.bones.empty())
      {
        PS_LOG_BUILDER(Error,
          "Mesh doesn't have a skeleton. Skeleton should be discarded.");
        return false;
      }

      return true;
    }
  }
}
