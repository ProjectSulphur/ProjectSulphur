#include "engine/win32/win32_input_server.h"
#include "engine/application/window.h"
#include "engine/input/input.h"

#define GLFW_EXPOSE_NATIVE_WIN32 1
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    Button GLFWKeyToSulphurButton(int glfw_key)
    {
      Button button = Button::kUnknown;
      switch (glfw_key)
      {
      case GLFW_KEY_SPACE:            button = Button::kSpace;           break;
      case GLFW_KEY_APOSTROPHE:       button = Button::kApostrophe;      break;
      case GLFW_KEY_COMMA:            button = Button::kComma;           break;
      case GLFW_KEY_MINUS:            button = Button::kMinus;           break;
      case GLFW_KEY_PERIOD:           button = Button::kPeriod;          break;
      case GLFW_KEY_SLASH:            button = Button::kSlash;           break;
      case GLFW_KEY_0:                button = Button::k0;               break;
      case GLFW_KEY_1:                button = Button::k1;               break;
      case GLFW_KEY_2:                button = Button::k2;               break;
      case GLFW_KEY_3:                button = Button::k3;               break;
      case GLFW_KEY_4:                button = Button::k4;               break;
      case GLFW_KEY_5:                button = Button::k5;               break;
      case GLFW_KEY_6:                button = Button::k6;               break;
      case GLFW_KEY_7:                button = Button::k7;               break;
      case GLFW_KEY_8:                button = Button::k8;               break;
      case GLFW_KEY_9:                button = Button::k9;               break;
      case GLFW_KEY_SEMICOLON:        button = Button::kSemicolon;       break;
      case GLFW_KEY_EQUAL:            button = Button::kEqual;           break;
      case GLFW_KEY_A:                button = Button::kA;               break;
      case GLFW_KEY_B:                button = Button::kB;               break;
      case GLFW_KEY_C:                button = Button::kC;               break;
      case GLFW_KEY_D:                button = Button::kD;               break;
      case GLFW_KEY_E:                button = Button::kE;               break;
      case GLFW_KEY_F:                button = Button::kF;               break;
      case GLFW_KEY_G:                button = Button::kG;               break;
      case GLFW_KEY_H:                button = Button::kH;               break;
      case GLFW_KEY_I:                button = Button::kI;               break;
      case GLFW_KEY_J:                button = Button::kJ;               break;
      case GLFW_KEY_K:                button = Button::kK;               break;
      case GLFW_KEY_L:                button = Button::kL;               break;
      case GLFW_KEY_M:                button = Button::kM;               break;
      case GLFW_KEY_N:                button = Button::kN;               break;
      case GLFW_KEY_O:                button = Button::kO;               break;
      case GLFW_KEY_P:                button = Button::kP;               break;
      case GLFW_KEY_Q:                button = Button::kQ;               break;
      case GLFW_KEY_R:                button = Button::kR;               break;
      case GLFW_KEY_S:                button = Button::kS;               break;
      case GLFW_KEY_T:                button = Button::kT;               break;
      case GLFW_KEY_U:                button = Button::kU;               break;
      case GLFW_KEY_V:                button = Button::kV;               break;
      case GLFW_KEY_W:                button = Button::kW;               break;
      case GLFW_KEY_X:                button = Button::kX;               break;
      case GLFW_KEY_Y:                button = Button::kY;               break;
      case GLFW_KEY_Z:                button = Button::kZ;               break;
      case GLFW_KEY_LEFT_BRACKET:     button = Button::kLeftBracket;     break;
      case GLFW_KEY_BACKSLASH:        button = Button::kBackslash;       break;
      case GLFW_KEY_RIGHT_BRACKET:    button = Button::kRightBracket;    break;
      case GLFW_KEY_GRAVE_ACCENT:     button = Button::kGraveAccent;     break;
      case GLFW_KEY_WORLD_1:          button = Button::kWorld1;          break;
      case GLFW_KEY_WORLD_2:          button = Button::kWorld2;          break;
      case GLFW_KEY_ESCAPE:           button = Button::kEscape;          break;
      case GLFW_KEY_ENTER:            button = Button::kEnter;           break;
      case GLFW_KEY_TAB:              button = Button::kTab;             break;
      case GLFW_KEY_BACKSPACE:        button = Button::kBackspace;       break;
      case GLFW_KEY_INSERT:           button = Button::kInsert;          break;
      case GLFW_KEY_DELETE:           button = Button::kDelete;          break;
      case GLFW_KEY_RIGHT:            button = Button::kRight;           break;
      case GLFW_KEY_LEFT:             button = Button::kLeft;            break;
      case GLFW_KEY_DOWN:             button = Button::kDown;            break;
      case GLFW_KEY_UP:               button = Button::kUp;              break;
      case GLFW_KEY_PAGE_UP:          button = Button::kPageUp;          break;
      case GLFW_KEY_PAGE_DOWN:        button = Button::kPageDown;        break;
      case GLFW_KEY_HOME:             button = Button::kHome;            break;
      case GLFW_KEY_END:              button = Button::kEnd;             break;
      case GLFW_KEY_CAPS_LOCK:        button = Button::kCapsLock;        break;
      case GLFW_KEY_SCROLL_LOCK:      button = Button::kScrollLock;      break;
      case GLFW_KEY_NUM_LOCK:         button = Button::kNumLock;         break;
      case GLFW_KEY_PRINT_SCREEN:     button = Button::kPrintScreen;     break;
      case GLFW_KEY_PAUSE:            button = Button::kPause;           break;
      case GLFW_KEY_F1:               button = Button::kF1;              break;
      case GLFW_KEY_F2:               button = Button::kF2;              break;
      case GLFW_KEY_F3:               button = Button::kF3;              break;
      case GLFW_KEY_F4:               button = Button::kF4;              break;
      case GLFW_KEY_F5:               button = Button::kF5;              break;
      case GLFW_KEY_F6:               button = Button::kF6;              break;
      case GLFW_KEY_F7:               button = Button::kF7;              break;
      case GLFW_KEY_F8:               button = Button::kF8;              break;
      case GLFW_KEY_F9:               button = Button::kF9;              break;
      case GLFW_KEY_F10:              button = Button::kF10;             break;
      case GLFW_KEY_F11:              button = Button::kF11;             break;
      case GLFW_KEY_F12:              button = Button::kF12;             break;
      case GLFW_KEY_F13:              button = Button::kF13;             break;
      case GLFW_KEY_F14:              button = Button::kF14;             break;
      case GLFW_KEY_F15:              button = Button::kF15;             break;
      case GLFW_KEY_F16:              button = Button::kF16;             break;
      case GLFW_KEY_F17:              button = Button::kF17;             break;
      case GLFW_KEY_F18:              button = Button::kF18;             break;
      case GLFW_KEY_F19:              button = Button::kF19;             break;
      case GLFW_KEY_F20:              button = Button::kF20;             break;
      case GLFW_KEY_F21:              button = Button::kF21;             break;
      case GLFW_KEY_F22:              button = Button::kF22;             break;
      case GLFW_KEY_F23:              button = Button::kF23;             break;
      case GLFW_KEY_F24:              button = Button::kF24;             break;
      case GLFW_KEY_F25:              button = Button::kF25;             break;
      case GLFW_KEY_KP_0:             button = Button::kKeypad0;         break;
      case GLFW_KEY_KP_1:             button = Button::kKeypad1;         break;
      case GLFW_KEY_KP_2:             button = Button::kKeypad2;         break;
      case GLFW_KEY_KP_3:             button = Button::kKeypad3;         break;
      case GLFW_KEY_KP_4:             button = Button::kKeypad4;         break;
      case GLFW_KEY_KP_5:             button = Button::kKeypad5;         break;
      case GLFW_KEY_KP_6:             button = Button::kKeypad6;         break;
      case GLFW_KEY_KP_7:             button = Button::kKeypad7;         break;
      case GLFW_KEY_KP_8:             button = Button::kKeypad8;         break;
      case GLFW_KEY_KP_9:             button = Button::kKeypad9;         break;
      case GLFW_KEY_KP_DECIMAL:       button = Button::kKeypadDecimal;   break;
      case GLFW_KEY_KP_DIVIDE:        button = Button::kKeypadDivide;    break;
      case GLFW_KEY_KP_MULTIPLY:      button = Button::kKeypadMultiply;  break;
      case GLFW_KEY_KP_SUBTRACT:      button = Button::kKeypadSubtract;  break;
      case GLFW_KEY_KP_ADD:           button = Button::kKeypadAdd;       break;
      case GLFW_KEY_KP_ENTER:         button = Button::kKeypadEnter;     break;
      case GLFW_KEY_KP_EQUAL:         button = Button::kKeypadEqual;     break;
      case GLFW_KEY_LEFT_SHIFT:       button = Button::kLeftShift;       break;
      case GLFW_KEY_LEFT_CONTROL:     button = Button::kLeftControl;     break;
      case GLFW_KEY_LEFT_ALT:         button = Button::kLeftAlt;         break;
      case GLFW_KEY_LEFT_SUPER:       button = Button::kLeftSuper;       break;
      case GLFW_KEY_RIGHT_SHIFT:      button = Button::kRightShift;      break;
      case GLFW_KEY_RIGHT_CONTROL:    button = Button::kRightControl;    break;
      case GLFW_KEY_RIGHT_ALT:        button = Button::kRightAlt;        break;
      case GLFW_KEY_RIGHT_SUPER:      button = Button::kRightSuper;      break;
      case GLFW_KEY_MENU:             button = Button::kMenu;            break;
      }
      return button;
    }
    Button GLFWMouseButtonToButton(int glfw_button)
    {
      Button button = Button::kUnknown;
      switch (glfw_button)
      {
      case GLFW_MOUSE_BUTTON_LEFT:    button = Button::kLeftMouse;       break;
      case GLFW_MOUSE_BUTTON_RIGHT:   button = Button::kRightMouse;      break;
      case GLFW_MOUSE_BUTTON_MIDDLE:  button = Button::kMiddleMouse;     break;
      case GLFW_MOUSE_BUTTON_4:       button = Button::kMouse4;          break;
      case GLFW_MOUSE_BUTTON_5:       button = Button::kMouse5;          break;
      }
      return button;
    }
    Button GLFWJoystickButtonToButton(int glfw_button)
    {
      Button button = Button::kUnknown;
      switch (glfw_button)
      {
      case 0:  button = Button::kGamepadDpadUp;      break;
      case 1:  button = Button::kGamepadDpadLeft;    break;
      case 2:  button = Button::kGamepadDpadDown;    break;
      case 3:  button = Button::kGamepadDpadRight;   break;
      case 4:  button = Button::kGamepadActionUp;    break;
      case 5:  button = Button::kGamepadActionLeft;  break;
      case 6:  button = Button::kGamepadActionDown;  break;
      case 7:  button = Button::kGamepadActionRight; break;
      case 8:  button = Button::kGamepadLeftStick;   break;
      case 9:  button = Button::kGamepadRightStick;  break;
      case 10: button = Button::kGamepadLeftBumper;  break;
      case 11: button = Button::kGamepadRightBumper; break;
      case 12: button = Button::kGamepadStart;       break;
      case 13: button = Button::kGamepadSelect;      break;
      case 14: button = Button::kGamepadHome;        break;
      }
      return button;
    }
    Axis GLFWJoystickAxisToAxis(int glfw_button)
    {
      Axis axis = Axis::kUnknown;
      switch (glfw_button)
      {
      case 0: axis = Axis::kLeftX;        break;
      case 1: axis = Axis::kLeftY;        break;
      case 2: axis = Axis::kRightX;       break;
      case 3: axis = Axis::kRightY;       break;
      case 4: axis = Axis::kLeftTrigger;  break;
      case 5: axis = Axis::kRightTrigger; break;
      }
      return axis;
    }

    //-------------------------------------------------------------------------
    void Win32InputServer::KeyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/)
    {
      const Button button = GLFWKeyToSulphurButton(key);
      if (button != Button::kUnknown)
      {
        input_client_->SetButtonDown(button, action != GLFW_RELEASE);
      }
    }
    void Win32InputServer::CharCallback(GLFWwindow* /*window*/, unsigned int code_point)
    {
      input_client_->AppendInputString(static_cast<char>(code_point));
    }

    //-------------------------------------------------------------------------
    void Win32InputServer::CursorPosCallback(GLFWwindow* window, double x_pos, double y_pos)
    {
      int window_width, window_height;
      glfwGetWindowSize(window, &window_width, &window_height);

      if (window_width == 0 || window_height == 0)
      {
        return;
      }

      const double normalized_x = -1.0 + 2.0 * x_pos / window_width;
      const double normalized_y = -1.0 + 2.0 * y_pos / window_height;
      input_client_->SetMousePosition(glm::vec2(normalized_x, normalized_y));
    }
    void Win32InputServer::CursorEnterCallback(GLFWwindow* /*window*/, int entered)
    {
      input_client_->SetMousePresent(entered == GLFW_TRUE);
    }
    void Win32InputServer::MouseButtonCallback(GLFWwindow* /*window*/, int mouse_button, int action, int /*mods*/)
    {
      const Button button = GLFWMouseButtonToButton(mouse_button);
      if (button != Button::kUnknown)
      {
        input_client_->SetButtonDown(button, action != GLFW_RELEASE);
      }
    }
    void Win32InputServer::ScrollCallback(GLFWwindow* /*window*/, double /*x_offset*/, double y_offset)
    {
      input_client_->SetScrollOffset(static_cast<float>(y_offset));
    }

    //-------------------------------------------------------------------------
    void Win32InputServer::JoystickCallback(int joy, int event)
    {
      if (joy >= 0 && joy < Input::kMaxGamepads)
      {
        input_client_->SetGamepadPresent(event == GLFW_CONNECTED, joy);
      }
    }

    //-------------------------------------------------------------------------
    PlatformInput* Win32InputServer::input_client_ = nullptr;
    
    //-------------------------------------------------------------------------
    Win32InputServer::Win32InputServer(const Win32GLFWWindowWrapper& window, PlatformInput& input_client) :
      window_(window)
    {
      input_client_ = &input_client;

      // Setup callbacks
      glfwSetKeyCallback(window_.glfw_window(), KeyCallback);
      glfwSetCharCallback(window_.glfw_window(), CharCallback);

      glfwSetCursorEnterCallback(window_.glfw_window(), CursorEnterCallback);
      glfwSetCursorPosCallback(window_.glfw_window(), CursorPosCallback);
      glfwSetMouseButtonCallback(window_.glfw_window(), MouseButtonCallback);
      glfwSetScrollCallback(window_.glfw_window(), ScrollCallback);

      glfwSetJoystickCallback(JoystickCallback);

      // Assuming a keyboard is always present.
      input_client_->SetKeyboardPresent(true);
    }
    Win32InputServer::~Win32InputServer()
    {
      input_client_ = nullptr;

      // Unset callbacks
      glfwSetKeyCallback(window_.glfw_window(), nullptr);
      glfwSetCharCallback(window_.glfw_window(), nullptr);

      glfwSetCursorEnterCallback(window_.glfw_window(), nullptr);
      glfwSetCursorPosCallback(window_.glfw_window(), nullptr);
      glfwSetMouseButtonCallback(window_.glfw_window(), nullptr);
      glfwSetScrollCallback(window_.glfw_window(), nullptr);

      glfwSetJoystickCallback(nullptr);
    }

    //-------------------------------------------------------------------------
    void Win32InputServer::ProcessEvents()
    {
      assert(input_client_ != nullptr);

      // Poll joystick input
      int count;
      for (int i_joy = 0; i_joy < Input::kMaxGamepads; ++i_joy)
      {
        const float* axes = glfwGetJoystickAxes(i_joy, &count);
        for (int i_axis = 0; i_axis < count; ++i_axis)
        {
          const Axis axis = GLFWJoystickAxisToAxis(i_axis);
          if (axis != Axis::kUnknown)
          {
            input_client_->SetAxis(axis, axes[i_axis]);
          }
        }
        const unsigned char* buttons = glfwGetJoystickButtons(i_joy, &count);
        for (int i_button = 0; i_button < count; ++i_button)
        {
          const Button button = GLFWJoystickButtonToButton(i_button);
          if (button != Button::kUnknown)
          {
            input_client_->SetButtonDown(button, buttons[i_button] == GLFW_PRESS);
          }
        }
      }

      input_client_->StartFrame();
    }
  }
}