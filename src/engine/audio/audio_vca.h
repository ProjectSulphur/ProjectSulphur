#pragma once
#include <foundation/containers/string.h>

namespace FMOD
{
  namespace Studio
  {
    class VCA;
  }
}

namespace sulphur
{
  namespace engine
  {
    /**
    * @class sulphur::engine::AudioVCA
    * @brief Interfaces to the FMod sytem to provide VCA functionality
    * @author Rodi Lankester
    */
    class AudioVCA
    {
      friend class AudioBank;
    protected: 
      /**
      * @brief Create an AudioVCA instance from a FMOD::Studio::VCA instance
      * @param[in] fmod_vca (FMOD::Studio::VCA*) The FMOD::Studio::VCA instance
      */
      AudioVCA(FMOD::Studio::VCA* fmod_vca);
    public:
      /**
      * @brief Get the path of the VCA
      * @returns (foundation::String) The path
      */
      foundation::String GetPath();

      /**
      * @brief Get the volume of the VCA
      * @returns (float) The volume
      */
      float GetVolume();
      /**
      * @brief Set the volume of the VCA
      * @returns (float) The volume
      */
      void SetVolume(float volume);
    protected:
      FMOD::Studio::VCA* fmod_vca_; //!< The FMOD::Studio::VCA instance 
    };
  }
}