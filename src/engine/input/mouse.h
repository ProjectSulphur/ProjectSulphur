#pragma once
#include "mappings.h"

#include <foundation/logging/logger.h>

#include <glm/glm.hpp>

namespace sulphur 
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::Mouse
    * @brief An abstraction class storing the state of the mouse input
    * @author Maarten ten Velden
    */
    class Mouse
    {
    public:
      /**
      * @brief Sets the input received during the last frame as the current read-input and sets up a new frame
      */
      void StartFrame();

      /**
      * @brief A static function to test if a button is a valid mouse-button
      * @return (bool) True is the button is in fact a valid mouse-button
      */
      static bool IsButton(Button button);

      /**
      * @brief Set if the mouse will be present from the next frame onward
      * @param[in] present (bool) True if the mouse will be present next frame
      * @see sulphur::engine::Mouse::StartFrame
      */
      void SetPresent(bool present);
      /**
      * @brief Test if the mouse is present
      * @return (bool) True if the mouse is present
      */
      bool present() const;

      /**
      * @brief Set the next position of the cursor
      * @param[in] position (glm::vec2) The position, that will be used next frame in normalized screen coordinates, ranged [-1 to 1]
      * @remarks The top left of the screen is [-1,-1] while the bottom-right of the screen is [1,1]
      * @see sulphur::engine::Mouse::StartFrame
      */
      void SetPosition(glm::vec2 position);
      /**
      * @brief Set the next offset of the mouse's scroll wheel
      * @return (float) The offset value that will be used next frame
      * @remarks A negative scroll offset indicates the user scrolling down on the y-axis, while a positive one indicates the user scrolling up on the y-axis.
      */
      void SetScrollOffset(float offset);
      /**
      * @brief Set the next state of a button
      * @param[in] button (sulphur::engine::Button) An enumerator describing which hardware button to use
      * @param[in] down (bool) The state of the button, that will be used next frame
      * @remarks If the button is not a mouse button this will fail/assert
      * @see sulphur::engine::Mouse::IsButton
      * @see sulphur::engine::Mouse::StartFrame
      */
      void SetButton(Button button, bool down);

      /**
      * @brief Get the current position of the cursor on the screen
      * @return (glm::vec2) The last known position of the cursor in normalized screen coordinates, ranged [-1 to 1]
      * @remarks The top left of the screen is [-1,-1] while the bottom-right of the screen is [1,1]
      */
      glm::vec2 position() const;
      /**
      * @brief Get the offset between the current cursor position and the previous one
      * @return (glm::vec2) The offset of the cursor in normalized screen coordinates, ranged [-1 to 1]
      * @remarks The top left of the screen is [-1,-1] while the bottom-right of the screen is [1,1]
      */
      glm::vec2 GetOffset() const;
      /**
      * @brief Get the offset of the mouse's scroll wheel
      * @return (float) The offset value
      * @remarks A negative scroll offset indicates the user scrolling down on the y-axis, while a positive one indicates the user scrolling up on the y-axis.
      */
      float scroll_offset() const;
      /**
      * @brief Get the current state of the specified button
      * @param[in] button (sulphur::engine::Button) An enumerator describing which hardware button to use
      * @remarks If the button is not a mouse button this will fail/assert
      * @see sulphur::engine::Mouse::IsButton
      * @return (sulphur::engine::ButtonState) The current state of the specified button
      */
      ButtonState GetButton(Button button) const;


    private:
      bool next_present_; //<! If the mouse will be present next frame
      bool present_; //<! If the mouse is present this frame

      glm::vec2 next_position_;
      glm::vec2 position_;
      glm::vec2 prev_position_;

      float next_scroll_offset_;
      float scroll_offset_;

      bool next_buttons_[kNumMouseButtons];
      bool buttons_[kNumMouseButtons];
      bool prev_buttons_[kNumMouseButtons];

    };

    //-------------------------------------------------------------------------
    inline bool Mouse::IsButton(Button button)
    {
      const uint idx = static_cast<uint>(button) - kFirstMouseButton;
      if (idx >= kNumMouseButtons)
      {
        return false;
      }
      return true;
    }
    
    //-------------------------------------------------------------------------
    inline void Mouse::SetPresent(bool present)
    {
      next_present_ = present;
    }
    inline bool Mouse::present() const
    {
      return present_;
    }

    //-------------------------------------------------------------------------
    inline void Mouse::SetPosition(glm::vec2 position)
    {
      next_position_ = position;
    }
    inline void Mouse::SetScrollOffset(float offset)
    {
      next_scroll_offset_= offset;
    }
    inline void Mouse::SetButton(Button button, bool down)
    {
      PS_LOG_IF(IsButton(button) == false, Assert, "Invalid buttoncode passed to mouse");\
      const uint idx = static_cast<uint>(button) - kFirstMouseButton;
      next_buttons_[idx] = down;
    }

    //-------------------------------------------------------------------------
    inline glm::vec2 Mouse::position() const
    {
      return position_;
    }
    inline glm::vec2 Mouse::GetOffset() const
    {
      return position_ - prev_position_;
    }
    inline float Mouse::scroll_offset() const
    {
      return scroll_offset_;
    }
    inline ButtonState Mouse::GetButton(Button button) const
    {
      PS_LOG_IF(IsButton(button) == false, Assert, "Invalid buttoncode passed to mouse");
      const uint idx = static_cast<uint>(button) - kFirstMouseButton;
      return GetButtonState(buttons_[idx], prev_buttons_[idx]);
    }

  }
}