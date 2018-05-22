#include "engine/application/frame_scheduler.h"

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    FrameScheduler::FrameScheduler(uint fixed_updates_per_second) :
      accumulator_(0.0f),
      fps_time_elapsed_(0.0f),
      fps_num_frames_(0)
    {
      frame_count_ = 0;
      frame_time_ = 0.0f;
      delta_time_ = 0.0f;

      fixed_delta_time_ = 0;
      fixed_time_ = 0.0f;
      fixed_delta_time_ = 1.0f / fixed_updates_per_second;
    }

    //--------------------------------------------------------------------------
    bool FrameScheduler::TryStartFrame(float delta_time)
    {
      // If the delta time was to large we skip the frame
      if (delta_time > 0.6f)
      {
        return false;
      }

      ++frame_count_;
      delta_time_ = delta_time;

      frame_time_ += delta_time;
      accumulator_ += delta_time;

      // Keep track of the number of frames per second
      fps_time_elapsed_ += delta_time;
      ++fps_num_frames_;

      if (fps_time_elapsed_ > 1.0f)
      {
        fps_ = fps_num_frames_;

        fps_time_elapsed_ -= 1.0f;
        fps_num_frames_ = 0;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    bool FrameScheduler::TryStartFixed()
    {
      if (accumulator_ < fixed_delta_time_)
      {
        return false;
      }

      ++fixed_count_;
      fixed_time_ += fixed_delta_time_;
      accumulator_ -= fixed_delta_time_;
      
      return true;
    }
  }
}