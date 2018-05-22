#include "mouse.h"

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    void Mouse::StartFrame()
    {
      present_ = next_present_;

      prev_position_ = position_;
      position_ = next_position_;

      scroll_offset_ = next_scroll_offset_;

      memcpy(prev_buttons_, buttons_, sizeof buttons_);
      memcpy(buttons_, next_buttons_, sizeof buttons_);
    }
  }
}