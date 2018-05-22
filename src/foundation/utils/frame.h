#pragma once
#include "type_definitions.h"

namespace sulphur
{
  namespace foundation
  {
    /**
     * @class sulphur::foundation::Frame
     * @brief Interface for retrieving frame data
     * @author Kenneth Buijssen
     */
    class Frame
    {
    public:
      /** @see frame_count_ */
      static uint frame_count();
      
      /** @see frame_time_ */
      static float frame_time();
      
      /** @see delta_time_ */
      static float delta_time();
      
      /** @see fps_ */
      static uint fixed_count();
      
      /** @see fixed_count_ */
      static float fixed_time();
      
      /** @see fixed_time_ */
      static float fixed_delta_time();
      
      /** @see fps_ */
      static uint fps();

    protected:
      static uint frame_count_;       //!< The number of the current frame
      static float frame_time_;       //!< The time at the start of the frame
      static float delta_time_;       //!< Time it took to complete the previous frame

      static uint fixed_count_;       //!< The number of the fixed update
      static float fixed_time_;       //!< The time at the start of the fixed update
      static float fixed_delta_time_; //!< The fixed delta time of the current frame

      static uint fps_;               //!< The number of frames per second
    };
  }
}
