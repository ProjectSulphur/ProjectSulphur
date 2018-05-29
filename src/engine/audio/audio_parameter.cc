#include "engine/audio/audio_parameter.h"
#include "engine/audio/audio_utils.h"

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------
    AudioParameter::AudioParameter(FMOD::Studio::ParameterInstance* instance)
    {
      instance_ = instance;
      FMOD_STUDIO_PARAMETER_DESCRIPTION desc;
      AudioUtils::CheckError(instance->getDescription(&desc));

      name_ = desc.name;
      index_ = desc.index;
      min_value_ = desc.minimum;
      max_value_ = desc.maximum;
      default_value_ = desc.maximum;
      type_ = static_cast<AudioParameterType>(desc.type);
    }

    //------------------------------------------------------------------------------
    float AudioParameter::GetValue()
    {
      float value = 0;
      AudioUtils::CheckError(instance_->getValue(&value));
      return value;
    }

    //------------------------------------------------------------------------------
    void AudioParameter::SetValue(float value)
    {
      AudioUtils::CheckError(instance_->setValue(value));
    }

    //------------------------------------------------------------------------------
    const char* AudioParameter::name()
    {
      return name_;
    }

    //------------------------------------------------------------------------------
    int AudioParameter::index()
    {
      return index_;
    }

    //------------------------------------------------------------------------------
    float AudioParameter::min_value()
    {
      return min_value_;
    }

    //------------------------------------------------------------------------------
    float AudioParameter::max_value()
    {
      return max_value_;
    }

    //------------------------------------------------------------------------------
    float AudioParameter::default_value()
    {
      return default_value_;
    }

    //------------------------------------------------------------------------------
    AudioParameterType AudioParameter::type()
    {
      return type_;
    }
  }
}