#include "foundation/utils/frame.h"

namespace sulphur
{
  namespace foundation
  {
    uint  Frame::frame_count_;
    float Frame::frame_time_;
    float Frame::delta_time_;
    uint  Frame::fixed_count_;
    float Frame::fixed_time_;
    float Frame::fixed_delta_time_;
    uint Frame::fps_;

    //--------------------------------------------------------------------------
    uint Frame::frame_count()
    {
      return frame_count_;
    }

    //--------------------------------------------------------------------------
    float Frame::frame_time()
    {
      return frame_time_;
    }

    //--------------------------------------------------------------------------
    float Frame::fixed_delta_time()
    {
      return fixed_delta_time_;
    }

    //--------------------------------------------------------------------------
    uint Frame::fps()
    {
      return fps_;
    }

    //--------------------------------------------------------------------------
    float Frame::delta_time()
    {
      return delta_time_;
    }

    //--------------------------------------------------------------------------
    uint Frame::fixed_count()
    {
      return fixed_count_;
    }

    //--------------------------------------------------------------------------
    float Frame::fixed_time()
    {
      return fixed_time_;
    }
  }
}