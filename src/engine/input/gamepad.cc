#include "gamepad.h"

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    void Gamepad::StartFrame()
    {
      present_ = next_present_;

      memcpy(prev_buttons_, buttons_, sizeof buttons_);
      memcpy(buttons_, next_buttons_, sizeof buttons_);

      memcpy(axes_, next_axes_, sizeof axes_);
    }
  }
}