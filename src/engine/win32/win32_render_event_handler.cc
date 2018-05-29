#include "engine/win32/win32_render_event_handler.h"

#include "engine/graphics/irenderer.h"
#include <Windows.h>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    Win32RenderEventHandler::Win32RenderEventHandler(
      foundation::SharedPointer<IRenderer> renderer):
      renderer_(renderer)
    {
    }

    //--------------------------------------------------------------------------
    bool Win32RenderEventHandler::OnWin32Event(void*,
      uint message, uint64_t, int64_t long_param)
    {
      switch (message)
      {
      case WM_SIZE:
        {
          const int width = LOWORD(long_param);
          const int height = HIWORD(long_param);

          renderer_->OnResizeWindow(width, height);
          return true;
        }
      default:
        return false;
      }

    }
  }
}