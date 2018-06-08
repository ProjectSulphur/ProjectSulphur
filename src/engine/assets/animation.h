#pragma once

#include "engine/assets/asset_interfaces.h"

#include <foundation/pipeline-assets/animation.h>

namespace sulphur
{
  namespace engine
  {
    /**
    * @struct sulphur::engine::Keyframe
    * @brief Stores a timestamp in ticks and a value of any type, useful for animations.
    * @tparam T The type of value that you want to be keyframed.
    * @author Riko Ophorst
    */
    template<typename T>
    struct Keyframe
    {
      float time; //!< The time in ticks that this value should be 100% effective at.
      T value;    //!< The value of the keyframe. Can be anything, really.
    };

    using FloatKeyframe = Keyframe<float>;
    using Vector2Keyframe = Keyframe<glm::vec2>;
    using Vector3Keyframe = Keyframe<glm::vec3>;
    using Vector4Keyframe = Keyframe<glm::vec4>;
    using QuaternionKeyframe = Keyframe<glm::quat>;

    /**
    * @struct sulphur::engine::AnimationChannel
    * @brief Stores all the keyframes for a specific bone in an animation.
    * @remarks This is made up of three sets of keyframes. Each set of keyframes
    *          is meant for one specific property of a bone in an animation.
    * @author Riko Ophorst
    */
    struct AnimationChannel
    {
      foundation::String bone_name;                             //!< The bone that gets influenced by the keyframes in this channel.
      foundation::Vector<Vector3Keyframe> position_keys;        //!< The various keyframes that relate to the position of the bone.
      foundation::Vector<QuaternionKeyframe> rotation_keys;     //!< The various keyframes that relate to the rotation of the bone.
      foundation::Vector<Vector3Keyframe> scale_keys;           //!< The various keyframes that relate to the scaling of the bone.
    };

    /**
    * @class sulphur::engine::Animation
    * @brief Represents an animation which can be applied to a Skeleton.
    * @author Riko Ophorst
    */
    class Animation
    {
    public:
      /** 
      * @brief Default constructor.
      */
      Animation();

      /**
      * @brief Constructs an Animation based on a given set of inputs, customizable by the user.
      * @param[in] animation_channels (const sulphur::foundation::Vector<AnimationChannel>&) The various AnimationChannels in this Animation.
      * @param[in] duration (float) The duration of the animation in ticks.
      * @param[in] ticks_per_second (float) The ticks per second of the animation (higher means quicker playback).
      */
      Animation(
        const foundation::Vector<AnimationChannel>& animation_channels, 
        float duration, 
        float ticks_per_second
      );

      /**
      * @brief Constructs an Animation based off a preprocessed animation (AnimationData) by the Sulphur Builder.
      * @param[in] animation_data (const sulphur::foundation::AnimationData&) The preprocessed animation data.
      */
      Animation(const foundation::AnimationData& animation_data);

      /**
      * @brief Retrieve the duration of the animation in ticks.
      * @returns (float) The duration of the animation in ticks. 
      */
      float duration() const;

      /**
      * @brief Sets the duration of the animation ticks.
      * @param[in] duration (float) The duration of the animation in ticks.
      */
      void set_duration(float duration);

      /**
      * @brief Retrieve the playback speed of the animation in ticks per second.
      * @returns (float) The playback speed of the animation in ticks per second.
      */
      float ticks_per_second() const;

      /**
      * @brief Sets the duration of the animation in ticks.
      * @param[in] ticks (float) The duration of the animation in ticks.
      */
      void set_ticks_per_second(float ticks_per_second);

      /**
      * @brief Retrieve the set of AnimationChannels in this Animation.
      * @returns (const sulphur::foundation::Vector<AnimationChannel>&) The set of AnimationChannels in this Animation.
      * @remarks Every AnimationChannel animates one Bone, linked together via name.
      */
      const foundation::Vector<AnimationChannel>& animation_channels() const;

      /**
      * @brief Sets the set of AnimationChannels in this Animation.
      * @param[in] animation_channels (const sulphur::foundation::Vector<AnimationChannel>&) The set of AnimationChannels in this Animation.
      * @remarks Every AnimationChannel animates one Bone, linked together via name stored inside of the AnimationChannel.
      */
      void set_animation_channels(const foundation::Vector<AnimationChannel>& animation_channels);

    private:
      float duration_;                                          //!< The duration of the animation in ticks.
      float ticks_per_second_;                                  //!< The playback speed of the animation in ticks per second.
      foundation::Vector<AnimationChannel> animation_channels_; //!< The set of AnimationChannels owned by this Animation.
    };

    using AnimationHandle = AssetHandle<Animation>; //!< Asset handle to an Animation
  }
}