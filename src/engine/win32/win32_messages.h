#pragma once
#include "foundation/utils/type_definitions.h"

namespace sulphur
{
  namespace engine
  {
    /**
     * @brief Custom win32 messages defined by the sulphur engine
     * @author Kenneth Buijssen
     */
    enum struct Win32Messages : uint
    {
      kEnterFullscreen = 0x0400, //!< Message send when entering fullscren
      kExitFullscreen            //!< Message send when exiting fullscreen
    };
  }
}
