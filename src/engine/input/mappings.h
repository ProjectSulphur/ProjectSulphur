#pragma once

#include <foundation/utils/type_definitions.h>

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    enum struct Button : uint
    {
      kUnknown = 0,
      // Keyboard
      kSpace,
      kApostrophe,
      kComma,
      kMinus,
      kPeriod,
      kSlash,
      k0,
      k1,
      k2,
      k3,
      k4,
      k5,
      k6,
      k7,
      k8,
      k9,
      kSemicolon,
      kEqual,
      kA,
      kB,
      kC,
      kD,
      kE,
      kF,
      kG,
      kH,
      kI,
      kJ,
      kK,
      kL,
      kM,
      kN,
      kO,
      kP,
      kQ,
      kR,
      kS,
      kT,
      kU,
      kV,
      kW,
      kX,
      kY,
      kZ,
      kLeftBracket,
      kBackslash,
      kRightBracket,
      kGraveAccent,
      kWorld1,
      kWorld2,
      kEscape,
      kEnter,
      kTab,
      kBackspace,
      kInsert,
      kDelete,
      kRight,
      kLeft,
      kDown,
      kUp,
      kPageUp,
      kPageDown,
      kHome,
      kEnd,
      kCapsLock,
      kScrollLock,
      kNumLock,
      kPrintScreen,
      kPause,
      kF1,
      kF2,
      kF3,
      kF4,
      kF5,
      kF6,
      kF7,
      kF8,
      kF9,
      kF10,
      kF11,
      kF12,
      kF13,
      kF14,
      kF15,
      kF16,
      kF17,
      kF18,
      kF19,
      kF20,
      kF21,
      kF22,
      kF23,
      kF24,
      kF25,
      kKeypad0,
      kKeypad1,
      kKeypad2,
      kKeypad3,
      kKeypad4,
      kKeypad5,
      kKeypad6,
      kKeypad7,
      kKeypad8,
      kKeypad9,
      kKeypadDecimal,
      kKeypadDivide,
      kKeypadMultiply,
      kKeypadSubtract,
      kKeypadAdd,
      kKeypadEnter,
      kKeypadEqual,
      kLeftShift,
      kLeftControl,
      kLeftAlt,
      kLeftSuper,
      kRightShift,
      kRightControl,
      kRightAlt,
      kRightSuper,
      kMenu,
      // Gamepad
      kGamepadDpadUp,
      kGamepadDpadLeft,
      kGamepadDpadDown,
      kGamepadDpadRight,
      kGamepadActionUp,
      kGamepadActionLeft,
      kGamepadActionDown,
      kGamepadActionRight,
      kGamepadLeftStick,
      kGamepadRightStick,
      kGamepadLeftBumper,
      kGamepadRightBumper,
      kGamepadStart,
      kGamepadSelect,
      kGamepadHome,
      // Mouse buttons
      kLeftMouse,
      kRightMouse,
      kMiddleMouse,
      kMouse4,
      kMouse5
    };
    constexpr uint kFirstKeyboardButton = static_cast<uint>(Button::kSpace);
    constexpr uint kLastKeyboardButton = static_cast<uint>(Button::kMenu);
    constexpr uint kNumKeyboardButtons = kLastKeyboardButton - kFirstKeyboardButton + 1;
    constexpr uint kFirstGamepadButton= static_cast<uint>(Button::kGamepadDpadUp);
    constexpr uint kLastGamepadButton = static_cast<uint>(Button::kGamepadHome);
    constexpr uint kNumGamepadButtons = kLastGamepadButton - kFirstGamepadButton + 1;
    constexpr uint kFirstMouseButton = static_cast<uint>(Button::kLeftMouse);
    constexpr uint kLastMouseButton = static_cast<uint>(Button::kMouse5);
    constexpr uint kNumMouseButtons = kLastMouseButton - kFirstMouseButton + 1;

    //-------------------------------------------------------------------------
    enum struct ButtonState
    {
      kUp = 0,
      kFalling,
      kDown,
      kRising
    };
    inline ButtonState GetButtonState(bool down, bool prev_down)
    {
      if (down == true)
      {
        if (prev_down == false)
        {
          return ButtonState::kFalling;
        }
        else
        {
          return ButtonState::kDown;
        }
      }
      else if (prev_down == true)
      {
        return ButtonState::kRising;
      }

      return ButtonState::kUp;
    }

    //-------------------------------------------------------------------------
    enum struct Axis : uint
    {
      kUnknown = 0,
      // Gamepad
      kLeftX,
      kLeftY,
      kRightX,
      kRightY,
      kLeftTrigger,
      kRightTrigger
    };
    constexpr uint kFirstGamepadAxis = static_cast<uint>(Axis::kLeftX);
    constexpr uint kLastGamepadAxis = static_cast<uint>(Axis::kRightTrigger);
    constexpr uint kNumGamepadAxes = kLastGamepadAxis - kFirstGamepadAxis + 1;

  }
}