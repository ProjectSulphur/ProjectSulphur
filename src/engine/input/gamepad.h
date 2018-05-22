#pragma once
#include "engine/input/mappings.h"

#include <foundation/logging/logger.h>

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    class Gamepad
    {
    public:
      /**
      * @brief Sets the input received during the last frame as the current read-input and sets up a new frame
      */
      void StartFrame();

      /**
      * @brief A static function to test if a button is a valid gamepad button
      * @return (bool) True is the button is in fact a valid gamepad button
      */
      static bool IsButton(Button button);
      /**
      * @brief A static function to test if a button is a valid gamepad axis
      * @return (bool) True is the axis is in fact a valid gamepad axis
      */
      static bool IsAxis(Axis axis);

      /**
      * @brief Set if the gamepad will be present from the next frame onward
      * @param[in] present (bool) True if the gamepad will be present next frame
      * @see sulphur::engine::Gamepad::StartFrame
      */
      void SetPresent(bool present);
      /**
      * @brief Test if the gamepad is present
      * @return (bool) True if the gamepad is present
      */
      bool present() const;

      /**
      * @brief Set the next state of a button
      * @param[in] button (sulphur::engine::Button) An enumerator describing which hardware button to use
      * @param[in] down (bool) The state of the button, that will be used next frame
      * @remarks If the button is not a gamepad button this will fail/assert
      * @see sulphur::engine::Gamepad::IsButton
      * @see sulphur::engine::Gamepad::StartFrame
      */
      void SetButton(Button button, bool down);
      /**
      * @brief Set the next state of an axis
      * @param[in] axis (sulphur::engine::Axis) An enumerator describing which hardware axis to use
      * @param[in] value (float) The value of the axis, that will be used next frame
      * @remarks If the axis is not a gamepad axis this will fail/assert
      * @see sulphur::engine::Gamepad::IsAxis
      * @see sulphur::engine::Gamepad::StartFrame
      */
      void SetAxis(Axis axis, float value);

      /**
      * @brief Get the current state of the specified button
      * @param[in] button (sulphur::engine::Button) An enumerator describing which hardware button to use
      * @remarks If the button is not a gamepad button this will fail/assert
      * @see sulphur::engine::Gamepad::IsButton
      * @return (sulphur::engine::ButtonState) The current state of the specified button
      */
      ButtonState GetButton(Button button) const;
      /**
      * @brief Get the value of an axis
      * @param[in] axis (sulphur::engine::Axis) An enumerator describing which hardware axis to use
      * @return (float) The resulting normalized axis value, ranged [-1 to 1]
      */
      float GetAxis(Axis axis) const;

    private:
      bool next_present_; //<! If the gamepad will be present next frame
      bool present_; //<! If the gamepad is present this frame

      bool next_buttons_[kNumGamepadButtons];
      bool buttons_[kNumGamepadButtons];
      bool prev_buttons_[kNumGamepadButtons];

      float next_axes_[kNumGamepadAxes];
      float axes_[kNumGamepadAxes];

    };

    //-------------------------------------------------------------------------
    inline bool Gamepad::IsButton(Button button)
    {
      const uint idx = static_cast<uint>(button) - kFirstGamepadButton;
      if (idx >= kNumGamepadButtons)
      {
        return false;
      }
      return true;
    }
    inline bool Gamepad::IsAxis(Axis axis)
    {
      const uint idx = static_cast<uint>(axis) - kFirstGamepadAxis;
      if (idx >= kNumGamepadAxes)
      {
        return false;
      }
      return true;
    }

    //-------------------------------------------------------------------------
    inline void Gamepad::SetPresent(bool present)
    {
      next_present_ = present;
    }
    inline bool Gamepad::present() const
    {
      return present_;
    }

    //-------------------------------------------------------------------------
    inline void Gamepad::SetButton(Button button, bool down)
    {
      const uint idx = static_cast<uint>(button) - kFirstGamepadButton;
      next_buttons_[idx] = down;
    }
    inline void Gamepad::SetAxis(Axis axis, float value)
    {
      const uint idx = static_cast<uint>(axis) - kFirstGamepadAxis;
      next_axes_[idx] = value;
    }

    //-------------------------------------------------------------------------
    inline ButtonState Gamepad::GetButton(Button button) const
    {
      PS_LOG_IF(IsButton(button) == false, Assert, "Invalid buttoncode passed to gamepad");
      const uint idx = static_cast<uint>(button) - kFirstGamepadButton;
      return GetButtonState(buttons_[idx], prev_buttons_[idx]);
    }
    inline float Gamepad::GetAxis(Axis axis) const
    {
      PS_LOG_IF(IsAxis(axis) == false, Assert, "Invalid axiscode passed to gamepad");
      return axes_[static_cast<uint>(axis) - kFirstGamepadAxis];
    }
  }
}