#include "animation_state.h"

#include <foundation/utils/frame.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------------------------------
    AnimationState::AnimationState() :
      is_playing_(false),
      playback_time_(0.0f),
      playback_multiplier_(1.0f)
    {

    }

    //------------------------------------------------------------------------------------------------------
    AnimationState::AnimationState(SkeletonHandle skeleton) :
      is_playing_(false),
      playback_time_(0.0f),
      playback_multiplier_(1.0f),
      skeleton_(skeleton)
    {

    }

    //------------------------------------------------------------------------------------------------------
    AnimationState::AnimationState(AnimationHandle animation) :
      is_playing_(false),
      playback_time_(0.0f),
      playback_multiplier_(1.0f),
      animation_(animation)
    {

    }

    //------------------------------------------------------------------------------------------------------
    AnimationState::AnimationState(SkeletonHandle skeleton, AnimationHandle animation) :
      is_playing_(false),
      playback_time_(0.0f),
      playback_multiplier_(1.0f),
      skeleton_(skeleton),
      animation_(animation)
    {
      
    }

    //------------------------------------------------------------------------------------------------------
    void AnimationState::Update()
    {
      if (is_playing_ && foundation::Frame::delta_time() * playback_multiplier_ > 0.0f)
      {
        playback_time_ += foundation::Frame::delta_time() * playback_multiplier_;

        if (skeleton_ && animation_)
        {
          float local_playback_time = fmod(animation_->ticks_per_second() * playback_time_, animation_->duration());
          const foundation::Vector<Bone>& bones = skeleton_->bones();
          
          if (transformed_bones_.size() != bones.size())
          {
            transformed_bones_.resize(bones.size());
          }
          
          for (int i = 0; i < bones.size(); ++i)
          {
            transformed_bones_[i] = CalculateBoneTransform(i, local_playback_time);
          }
        }
      }
    }

    //------------------------------------------------------------------------------------------------------
    const foundation::Vector<glm::mat4>& AnimationState::GetTransformedBones()
    {
      return transformed_bones_;
    }

    //------------------------------------------------------------------------------------------------------
    glm::mat4 AnimationState::CalculateBoneTransform(unsigned int bone_index, float playback_time) const
    {
      glm::mat4 result(1.0f);
      glm::mat4 parent_transform(1.0f);

      const Bone& bone = skeleton_->bones()[bone_index];

      if (bone.parent != UINT_MAX)
      {
        parent_transform = CalculateBoneTransform(bone.parent, playback_time);
      }

      unsigned int channel_index = GetAnimationChannelByBoneIndex(bone_index);
      if (channel_index != UINT_MAX)
      {
        const AnimationChannel& channel = animation_->animation_channels()[channel_index];
        
        glm::vec3 position = ProcessKeyframes(playback_time, channel.position_keys);
        glm::quat rotation = ProcessKeyframes(playback_time, channel.rotation_keys);
        glm::vec3 scale = ProcessKeyframes(playback_time, channel.scale_keys);
        
        glm::mat4 position_matrix = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 rotation_matrix = glm::toMat4(rotation);
        glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f), scale);
        
        result = position_matrix * rotation_matrix * scaling_matrix;
        result = parent_transform * result;
        result = result * glm::transpose(skeleton_->bones()[bone_index].bone_transform);

        // Override it for now to test whether bone transforms work.
        result = parent_transform * skeleton_->bones()[bone_index].bone_transform;
      }
      
      return result;
    }

    //------------------------------------------------------------------------------------------------------
    glm::vec3 AnimationState::ProcessKeyframes(
      float playback_time, 
      const foundation::Vector<AnimationVectorKey>& keys) const
    {
      if (keys.size() == 0)
      {
        return glm::vec3(1.0f, 1.0f, 1.0f);
      }
      else if (keys.size() == 1)
      {
        return keys[0].value;
      }
      else if (keys.size() > 1)
      {
        unsigned int current_key_index = UINT_MAX;
        unsigned int next_key_index = UINT_MAX;
        
        for (size_t i = 0; i < keys.size(); ++i) 
        {
          if (playback_time < keys[i].time)
          {
            current_key_index = static_cast<unsigned int>(i);
          }
        }

        if (current_key_index == UINT_MAX)
        {
          return keys[0].value;
        }

        next_key_index = static_cast<unsigned int>(fmod(current_key_index + 1, keys.size()));

        float delta_time = keys[next_key_index].time - keys[current_key_index].time;
        float factor = (playback_time - keys[current_key_index].time) / delta_time;

        const glm::vec3& start = keys[current_key_index].value;
        const glm::vec3& end = keys[next_key_index].value;
        glm::vec3 to_end = end - start;

        return start + (to_end * factor);
      }

      return glm::vec3(1.0f, 1.0f, 1.0f);
    }

    //------------------------------------------------------------------------------------------------------
    glm::quat AnimationState::ProcessKeyframes(
      float playback_time,
      const foundation::Vector<AnimationQuaternionKey>& keys) const
    {
      if (keys.size() == 0)
      {
        return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
      }
      else if (keys.size() == 1)
      {
        return keys[0].value;
      }
      else if (keys.size() > 1)
      {
        unsigned int current_key_index = UINT_MAX;
        unsigned int next_key_index = UINT_MAX;

        for (size_t i = 0; i < keys.size(); ++i)
        {
          if (playback_time < keys[i].time)
          {
            current_key_index = static_cast<unsigned int>(i);
          }
        }

        if (current_key_index == UINT_MAX)
        {
          return keys[0].value;
        }

        next_key_index = static_cast<unsigned int>(fmod(current_key_index + 1, keys.size()));

        float delta_time = keys[next_key_index].time - keys[current_key_index].time;
        float factor = (playback_time - keys[current_key_index].time) / delta_time;

        const glm::quat& start = keys[current_key_index].value;
        const glm::quat& end = keys[next_key_index].value;

        return glm::normalize(glm::slerp(start, end, factor));
      }

      return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

    //------------------------------------------------------------------------------------------------------
    unsigned int AnimationState::GetAnimationChannelByBoneIndex(unsigned int bone_index) const
    {
      const foundation::Vector<AnimationChannel>& channels = animation_->animation_channels();
      
      const foundation::UnorderedMap<foundation::String, unsigned int>& bone_names = skeleton_->bone_names();

      for (size_t i = 0; i < channels.size(); ++i)
      {
        foundation::UnorderedMap<foundation::String, unsigned int>::const_iterator found_bone =
          bone_names.find(channels[i].bone_name);

        if (found_bone != bone_names.end() && bone_index == skeleton_->bone_names().at(channels[i].bone_name))
        {
          return static_cast<unsigned int>(i);
        }
      }

      return UINT_MAX;
    }

    //------------------------------------------------------------------------------------------------------
    void AnimationState::Play()
    {
      is_playing_ = true;
    }

    //------------------------------------------------------------------------------------------------------
    void AnimationState::Pause()
    {
      is_playing_ = false;
    }

    //------------------------------------------------------------------------------------------------------
    void AnimationState::SetPlaying(bool playing)
    {
      is_playing_ = playing;
    }

    //------------------------------------------------------------------------------------------------------
    bool AnimationState::TogglePlaying()
    {
      is_playing_ = !is_playing_;
      return is_playing_;
    }

    //------------------------------------------------------------------------------------------------------
    bool AnimationState::IsPlaying() const
    {
      return is_playing_;
    }

    //------------------------------------------------------------------------------------------------------
    void AnimationState::SetPlaybackTime(float time)
    {
      playback_time_ = time;
    }

    //------------------------------------------------------------------------------------------------------
    float AnimationState::GetPlaybackTime() const
    {
      return playback_time_;
    }

    //------------------------------------------------------------------------------------------------------
    void AnimationState::SetPlaybackMultiplier(float multiplier)
    {
      playback_multiplier_ = multiplier;
    }

    //------------------------------------------------------------------------------------------------------
    float AnimationState::GetPlaybackMultiplier() const
    {
      return playback_multiplier_;
    }
    
    //------------------------------------------------------------------------------------------------------
    void AnimationState::SetSkeleton(SkeletonHandle skeleton)
    {
      skeleton_ = skeleton;
    }
    
    //------------------------------------------------------------------------------------------------------
    SkeletonHandle AnimationState::GetSkeleton() const
    {
      return skeleton_;
    }
    
    //------------------------------------------------------------------------------------------------------
    void AnimationState::SetAnimation(AnimationHandle animation)
    {
      animation_ = animation;
    }
    
    //------------------------------------------------------------------------------------------------------
    AnimationHandle AnimationState::GetAnimation() const
    {
      return animation_;
    }
  }
}