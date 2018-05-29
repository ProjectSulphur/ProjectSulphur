#pragma once
#include <fmod_errors.h>
#include <fmod_studio.hpp>

#include "engine/audio/audio_structs.h"
namespace sulphur
{
  namespace engine
  {
    /**
    * @class sulphur::engine::AudioUtils
    * @brief Provides utility function for the audio system
    * @author Rodi Lankester
    */
    class AudioUtils
    {
    public:
      /**
      * @brief Check the result of a FMod call and log an error message if it gave an error
      * @param[in] (FMOD_RESULT) The result of the FMod call
      */
      static void CheckError(FMOD_RESULT res);

      /**
      * @brief Convert FMOD_3D_ATTRIBUTES to Audio3DAttributes
      * @param[in] attributes (const FMOD_3D_ATTRIBUTES&) The attributes to convert
      * @returns (Audio3DAttributes) The converted attributes
      */
      static Audio3DAttributes FModAttributesToEngine(const FMOD_3D_ATTRIBUTES& attributes);
      /**
      * @brief Convert Audio3DAttributes to FMOD_3D_ATTRIBUTES
      * @param[in] attributes (const Audio3DAttributes&) The attributes to convert
      * @returns (FMOD_3D_ATTRIBUTES) The converted attributes
      */
      static FMOD_3D_ATTRIBUTES EngineAttributesToFMod(const Audio3DAttributes& attributes);

      /**
      * @brief Convert a FMOD_VECTOR to a glm::vec3
      * @param[in] vec (const FMOD_VECTOR&) The vector to convert
      * @returns (glm::vec3) The converted vector
      */
      static glm::vec3 FModVecToGLM(const FMOD_VECTOR& vec);
      /**
      * @brief Convert a glm::vec3 to a FMOD_VECTOR
      * @param[in] vec (const glm::vec3&) The vector to convert
      * @returns (FMOD_VECTOR) The converted vector
      */
      static FMOD_VECTOR GLMVecToFMod(const glm::vec3& vec);
    };
  }
}