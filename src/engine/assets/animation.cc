#include "animation.h"

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------------------------------
    Animation::Animation() :
      duration_(0.0f),
      ticks_per_second_(1.0f)
    {

    }

    //------------------------------------------------------------------------------------------------------
    Animation::Animation(
      const foundation::Vector<AnimationChannel>& animation_channels, 
      float duration, 
      float ticks_per_second
    ) :
      duration_(duration),
      ticks_per_second_(ticks_per_second),
      animation_channels_(animation_channels)
    {

    }

    //------------------------------------------------------------------------------------------------------
    Animation::Animation(const foundation::AnimationData& animation_data)
    {
      duration_ = animation_data.duration;
      ticks_per_second_ = animation_data.ticks_per_second;
      
      animation_channels_.resize(animation_data.channels.size());
      for (int i = 0; i < animation_data.channels.size(); ++i)
      {
        const foundation::AnimationChannel& loaded_channel = animation_data.channels[i];
        AnimationChannel& new_channel = animation_channels_[i];
        new_channel.bone_name = loaded_channel.bone_name;

        new_channel.position_keys.resize(loaded_channel.position_keys.size());
        for (int j = 0; j < loaded_channel.position_keys.size(); ++j)
        {
          new_channel.position_keys[j].time = loaded_channel.position_keys[j].time;
          new_channel.position_keys[j].value = loaded_channel.position_keys[j].vector;
        }

        new_channel.rotation_keys.resize(loaded_channel.rotation_keys.size());
        for (int j = 0; j < loaded_channel.rotation_keys.size(); ++j)
        {
          new_channel.rotation_keys[j].time = loaded_channel.rotation_keys[j].time;
          new_channel.rotation_keys[j].value = loaded_channel.rotation_keys[j].quaternion;
        }

        new_channel.scale_keys.resize(loaded_channel.scale_keys.size());
        for (int j = 0; j < loaded_channel.scale_keys.size(); ++j)
        {
          new_channel.scale_keys[j].time = loaded_channel.scale_keys[j].time;
          new_channel.scale_keys[j].value = loaded_channel.scale_keys[j].vector;
        }
      }
    }

    //------------------------------------------------------------------------------------------------------
    float Animation::duration() const
    {
      return duration_;
    }

    //------------------------------------------------------------------------------------------------------
    void Animation::set_duration(float duration)
    {
      duration_ = duration;
    }

    //------------------------------------------------------------------------------------------------------
    float Animation::ticks_per_second() const
    {
      return ticks_per_second_;
    }

    //------------------------------------------------------------------------------------------------------
    void Animation::set_ticks_per_second(float ticks_per_second)
    {
      ticks_per_second_ = ticks_per_second;
    }

    //------------------------------------------------------------------------------------------------------
    const foundation::Vector<AnimationChannel>& Animation::animation_channels() const
    {
      return animation_channels_;
    }

    //------------------------------------------------------------------------------------------------------
    void Animation::set_animation_channels(const foundation::Vector<AnimationChannel>& animation_channels)
    {
      animation_channels_ = animation_channels;
    }
  }
}