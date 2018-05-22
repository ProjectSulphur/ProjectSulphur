#pragma once

#include <foundation/utils/frame.h>

namespace sulphur
{
  namespace engine
  {
    /**
    * @class sulphur::engine::FrameScheduler
    * @brief Helper class for managing frame updates
    * @author Kenneth Buijssen
    */
    class FrameScheduler : public foundation::Frame
    {
    public:
      /**
       * @brief Create an update scheduler with the specified number of fixed 
       *        updates a second.
       * @param[in] fixed_updates_per_second (uint)
       *            The number of fixed updates to execute per second
       */
      FrameScheduler(uint fixed_updates_per_second = 50);

      /**
       * @brief Try to start a frame with the specified amout of time elapsed
       * @param[in] delta_time (float) 
       *            The amount of time elapsed since the previous call to TryStartFrame
       * @return True if a new frame was started
       */
      bool TryStartFrame(float delta_time);

      /**
      * @brief Try to start a fixed update
      * @return True if a fixed update should be executed
      */
      bool TryStartFixed();

    private:
      float accumulator_;         //<! Time not consumed by fixed updates 
      float fps_time_elapsed_;    //<! Time elapsed in the current fps measurement
      uint fps_num_frames_;       //<! Number of frames in the current fps measurement
    };
  }
}
