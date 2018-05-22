#pragma once

namespace sulphur
{
  namespace foundation
  {
    /**
     * @struct sulphur::engine::NativeWindowHandle
     * @brief Struct for exposing native window handles for the different platforms
     */
    struct NativeWindowHandle
    {
      /**
       * @enum sulphur::engine::NativeWindowHandle::Type
       * @brief 
       */
      enum struct Type
      {
        kWin32, //!< Indicates a win32 window handle is contained
      } type;   //!< The type of window handle contained

      union
      {
        void* win32_window; //!< Native win32 window handle
      };
    };
  }
}
