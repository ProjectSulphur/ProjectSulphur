#pragma once

#ifdef WIN32
#include "../win32/win32_input_server.h"
namespace sulphur
{
  namespace engine
  {
    using InputServer = Win32InputServer;
  }
}
#endif
