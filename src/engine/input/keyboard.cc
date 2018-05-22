#include "keyboard.h"

#include <cstring>

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    void Keyboard::StartFrame()
    {
      present_ = next_present_;

      memcpy(prev_buttons_, buttons_, sizeof buttons_);
      memcpy(buttons_, next_buttons_, sizeof buttons_);
    }
  }
}