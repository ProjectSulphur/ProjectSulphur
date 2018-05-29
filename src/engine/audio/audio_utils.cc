#include "engine/audio/audio_utils.h"
#include <foundation/logging/logger.h>

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------
    void AudioUtils::CheckError(FMOD_RESULT res)
    {
      if (res != FMOD_OK)
      {
        const char* error = FMOD_ErrorString(res);
        PS_LOG(Error, "FMod error: (%d) %s", res, error);
      }
    }

    //------------------------------------------------------------------------------
    Audio3DAttributes AudioUtils::FModAttributesToEngine(const FMOD_3D_ATTRIBUTES& attributes)
    {
      Audio3DAttributes engine_attributes;
      engine_attributes.position = FModVecToGLM(attributes.position);
      engine_attributes.forward = FModVecToGLM(attributes.forward);
      engine_attributes.up = FModVecToGLM(attributes.up);
      engine_attributes.velocity = FModVecToGLM(attributes.velocity);

      return engine_attributes;
    }

    //------------------------------------------------------------------------------
    FMOD_3D_ATTRIBUTES AudioUtils::EngineAttributesToFMod(const Audio3DAttributes& attributes)
    {
      FMOD_3D_ATTRIBUTES fmod_attributes;
      fmod_attributes.position = GLMVecToFMod(attributes.position);
      fmod_attributes.forward = GLMVecToFMod(attributes.forward);
      fmod_attributes.up = GLMVecToFMod(attributes.up);
      fmod_attributes.velocity = GLMVecToFMod(attributes.velocity);

      return fmod_attributes;
    }

    //------------------------------------------------------------------------------
    glm::vec3 AudioUtils::FModVecToGLM(const FMOD_VECTOR& vec)
    {
      glm::vec3 glm_vec;

      glm_vec.x = vec.x;
      glm_vec.y = vec.y;
      glm_vec.z = vec.z;

      return glm_vec;
    }

    //------------------------------------------------------------------------------
    FMOD_VECTOR AudioUtils::GLMVecToFMod(const glm::vec3& vec)
    {
      FMOD_VECTOR fmod_vec;

      fmod_vec.x = vec.x;
      fmod_vec.y = vec.y;
      fmod_vec.z = vec.z;

      return fmod_vec;
    }
  }
}