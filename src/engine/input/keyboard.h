#pragma once
#include "mappings.h"

#include <foundation/logging/logger.h>

namespace sulphur
{
  namespace engine
  {
    /**
    * @class sulphur::engine::Keyboard
    * @brief An abstraction class storing the state of the keyboard input
    * @author Maarten ten Velden
    */
    class Keyboard
    {
    public:
      /**
      * @brief Sets the input received during the last frame as the current read-input and sets up a new frame
      */
      void StartFrame();

      /**
      * @brief A static function to test if a button is a valid keyboard button
      * @return (bool) True is the button is in fact a valid keyboard button
      */
      static bool IsButton(Button button);

      /**
      * @brief Set if the keyboard will be present from the next frame onward
      * @param[in] present (bool) True if the keyboard will be present next frame
      * @see sulphur::engine::Keyboard::StartFrame
      */
      void SetPresent(bool present);
      /**
      * @brief Test if the keyboard is present
      * @return (bool) True if the keyboard is present
      */
      bool present() const;

      /**
      * @brief Set the next state of a button
      * @param[in] button (sulphur::engine::Button) An enumerator describing which hardware button to use
      * @param[in] down (bool) The state of the button, that will be used next frame
      * @remarks If the button is not a keyboard button this will fail/assert
      * @see sulphur::engine::Keyboard::IsButton
      * @see sulphur::engine::Keyboard::StartFrame
      */
      void SetButton(Button button, bool down);

      /**
      * @brief Get the current state of the specified button
      * @param[in] button (sulphur::engine::Button) An enumerator describing which hardware button to use
      * @remarks If the button is not a keyboard button this will fail/assert
      * @see sulphur::engine::Keyboard::IsButton
      * @return (sulphur::engine::ButtonState) The current state of the specified button
      */
      ButtonState GetButton(Button button) const;

    private:
      bool next_present_; //<! If the keyboard will be present next frame
      bool present_; //<! If the keyboard is present this frame

      bool next_buttons_[kNumKeyboardButtons];
      bool buttons_[kNumKeyboardButtons];
      bool prev_buttons_[kNumKeyboardButtons];

    };

    //-------------------------------------------------------------------------
    inline bool Keyboard::IsButton(Button button)
    {
      const uint idx = static_cast<uint>(button) - kFirstKeyboardButton;
      if (idx >= kNumKeyboardButtons)
      {
        return false;
      }
      return true;
    }

    //-------------------------------------------------------------------------
    inline void Keyboard::SetPresent(bool present)
    {
      next_present_ = present;
    }
    inline bool Keyboard::present() const
    {
      return present_;
    }

    //-------------------------------------------------------------------------
    inline void Keyboard::SetButton(Button button, bool down)
    {
      PS_LOG_IF(IsButton(button) == false, Assert, "Invalid buttoncode passed to keyboard");
      const uint idx = static_cast<uint>(button) - kFirstKeyboardButton;
      next_buttons_[idx] = down;
    }

    //-------------------------------------------------------------------------
    inline ButtonState Keyboard::GetButton(Button button) const
    {
      PS_LOG_IF(IsButton(button) == false, Assert, "Invalid buttoncode passed to keyboard");
      const uint idx = static_cast<uint>(button) - kFirstKeyboardButton;
      return GetButtonState(buttons_[idx], prev_buttons_[idx]);
    }

  }
}