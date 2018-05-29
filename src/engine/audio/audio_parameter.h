#pragma once
#include "audio_structs.h"

namespace sulphur
{
  namespace engine
  {
    /**
    * @class sulphur::engine::AudioParameter
    * @brief Interfaces to the FMod sytem to provide event parameter functionality
    * @author Rodi Lankester
    */
    class AudioParameter
    {
      friend class AudioEvent;
      friend class AudioEventDescription;
    protected:
      /**
      * @brief Creates AudioParamater class from FMOD::Studio::ParameterInstance
      * @param[in] instance (FMOD::Studio::ParameterInstance*) The instance
      */
      AudioParameter(FMOD::Studio::ParameterInstance* instance);
    public:
      /**
      * @brief Get the current value of the parameter
      * @returns (float) The current value
      */
      float GetValue();
      /**
      * @brief Set the value of the parameter
      * @param[in] value (float) The current value
      */
      void SetValue(float value);

      /**
      * @brief Get the name of the parameter
      * @returns (const char*) The name
      */
      const char* name();
      /**
      * @brief Get the index of the parameter
      * @returns (int) The index
      */
      int index();
      /**
      * @brief Get the minimum value of the parameter
      * @returns (float) The value
      */
      float min_value();
      /**
      * @brief Get the maximum value of the parameter
      * @returns (float) The value
      */
      float max_value();
      /**
      * @brief Get the default value of the parameter
      * @returns (float) The value
      */
      float default_value();
      /**
      * @brief Get the type of the parameter
      * @returns (AudioParameterType) The type
      */
      AudioParameterType type();
    private:
      const char* name_; //!< The name of the parameter
      int index_; //!< The index of the paramets 
      float min_value_; //!< The minimum value of the parameter
      float max_value_; //!< The maximum value of the parameter
      float default_value_; //!< The default value of the parameter
      AudioParameterType type_; //!< The type of the parameter

      FMOD::Studio::ParameterInstance* instance_; //!< The FMod instance of the parameter
    };
  }
}