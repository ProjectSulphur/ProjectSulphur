#pragma once

namespace sulphur
{
  namespace foundation
  {
    /**
     * @enum sulphur::foundation::WindowMode
     * @brief Different window modes
     */
    enum struct WindowMode
    {
      kWindowed,  //<! Decorated window
      kBorderless, //<! Undecorated window
      kFullscreen //<! Exlusive fullscreen
    };
  }
}
