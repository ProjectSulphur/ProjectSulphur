#pragma once 
#include "engine/audio/audio_structs.h"

namespace sulphur
{
  namespace engine
  {
    class AudioSystem;
    /**
    * @class sulphur::engine::AudioListener
    * @brief Interfaces to the FMod sytem to provide listener functionality
    * @author Rodi Lankester
    */
    class AudioListener
    {
      friend class AudioSystem;
    protected:
      /**
      * @brief Create the audio listener
      * @param[in] audio_system (AudioSystem*) A pointer to the audio system
      * @param[in] listener_id (int) The listener ID
      */
      AudioListener(AudioSystem* audio_system, int listener_id);
    public:
      /**
      * @brief Get this listener's weight
      * @returns (float) This listener's weight
      */
      float GetWeight();
      /**
      * @brief Set this listener's weight 
      * @param[in] (float) This listeners weight
      */
      void SetWeight(float weight);

      /**
      * @brief Get this listener's 3D attributes
      * @returns (Audio3DAttributes) This listener's 3D attributes
      */
      Audio3DAttributes Get3DAttributes();
      /**
      * @brief Set this listener's 3D attributes
      * @param[in] attributes (Audio3DAttributes) This listeners 3D attributes
      */
      void Set3DAttributes(const Audio3DAttributes& attributes);
    protected:
      int listener_id_; //!< This listener's id
      AudioSystem* audio_system_; //!< A pointer to the audio system
    };
  }
}