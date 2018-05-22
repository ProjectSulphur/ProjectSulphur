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
#else
#include "../ps4/ps4_input_server.h"
namespace sulphur
{
  namespace engine
  {
    using InputServer = PS4InputServer;
  }
}
#endif
