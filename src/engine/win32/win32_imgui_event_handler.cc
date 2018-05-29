#include "engine/win32/win32_imgui_event_handler.h"

#include <Windows.h>
#include "graphics/d3d11/imgui_impl.h"
#include "foundation/logging/logger.h"

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    bool Win32ImguiEventHandler::OnWin32Event(void*,
      uint message, uint64_t word_param, int64_t long_param)
    {
      ImGuiIO& io = ImGui::GetIO();

      switch (message)
      {
      case WM_LBUTTONDOWN:
        io.MouseDown[0] = true;
        return io.WantCaptureMouse;
      case WM_LBUTTONUP:
        io.MouseDown[0] = false;
        return io.WantCaptureMouse;
      case WM_RBUTTONDOWN:
        io.MouseDown[1] = true;
        return io.WantCaptureMouse;
      case WM_RBUTTONUP:
        io.MouseDown[1] = false;
        return io.WantCaptureMouse;
      case WM_MBUTTONDOWN:
        io.MouseDown[2] = true;
        return io.WantCaptureMouse;
      case WM_MBUTTONUP:
        io.MouseDown[2] = false;
        return io.WantCaptureMouse;
      case WM_MOUSEWHEEL:
        io.MouseWheel += GET_WHEEL_DELTA_WPARAM(word_param) > 0 ? +1.0f : -1.0f;
        return io.WantCaptureMouse;
      case WM_MOUSEMOVE:
        io.MousePos.x = static_cast<signed short>(long_param);
        io.MousePos.y = static_cast<signed short>(long_param >> 16);
        return io.WantCaptureMouse;
      case WM_KEYDOWN:
        if (word_param < 256)
          io.KeysDown[word_param] = 1;
        return io.WantCaptureKeyboard;
      case WM_KEYUP:
        if (word_param < 256)
          io.KeysDown[word_param] = 0;
        return io.WantCaptureKeyboard;
      case WM_CHAR:
        // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
        if (word_param > 0 && word_param < 0x10000)
          io.AddInputCharacter(static_cast<unsigned short>(word_param));
        return io.WantTextInput;
      default:
        return false;
      }
    }
  }
}
