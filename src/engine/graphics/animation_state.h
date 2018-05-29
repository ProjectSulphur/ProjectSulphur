#pragma once

#include "engine/assets/skeleton.h"
#include "engine/assets/animation.h"

#include <foundation/containers/vector.h>

#include <glm/mat4x4.hpp>

namespace sulphur
{
  namespace engine
  {
    /**
    * @class sulphur::engine::AnimationState
    * @brief Controls the state of an animation based on a Skeleton and an Animation.
    * @author Riko Ophorst
    */
    class AnimationState
    {
    public:
      /**
      * @brief Default constructor. Constructs an empty AnimationState.
      */
      AnimationState();

      /**
      * @brief Constructs an AnimationState based on a Skeleton.
      * @param[in] skeleton (sulphur::engine::SkeletonHandle) The Skeleton to be part of this AnimationState.
      */
      AnimationState(SkeletonHandle skeleton);

      /**
      * @brief Constructs an AnimationState based on an Animation.
      * @param[in] animation (sulphur::engine::AnimationHandle) The Animation to be part of this AnimationState.
      */
      AnimationState(AnimationHandle animation);
      

      /**
      * @brief Constructs an AnimationState based on a Skeleton and an Animation.
      * @param[in] skeleton (sulphur::engine::SkeletonHandle) The Skeleton to be part of this AnimationState.
      * @param[in] animation (sulphur::engine::AnimationHandle) The Animation to be part of this AnimationState.
      */
      AnimationState(SkeletonHandle skeleton, AnimationHandle animation);

      /**
      * @brief Updates the AnimationState.
      * @remarks This is the function that will start off the recalculation for 
      *          the bone transforms. It all gets kicked off from here. This
      *          should be called every frame, by the SkinnedMeshRenderSystem.
      */
      void Update();

      /**
      * @brief Returns the cached transformed bone matrices, ready to be uploaded to a GPU buffer.
      * @returns (const foundation::Vector<glm::mat4>&) The cached transformed bone matrices, ready to be uploaded to a GPU buffer.
      */
      const foundation::Vector<glm::mat4>& GetTransformedBones();

      /**
      * @brief Calculates the bone transform for a specific bone at a specific point in local animation playback time.
      * @param[in] bone_index (unsigned int) The index of the bone in the Skeleton to be transformed.
      * @param[in] playback_time (float) The playback time of the animation in keyframe ticks.
      * @returns (glm::mat4) The transformed bone matrix.
      * @remarks This function is recursive. It will call itself based on the inserted bone's parent.
      */
      glm::mat4 CalculateBoneTransform(unsigned int bone_index, float playback_time) const;

      /**
      * @brief Processes a set of AnimationVectorKeys to return the current vector value based on a given playback time.
      * @param[in] playback_time (float) The playback time of the animation in keyframe ticks.
      * @param[in] keys (const sulphur::foundation::Vector<AnimationVectorKey>&) The vector keyframes to process.
      * @returns The current vector value based on the given playback time.
      */
      glm::vec3 ProcessKeyframes(float playback_time, const foundation::Vector<AnimationVectorKey>& keys) const;
      
      /**
      * @brief Processes a set of AnimationQuaternionKey to return the current quaternion value based on a given playback time.
      * @param[in] playback_time (float) The playback time of the animation in keyframe ticks.
      * @param[in] keys (const sulphur::foundation::Vector<AnimationQuaternionKey>&) The quaternion keyframes to process.
      * @returns The current quaternion value based on the given playback time.
      */
      glm::quat ProcessKeyframes(float playback_time, const foundation::Vector<AnimationQuaternionKey>& keys) const;

      /**
      * @brief Retrieves an index to an AnimationChannel in the Animation based on the given bone's name.
      * @param[in] bone_index (unsigned int) The index of the bone of which you want to find the AnimationChannel.
      * @returns (unsigned int) An index into this AnimationState's animation channels array.
      * @remarks This function can also return UINT_MAX in case that there is no animation channel that
      *          matches with the name of the bone.
      */
      unsigned int GetAnimationChannelByBoneIndex(unsigned int bone_index) const;

      /**
      * @brief Starts playing the animation from its current playback time.
      * @remarks Doesn't modify the current playback time.
      */
      void Play();

      /**
      * @brief Pauses the animation at its current playback time.
      * @remarks Doesn't modify the current playback time.
      */
      void Pause();

      /**
      * @brief Sets whether the animation is currently playing or not.
      * @param[in] playing (bool) Whether the animation should be playing or not.
      * @remarks This makes the animation resume playing back from its current playback time.
      */
      void SetPlaying(bool playing);

      /**
      * @brief Toggles whether the animation is currently playing or not.
      * @returns (bool) Whether the animation will now be playing or not.
      */
      bool TogglePlaying();

      /**
      * @brief Retrieves whether the animation is playing back or not.
      * @returns (bool) Whether the animation is playing back or not.
      */
      bool IsPlaying() const;

      /**
      * @brief Sets the playback time of the animation.
      * @param[in] time (float) The new playback time of the animation in seconds.
      * @remarks This does not start or pause playing back the animation.
      */
      void SetPlaybackTime(float time);

      /**
      * @brief Retrieves the current playback time of the animation in seconds.
      * @returns (float) The current playback time of the animation in seconds.
      */
      float GetPlaybackTime() const;
      
      /**
      * @brief Sets the playback speed multiplier.
      * @param[in] multiplier (float) The multiplier that the animation should be played back at.
      * @remarks Default value is 1.0f. The higher the multiplier, the quicker the animation
      *          gets played back. The lower the number, the slower the animation gets played
      *          back.
      */
      void SetPlaybackMultiplier(float multiplier);

      /**
      * @brief Retrievess the playback speed multiplier.
      * @returns (float) The multiplier that the animation is played back at.
      * @remarks Default value is 1.0f. The higher the multiplier, the quicker the animation
      *          gets played back. The lower the number, the slower the animation gets played
      *          back.
      */
      float GetPlaybackMultiplier() const;

      /**
      * @brief Sets the Skeleton that this AnimationState is based on.
      * @param[in] skeleton (sulphur::engine::SkeletonHandle) The Skeleton to attach to this AnimationState.
      */
      void SetSkeleton(SkeletonHandle skeleton);

      /**
      * @brief Retrieves the Skeleton that this AnimationState is based on.
      * @returns (sulphur::engine::SkeletonHandle) The Skeleton attached to this AnimationState.
      */
      SkeletonHandle GetSkeleton() const;

      /**
      * @brief Sets the Animation that this AnimationState is based on.
      * @param[in] skeleton (sulphur::engine::SkeletonHandle) The Animation to attach to this AnimationState.
      */
      void SetAnimation(AnimationHandle animation);

      /**
      * @brief Retrieves the Animation that this AnimationState is based on.
      * @returns (sulphur::engine::AnimationHandle) The Animation attached to this AnimationState.
      */
      AnimationHandle GetAnimation() const;

    private:
      bool is_playing_; //!< Whether the animation is being played back.
      float playback_time_; //!< The current playback time of the animation.
      float playback_multiplier_; //!< The multiplier that gets applied to when the delta time to increase the playback time each update.

      SkeletonHandle skeleton_; //!< The Skeleton of this AnimationState.
      AnimationHandle animation_; //!< The Animation of this AnimationState.

      foundation::Vector<glm::mat4> transformed_bones_; //!< Cached array of transformed bone matrices, uploadable to the GPU.
    };
  }
}