#pragma once

/**
* @typedef sulphur::engine::Window
* @brief Alias pointing to the correct platform implementation
*/

#ifdef PS_WIN32
#include "engine/win32/win32_platform.h"

namespace sulphur
{
  namespace engine
  {
    using Platform = Win32Platform;
  }
}
#endif