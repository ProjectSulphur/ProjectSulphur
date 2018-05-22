#pragma once

/**
* @typedef sulphur::engine::Window
* @brief Alias pointing to the correct window implementation
*/

#ifdef PS_WIN32
#include "engine/win32/win32_window.h"

namespace sulphur
{
  namespace engine
  {
    using Window = Win32Window;
  }
}
#endif

#ifdef PS_PS4
#include "engine/ps4/ps4_window.h"

namespace sulphur
{
  namespace engine
  {
    using Window = PS4Window;
  }
}
#endif
