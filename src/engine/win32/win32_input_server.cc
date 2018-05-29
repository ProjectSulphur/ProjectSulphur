#include "engine/win32/win32_input_server.h"
#include "engine/application/window.h"
#include "engine/input/input.h"

#include <Windows.h>
#include <windowsx.h>
#include <xinput.h>

namespace sulphur
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    uint64_t WinGetExtendedKeyCode(uint64_t virtual_key, uint64_t long_param)
    {
      UINT scan_code = (long_param & 0x00ff0000) >> 16;
      bool extended = (long_param & 0x01000000) != 0;

      // @todo (Maarten) Process extended versions of numpad keys:
      //   - VK_RETURN (NUMPAD_ENTER)
      //   - VK_INSERT (NUMPAD_0)
      //   - VK_DELETE (NUMPAD_DECIMAL)
      //   - VK_HOME (NUMPAD_7)
      //   - VK_END (NUMPAD_1_
      //   - VK_PRIOR (NUMPAD_9)
      //   - VK_NEXT (NUMPAD_3)
      //   - VK_LEFT (NUMPAD_4)
      //   - VK_RIGHT (NUMPAD_6)
      //   - VK_UP (NUMPAD_8)
      //   - VK_DOWN (NUMPAD_2)
      //   - VK_CLEAR (NUMPAD_5)

      switch (virtual_key)
      {
      case VK_SHIFT:
        // @note (Maarten) The shift key does not set the extended flag which is why we handle
        //   this case individually.
        return MapVirtualKey(scan_code, MAPVK_VSC_TO_VK_EX);
      case VK_CONTROL:
        return extended ? VK_RCONTROL : VK_LCONTROL;
      case VK_MENU:
        return extended ? VK_RMENU : VK_LMENU;
      default:
        return virtual_key;
      }
    }

    //--------------------------------------------------------------------------
    Button WinKeyCodeToButton(uint64_t key_code)
    {
      Button button;
      switch (key_code)
      {
      case VK_SPACE:      button = Button::kSpace;           break;
      case VK_OEM_7:      button = Button::kApostrophe;      break;
      case VK_OEM_COMMA:  button = Button::kComma;           break;
      case VK_OEM_MINUS:  button = Button::kMinus;           break;
      case VK_OEM_PERIOD: button = Button::kPeriod;          break;
      case VK_OEM_2:      button = Button::kSlash;           break;
      case '0':           button = Button::k0;               break;
      case '1':           button = Button::k1;               break;
      case '2':           button = Button::k2;               break;
      case '3':           button = Button::k3;               break;
      case '4':           button = Button::k4;               break;
      case '5':           button = Button::k5;               break;
      case '6':           button = Button::k6;               break;
      case '7':           button = Button::k7;               break;
      case '8':           button = Button::k8;               break;
      case '9':           button = Button::k9;               break;
      case VK_OEM_1:      button = Button::kSemicolon;       break;
      case VK_OEM_PLUS:   button = Button::kEqual;           break;
      case 'A':           button = Button::kA;               break;
      case 'B':           button = Button::kB;               break;
      case 'C':           button = Button::kC;               break;
      case 'D':           button = Button::kD;               break;
      case 'E':           button = Button::kE;               break;
      case 'F':           button = Button::kF;               break;
      case 'G':           button = Button::kG;               break;
      case 'H':           button = Button::kH;               break;
      case 'I':           button = Button::kI;               break;
      case 'J':           button = Button::kJ;               break;
      case 'K':           button = Button::kK;               break;
      case 'L':           button = Button::kL;               break;
      case 'M':           button = Button::kM;               break;
      case 'N':           button = Button::kN;               break;
      case 'O':           button = Button::kO;               break;
      case 'P':           button = Button::kP;               break;
      case 'Q':           button = Button::kQ;               break;
      case 'R':           button = Button::kR;               break;
      case 'S':           button = Button::kS;               break;
      case 'T':           button = Button::kT;               break;
      case 'U':           button = Button::kU;               break;
      case 'V':           button = Button::kV;               break;
      case 'W':           button = Button::kW;               break;
      case 'X':           button = Button::kX;               break;
      case 'Y':           button = Button::kY;               break;
      case 'Z':           button = Button::kZ;               break;
      case VK_OEM_4:      button = Button::kLeftBracket;     break;
      case VK_OEM_5:      button = Button::kBackslash;       break;
      case VK_OEM_6:      button = Button::kRightBracket;    break;
      case VK_OEM_3:      button = Button::kGraveAccent;     break;
      case VK_ESCAPE:     button = Button::kEscape;          break;
      case VK_RETURN:     button = Button::kEnter;           break;
      case VK_TAB:        button = Button::kTab;             break;
      case VK_BACK:       button = Button::kBackspace;       break;
      case VK_INSERT:     button = Button::kInsert;          break;
      case VK_DELETE:     button = Button::kDelete;          break;
      case VK_RIGHT:      button = Button::kRight;           break;
      case VK_LEFT:       button = Button::kLeft;            break;
      case VK_DOWN:       button = Button::kDown;            break;
      case VK_UP:         button = Button::kUp;              break;
      case VK_PRIOR:      button = Button::kPageUp;          break;
      case VK_NEXT:       button = Button::kPageDown;        break;
      case VK_HOME:       button = Button::kHome;            break;
      case VK_END:        button = Button::kEnd;             break;
      case VK_CAPITAL:    button = Button::kCapsLock;        break;
      case VK_SCROLL:     button = Button::kScrollLock;      break;
      case VK_NUMLOCK:    button = Button::kNumLock;         break;
      case VK_SNAPSHOT:   button = Button::kPrintScreen;     break;
      case VK_PAUSE:      button = Button::kPause;           break;
      case VK_F1:         button = Button::kF1;              break;
      case VK_F2:         button = Button::kF2;              break;
      case VK_F3:         button = Button::kF3;              break;
      case VK_F4:         button = Button::kF4;              break;
      case VK_F5:         button = Button::kF5;              break;
      case VK_F6:         button = Button::kF6;              break;
      case VK_F7:         button = Button::kF7;              break;
      case VK_F8:         button = Button::kF8;              break;
      case VK_F9:         button = Button::kF9;              break;
      case VK_F10:        button = Button::kF10;             break;
      case VK_F11:        button = Button::kF11;             break;
      case VK_F12:        button = Button::kF12;             break;
      case VK_F13:        button = Button::kF13;             break;
      case VK_F14:        button = Button::kF14;             break;
      case VK_F15:        button = Button::kF15;             break;
      case VK_F16:        button = Button::kF16;             break;
      case VK_F17:        button = Button::kF17;             break;
      case VK_F18:        button = Button::kF18;             break;
      case VK_F19:        button = Button::kF19;             break;
      case VK_F20:        button = Button::kF20;             break;
      case VK_F21:        button = Button::kF21;             break;
      case VK_F22:        button = Button::kF22;             break;
      case VK_F23:        button = Button::kF23;             break;
      case VK_F24:        button = Button::kF24;             break;
      case VK_NUMPAD0:    button = Button::kKeypad0;         break;
      case VK_NUMPAD1:    button = Button::kKeypad1;         break;
      case VK_NUMPAD2:    button = Button::kKeypad2;         break;
      case VK_NUMPAD3:    button = Button::kKeypad3;         break;
      case VK_NUMPAD4:    button = Button::kKeypad4;         break;
      case VK_NUMPAD5:    button = Button::kKeypad5;         break;
      case VK_NUMPAD6:    button = Button::kKeypad6;         break;
      case VK_NUMPAD7:    button = Button::kKeypad7;         break;
      case VK_NUMPAD8:    button = Button::kKeypad8;         break;
      case VK_NUMPAD9:    button = Button::kKeypad9;         break;
      case VK_DECIMAL:    button = Button::kKeypadDecimal;   break;
      case VK_DIVIDE:     button = Button::kKeypadDivide;    break;
      case VK_MULTIPLY:   button = Button::kKeypadMultiply;  break;
      case VK_SUBTRACT:   button = Button::kKeypadSubtract;  break;
      case VK_ADD:        button = Button::kKeypadAdd;       break;
      case VK_LSHIFT:     button = Button::kLeftShift;       break;
      case VK_LCONTROL:   button = Button::kLeftControl;     break;
      case VK_LMENU:      button = Button::kLeftAlt;         break;
      case VK_LWIN:       button = Button::kLeftSuper;       break;
      case VK_RSHIFT:     button = Button::kRightShift;      break;
      case VK_RCONTROL:   button = Button::kRightControl;    break;
      case VK_RMENU:      button = Button::kRightAlt;        break;
      case VK_RWIN:       button = Button::kRightSuper;      break;
      case VK_MENU:       button = Button::kMenu;            break;
      default:            button = Button::kButtonUnknown;
      }
      return button;
    }

    //--------------------------------------------------------------------------
    Button WinMouseCodeToButton(int mouse_code)
    {
      Button button;
      switch (mouse_code)
      {
      case MK_LBUTTON:  button = Button::kLeftMouse;   break;
      case MK_RBUTTON:  button = Button::kRightMouse;  break;
      case MK_MBUTTON:  button = Button::kMiddleMouse; break;
      case MK_XBUTTON1: button = Button::kMouse4;      break;
      case MK_XBUTTON2: button = Button::kMouse5;      break;
      default:          button = Button::kButtonUnknown;
      }
      return button;
    }
    
    //--------------------------------------------------------------------------
    Win32InputServer::Win32InputServer(PlatformInput& input_client, Win32Window& window):
      input_client_(input_client),
      window_(window)
    {
      // We assume we always have a keyboard and mouse on windows
      input_client_.SetMousePresent(true);
      input_client_.SetKeyboardPresent(true);
    }

    //--------------------------------------------------------------------------
    Win32InputServer::~Win32InputServer()
    {
    }

    bool Win32InputServer::OnWin32Event(void*, uint message,
      uint64_t word_param, int64_t long_param)
    {
      Button button = Button::kButtonUnknown;

      switch(message)
      {
      case WM_LBUTTONDOWN:
        input_client_.SetButtonDown(Button::kLeftMouse, true);
        return true;
      case WM_LBUTTONUP:
        input_client_.SetButtonDown(Button::kLeftMouse, false);
        return true;
      case WM_MBUTTONDOWN:
        input_client_.SetButtonDown(Button::kMiddleMouse, true);
        return true;
      case WM_MBUTTONUP:
        input_client_.SetButtonDown(Button::kMiddleMouse, false);
        return true;
      case WM_RBUTTONDOWN:
        input_client_.SetButtonDown(Button::kRightMouse, true);
        return true;
      case WM_RBUTTONUP:
        input_client_.SetButtonDown(Button::kRightMouse, false);
        return true;
      case WM_XBUTTONDOWN:
        button = WinMouseCodeToButton(GET_XBUTTON_WPARAM(word_param));
        input_client_.SetButtonDown(button, true);
        return Mouse::IsButton(button);
      case WM_XBUTTONUP:
        button = WinMouseCodeToButton(GET_XBUTTON_WPARAM(word_param));
        input_client_.SetButtonDown(button, false);
        return Mouse::IsButton(button);
      case WM_MOUSEWHEEL:
        input_client_.SetScrollOffset(static_cast<float>(GET_WHEEL_DELTA_WPARAM(word_param)));
        return true;
      case WM_MOUSEMOVE:
        {
          const glm::ivec2 window_size = window_.GetSize();
          const double normalized_x = -1.0 + 2.0 * GET_X_LPARAM(long_param) / window_size.x;
          const double normalized_y = -1.0 + 2.0 * GET_Y_LPARAM(long_param) / window_size.y;
          input_client_.SetMousePosition(glm::vec2(normalized_x, normalized_y));
          return true;
        }
      case WM_KEYDOWN:
        button = WinKeyCodeToButton(WinGetExtendedKeyCode(word_param, long_param));
        input_client_.SetButtonDown(button, true);
        return Keyboard::IsButton(button);
      case WM_KEYUP:
        button = WinKeyCodeToButton(WinGetExtendedKeyCode(word_param, long_param));
        input_client_.SetButtonDown(button, false);
        return Keyboard::IsButton(button);
      case WM_CHAR:
        if (word_param > 0 && word_param < 0x10000)
        {
          input_client_.AppendInputString(static_cast<char>(word_param));
          return true;
        } 
      default:
        return false;
      }
    }

    static glm::vec2 GetNormalizedThumbStick(glm::ivec2 raw_value, uint dead_zone)
    {
      //determine how far the controller is pushed
      float magnitude = static_cast<float>(glm::sqrt(
        raw_value.x * raw_value.x + raw_value.y * raw_value.y));

      //determine the direction the controller is pushed
      glm::vec2 norm = { raw_value.x / magnitude,  raw_value.y / magnitude };
      float normalizedMagnitude = 0;

      //check if the controller is outside a circular dead zone
      if (magnitude > dead_zone)
      {
        //clip the magnitude at its expected maximum value
        if (magnitude > 32767) magnitude = 32767;

        //adjust magnitude relative to the end of the dead zone
        magnitude -= dead_zone;

        //optionally normalize the magnitude with respect to its expected range
        //giving a magnitude value of 0.0 to 1.0
        normalizedMagnitude = magnitude / (32767 - dead_zone);
      }
      else //if the controller is in the deadzone zero out the magnitude
      {
        magnitude = 0.0;
        normalizedMagnitude = 0.0;
      }

      return norm * normalizedMagnitude;
    }

    //--------------------------------------------------------------------------
    void Win32InputServer::ProcessEvents()
    {
      input_client_.StartFrame();

      for (int i = 0; i < XUSER_MAX_COUNT; ++i)
      {    
        XINPUT_STATE state = {};
        if (XInputGetState(i, &state) == ERROR_SUCCESS)
        {
          input_client_.SetGamepadPresent(true, i);

          // Tumb sticks
          const glm::vec2 left_thumb = GetNormalizedThumbStick(
            { state.Gamepad.sThumbLX, state.Gamepad.sThumbLY }, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
          const glm::vec2 right_thumb = GetNormalizedThumbStick(
            { state.Gamepad.sThumbRX, state.Gamepad.sThumbRY }, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

          input_client_.SetAxis(Axis::kLeftX, left_thumb.x, i);
          input_client_.SetAxis(Axis::kLeftY, left_thumb.y, i);
          input_client_.SetAxis(Axis::kRightX, right_thumb.x, i);
          input_client_.SetAxis(Axis::kRightY, right_thumb.y, i);

          // Triggers
          const float left_trig = state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD ?
            static_cast<float>(state.Gamepad.bLeftTrigger) / 255.0f : 0.0f;
          const float right_trig = state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD ?
            static_cast<float>(state.Gamepad.bRightTrigger) / 255.0f : 0.0f;

          input_client_.SetAxis(Axis::kLeftTrigger, left_trig, i);
          input_client_.SetAxis(Axis::kRightTrigger, right_trig, i);

          // Buttons
          auto set_button = [&](Button button, WORD flag) {
            input_client_.SetButtonDown(button, (state.Gamepad.wButtons & flag) != 0, i);
          };

          set_button(Button::kGamepadActionDown,  XINPUT_GAMEPAD_A);
          set_button(Button::kGamepadActionRight, XINPUT_GAMEPAD_B);
          set_button(Button::kGamepadActionLeft,  XINPUT_GAMEPAD_X);
          set_button(Button::kGamepadActionUp,    XINPUT_GAMEPAD_Y);
          set_button(Button::kGamepadDpadLeft,    XINPUT_GAMEPAD_DPAD_LEFT);
          set_button(Button::kGamepadDpadRight,   XINPUT_GAMEPAD_DPAD_RIGHT);
          set_button(Button::kGamepadDpadUp,      XINPUT_GAMEPAD_DPAD_UP);
          set_button(Button::kGamepadDpadDown,    XINPUT_GAMEPAD_DPAD_DOWN);
          set_button(Button::kGamepadLeftBumper,  XINPUT_GAMEPAD_LEFT_SHOULDER);
          set_button(Button::kGamepadRightBumper, XINPUT_GAMEPAD_RIGHT_SHOULDER);
          set_button(Button::kGamepadLeftStick,   XINPUT_GAMEPAD_LEFT_THUMB);
          set_button(Button::kGamepadRightStick,  XINPUT_GAMEPAD_RIGHT_THUMB);
          set_button(Button::kGamepadSelect,      XINPUT_GAMEPAD_BACK);
          set_button(Button::kGamepadStart,       XINPUT_GAMEPAD_START);
        }
        else
        {
          input_client_.SetGamepadPresent(false, i);
        } 
      }
    }
  }
}
