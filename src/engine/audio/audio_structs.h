#pragma once
#include <glm/vec3.hpp>
#include <fmod_studio.hpp>

namespace sulphur
{
  namespace engine
  {
    /**
    * @struct sulphur::engine::Audio3DAttributes
    * @brief glm version of FMOD_3D_ATTRIBUTES
    * @author Rodi Lankester
    */
    struct Audio3DAttributes
    {
      glm::vec3 position; //!< The position vector
      glm::vec3 velocity; //!< The velocity vector
      glm::vec3 forward;  //!< The forward vector
      glm::vec3 up;       //!< The up vector
    };

    /**
    * @struct sulphur::engine::PlayBackState
    * @brief Naming scheme conformant version of FMOD_STUDIO_PLAYBACK_STATE
    * @author Rodi Lankester
    */
    enum struct PlaybackState
    {
      kPlaying = FMOD_STUDIO_PLAYBACK_PLAYING,    //!< The event is playing
      kPaused = FMOD_STUDIO_PLAYBACK_SUSTAINING,  //!< The event is paused
      kStopped = FMOD_STUDIO_PLAYBACK_STOPPED,    //!< The event is stopped
      kStarting = FMOD_STUDIO_PLAYBACK_STARTING,  //!< The event is starting
      kStopping = FMOD_STUDIO_PLAYBACK_STOPPING   //!< The event is stopping
    };

    /**
    * @struct sulphur::engine::AudioParameterType
    * @brief Naming scheme conformant version of FMOD_STUDIO_PARAMETER_TYPE
    * @author Rodi Lankester
    */
    enum struct AudioParameterType
    {
      kGameControlled = FMOD_STUDIO_PARAMETER_GAME_CONTROLLED,                              //!< Controlled via the API using AudioEvent::SetParameter. 
      kAutomaticDistance = FMOD_STUDIO_PARAMETER_AUTOMATIC_DISTANCE,                        //!< Distance between the event and the listener. 
      kAutomaticEventConeAngle = FMOD_STUDIO_PARAMETER_AUTOMATIC_EVENT_CONE_ANGLE,          //!< Angle between the event's forward vector and the vector pointing from the event to the listener (0 to 180 degrees). 
      kAutomaticEventOrientation = FMOD_STUDIO_PARAMETER_AUTOMATIC_EVENT_ORIENTATION,       //!< Horizontal angle between the event's forward vector and listener's forward vector (-180 to 180 degrees). 
      kAutomaticDirection = FMOD_STUDIO_PARAMETER_AUTOMATIC_DIRECTION,                      //!< Horizontal angle between the listener's forward vector and the vector pointing from the listener to the event (-180 to 180 degrees). 
      kAutomaticElevation = FMOD_STUDIO_PARAMETER_AUTOMATIC_ELEVATION,                      //!< Angle between the listener's XZ plane and the vector pointing from the listener to the event (-90 to 90 degrees). 
      kAutomaticListenerOrientation = FMOD_STUDIO_PARAMETER_AUTOMATIC_LISTENER_ORIENTATION  //!< Horizontal angle between the listener's forward vector and the global positive Z axis (-180 to 180 degrees). 
    };

    /**
    * @struct sulphur::engine::AudioParameterDescription
    * @brief Naming scheme conformant version of FMOD_STUDIO_PARAMETER_DESCRIPTION
    * @author Rodi Lankester
    */
    struct AudioParameterDescription
    {
      const char *name;         //!< The parameter's name
      int index;                //!< The parameter's index
      float minimum;            //!< The minimum value  
      float maximum;            //!< The maximum value
      float defaultvalue;       //!< The default value
      AudioParameterType type;  //!< The parameter's type
    };
  }
}