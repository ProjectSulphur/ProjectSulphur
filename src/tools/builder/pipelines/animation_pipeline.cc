#include "tools/builder/pipelines/animation_pipeline.h"
#include "tools/builder/pipelines/scene_loader.h"
#include <foundation/pipeline-assets/animation.h>
#include <assimp/scene.h>

namespace sulphur 
{
  namespace builder 
  {
    //--------------------------------------------------------------------------------
    bool AnimationPipeline::Create(const foundation::Path& file,
      SceneLoader& scene_loader,
      foundation::Vector<foundation::AnimationAsset>& animations)
    {
      const aiScene* scene = scene_loader.LoadScene(file);
      if (scene == nullptr)
      {
        PS_LOG_BUILDER(Error,
          "Unable to load scene from file %s.", file.GetString().c_str());
        return false;
      }

      for(unsigned i = 0; i < scene->mNumAnimations; ++i)
      {
        aiAnimation* ai_animation = scene->mAnimations[i];
        foundation::AnimationAsset animation = {};

        animation.name = ai_animation->mName.C_Str();
        if (animation.name.get_length() == 0)
        {
          PS_LOG_BUILDER(Error,
            "Animation doesn't have a name. It will not be created.");
          continue;
        }
        animation.data.duration = static_cast<float>(ai_animation->mDuration);
        if (animation.data.duration <= 0.0f)
        {
          PS_LOG_BUILDER(Error,
            "Animation %s duration is 0. It will not be created.",
            animation.name.GetCString());
          continue;
        }
        animation.data.ticks_per_second = static_cast<float>(ai_animation->mTicksPerSecond);
        if (animation.data.ticks_per_second <= 0.0f)
        {
          PS_LOG_BUILDER(Error,
            "Animation %s ticks per second is 0. It will not be created.",
            animation.name.GetCString());
          continue;
        }

        for(unsigned j = 0; j < ai_animation->mNumChannels; ++j)
        {
          aiNodeAnim* ai_node_anim = ai_animation->mChannels[j];
          foundation::AnimationChannel channel = {};

          channel.bone_name = ai_node_anim->mNodeName.C_Str();
          if(channel.bone_name.empty() == true)
          {
            PS_LOG_BUILDER(Error,
              "Bone names cannot be empty. Animation %s will not be created.",
              animation.name.GetCString());
            continue;
          }

          channel.position_keys.resize(ai_node_anim->mNumPositionKeys);
          for(unsigned p = 0; p < ai_node_anim->mNumPositionKeys; ++p)
          {
            channel.position_keys[p].time = static_cast<float>(
              ai_node_anim->mPositionKeys[p].mTime);
            const aiVector3D& ai_vec = ai_node_anim->mPositionKeys[p].mValue;
            channel.position_keys[p].vector = glm::vec3(ai_vec.x, ai_vec.y, ai_vec.z);
          }

          channel.rotation_keys.resize(ai_node_anim->mNumRotationKeys);
          for (unsigned r = 0; r < ai_node_anim->mNumRotationKeys; ++r)
          {
            channel.rotation_keys[r].time = static_cast<float>(
              ai_node_anim->mRotationKeys[r].mTime);
            const aiQuaternion& ai_quat = ai_node_anim->mRotationKeys[r].mValue;
            channel.rotation_keys[r].quaternion = 
              glm::quat(ai_quat.w, ai_quat.x, ai_quat.y, ai_quat.z);
          }

          channel.scale_keys.resize(ai_node_anim->mNumScalingKeys);
          for (unsigned s = 0; s < ai_node_anim->mNumScalingKeys; ++s)
          {
            channel.scale_keys[s].time = static_cast<float>(
              ai_node_anim->mScalingKeys[s].mTime);
            const aiVector3D& ai_vec = ai_node_anim->mScalingKeys[s].mValue;
            channel.scale_keys[s].vector = glm::vec3(ai_vec.x, ai_vec.y, ai_vec.z);
          }

          animation.data.channels.push_back(channel);
        }

        animations.push_back(animation);
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    bool AnimationPipeline::PackageAnimation(const foundation::Path& asset_origin,
      foundation::AnimationAsset& animation)
    {
      if (animation.name.get_length() == 0)
      {
        PS_LOG_BUILDER(Error,
          "Animation name not initialized. The animation will not be packaged.");
        return false;
      }

      if (animation.data.channels.empty() == true)
      {
        PS_LOG_BUILDER(Error,
          "Animation holds no keyframe data. The animation will not be packaged.");
        return false;
      }

      foundation::Path output_file = "";
      if (RegisterAsset(asset_origin, animation.name, output_file, animation.id) == false)
      {
        PS_LOG_BUILDER(Error,
          "Failed to register animation. The animation will not be packaged.");

        return false;
      }

      foundation::BinaryWriter writer(output_file);

      writer.Write(animation.data);

      if (writer.SaveCompressed(foundation::CompressionType::kHighCompression) == false)
      {
        PS_LOG_BUILDER(Error,
          "Failed to package animation.");
        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    foundation::String AnimationPipeline::GetPackageExtension() const
    {
      return "san";
    }

    //--------------------------------------------------------------------------------
    foundation::String AnimationPipeline::GetCacheName() const
    {
      return "animation_package";
    }
  }
}
