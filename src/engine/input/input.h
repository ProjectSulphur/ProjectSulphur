#pragma once
#include "keyboard.h"
#include "mouse.h"
#include "gamepad.h"

#include <foundation/containers/string.h>
#include <foundation/containers/vector.h>
#include <foundation/containers/hash_map.h>
#include <foundation/job/resource.h>

#include "engine/scripting/scriptable_object.h"

#include <glm/glm.hpp>

namespace sulphur 
{
  namespace engine 
  {
    //-------------------------------------------------------------------------
    /**
    * @struct sulphur::engine::ButtonEvent
    * @brief Contains the data that defines a change of a button's state
    * @remarks If the button is either a mouse or keyboard button the device value will not be set
    * @see sulphur::engine::Mouse::IsButton
    * @see sulphur::engine::Keyboard::IsButton
    * @author Maarten ten Velden
    */
    struct ButtonEvent
    {
      Button button; //<! The button described by the event
      bool down; //<! The state (down/up) of the button in question
      uint device; //<! The device from which the button event originated
    };

    //-------------------------------------------------------------------------
    /**
    * @brief An enumerator that describes how the values of multiple axes are interpreted
    */
    enum struct AxisReadMode
    {
      kFirstNonZero = 0, //<! The default mode, returns the value of the first match who's value is not zero
      kAccumulate, //<! Returns the average of all matched axes
      kGreatest, //<! Returns the greatest (in abolute terms) of the matched axes
      kSmallest, //<! Returns the smallest (in absolute terms) of the matched axes
    };

    /**
    * @class sulphur::engine::Input
    * @brief An abstract interface for reading input values from multiple devices
    * @remarks The value of all input always lags one frame behind
    * @see sulphur::engine::PlatformInput
    * @author Maarten ten Velden
    */
    class Input : public foundation::BaseResource
    {
    public:
      static constexpr uint kAny = PS_UINT_MAX; //<! A constant used to indicate that all devices should be tested for input
      static constexpr uint kMaxGamepads = 4u; //<! A constant describing the maximum number of gamepads connected at a time

      /**
      *@brief constructor
      */
      Input();

      /**
      * @brief Get the current position of the cursor on the screen
      * @return (glm::vec2) The last known position of the cursor in normalized screen coordinates, ranged [-1 to 1]
      * @remarks The top left of the screen is [-1,-1] while the bottom-right of the screen is [1,1]
      */
      glm::vec2 GetMousePosition() const;
      /**
      * @brief Get the offset between the current cursor position and the previous one
      * @return (glm::vec2) The offset of the cursor (or 0.0f if mouse is not present) in normalized screen coordinates, ranged [-1 to 1]
      * @remarks The top left of the screen is [-1,-1] while the bottom-right of the screen is [1,1]
      */
      glm::vec2 GetMouseOffset() const;
      /**
      * @brief Get the offset of the mouse's scroll wheel
      * @return (float) The offset value (or 0.0f if mouse is not present)
      * @remarks A negative scroll offset indicates the user scrolling down on the y-axis, while a positive one indicates the user scrolling up on the y-axis.
      */
      float GetScrollOffset() const;

      /**
      * @brief Get the value of an axis on one or all devices
      * @param[in] axis (sulphur::engine::Axis) An enumerator describing which hardware axis to use
      * @param[in] device (uint) The device ID of which to process the input, either in the range [0 to suplhur::engine::Input::kMaxGamepads] or sulphur::engine::Input::kAny to use all devices
      * @param[in] mode (sulphur::engine::AxisReadMode) Describes how to iterate over all present devices if device is set to sulphur::engine::Input::kAny, 
      * @return (float) The resulting normalized axis value, ranged [-1 to 1] or 0.0f if the device is not present
      */
      float GetAxis(Axis axis, uint device = kAny, AxisReadMode mode = AxisReadMode::kFirstNonZero) const;
      /**
      * @brief Get the value of one or more of the mapped axes on one or all devices
      * @param[in] name (const char*) A pointer to a null-terminated tring containing the name of the mapped axis/axes
      * @param[in] device (uint) The device ID of which to process the input, either in the range [0 to suplhur::engine::Input::kMaxGamepads] or sulphur::engine::Input::kAny to use all devices
      * @param[in] mode (sulphur::engine::AxisReadMode) Describes how to iterate over all mapped axes and/or present devices if device is set to sulphur::engine::Input::kAny
      * @see sulphur::engine::Input::MapAxis
      * @return (float) The resulting normalized axis value, ranged [-1 to 1] or 0.0f if the device is not present
      */
      float GetAxis(const char* name, uint device = kAny, AxisReadMode mode = AxisReadMode::kFirstNonZero) const;

      /**
      * @brief Test if a button is rising on one or any of the devices
      * @param[in] button (sulphur::engine::Button) An enumerator describing which hardware button to use
      * @param[in] device (uint) The device ID of which to process the input, either in the range [0 to suplhur::engine::Input::kMaxGamepads] or sulphur::engine::Input::kAny to use any of the devices
      * @remarks If the button is either a mouse or keyboard button the device value will not be used
      * @see sulphur::engine::Mouse::IsButton
      * @see sulphur::engine::Keyboard::IsButton
      * @see sulphur::engine::ButtonState
      * @return (bool) Will return true if the button on any of the specified devices is in the state sulphur::engine::ButtonState::kRising or false when this is not the case or the queried device is not connected
      */
      bool IsButtonRising(Button button, uint device = kAny) const;
      /**
      * @brief Test if one or more of the mapped buttons is rising on one or any of the devices
      * @param[in] name (const char*) A pointer to a null-terminated string containing the name of the mapped button(s)
      * @param[in] device (uint) The device ID of which to process the input, either in the range [0 to suplhur::engine::Input::kMaxGamepads] or sulphur::engine::Input::kAny to use all devices
      * @remarks If the button is either a mouse or keyboard button the device value will not be used
      * @see sulphur::engine::Mouse::IsButton
      * @see sulphur::engine::Keyboard::IsButton
      * @see sulphur::engine::Input::MapButton
      * @see sulphur::engine::ButtonState
      * @return (bool) Will return true if the button on any of the mapped axes or any of the specified devices is in the state sulphur::engine::ButtonState::kRising or false when this is not the case or the queried device is not connected
      */
      bool IsButtonRising(const char* name, uint device = kAny) const;
      /**
      * @brief Test if a button is falling on one or any of the devices
      * @param[in] button (sulphur::engine::Button) An enumerator describing which hardware button to use
      * @param[in] device (uint) The device ID of which to process the input, either in the range [0 to suplhur::engine::Input::kMaxGamepads] or sulphur::engine::Input::kAny to use any of the devices
      * @remarks If the button is either a mouse or keyboard button the device value will not be used
      * @see sulphur::engine::Mouse::IsButton
      * @see sulphur::engine::Keyboard::IsButton
      * @see sulphur::engine::ButtonState
      * @return (bool) Will return true if the button on any of the specified devices is in the state sulphur::engine::ButtonState::kRising or false when this is not the case or the queried device is not connected
      */
      bool IsButtonFalling(Button button, uint device = kAny) const;
      /**
      * @brief Test if one or more of the mapped buttons is falling on one or any of the devices
      * @param[in] name (const char*) A pointer to a null-terminated string containing the name of the mapped button(s)
      * @param[in] device (uint) The device ID of which to process the input, either in the range [0 to suplhur::engine::Input::kMaxGamepads] or sulphur::engine::Input::kAny to use all devices
      * @remarks If the button is either a mouse or keyboard button the device value will not be used
      * @see sulphur::engine::Mouse::IsButton
      * @see sulphur::engine::Keyboard::IsButton
      * @see sulphur::engine::Input::MapButton
      * @see sulphur::engine::ButtonState
      * @return (bool) Will return true if the button on any of the mapped axes or any of the specified devices is in the state sulphur::engine::ButtonState::kFalling or false when this is not the case or the queried device is not connected
      */
      bool IsButtonFalling(const char* name, uint device = kAny) const;
      /**
      * @brief Test if a button is down on one or any of the devices
      * @param[in] button (sulphur::engine::Button) An enumerator describing which hardware button to use
      * @param[in] device (uint) The device ID of which to process the input, either in the range [0 to suplhur::engine::Input::kMaxGamepads] or sulphur::engine::Input::kAny to use any of the devices
      * @remarks If the button is either a mouse or keyboard button the device value will not be used
      * @see sulphur::engine::Mouse::IsButton
      * @see sulphur::engine::Keyboard::IsButton
      * @see sulphur::engine::ButtonState
      * @return (bool) Will return true if the button on any of the specified devices is in the state sulphur::engine::ButtonState::kDown or false when this is not the case or the queried device is not connected
      */
      bool IsButtonDown(Button button, uint device = kAny) const;
      /**
      * @brief Test if one or more of the mapped buttons is down on one or any of the devices
      * @param[in] name (const char*) A pointer to a null-terminated string containing the name of the mapped button(s)
      * @param[in] device (uint) The device ID of which to process the input, either in the range [0 to suplhur::engine::Input::kMaxGamepads] or sulphur::engine::Input::kAny to use all devices
      * @remarks If the button is either a mouse or keyboard button the device value will not be used
      * @see sulphur::engine::Mouse::IsButton
      * @see sulphur::engine::Keyboard::IsButton
      * @see sulphur::engine::Input::MapButton
      * @see sulphur::engine::ButtonState
      * @return (bool) Will return true if the button on any of the mapped axes or any of the specified devices is in the state sulphur::engine::ButtonState::kDown or false when this is not the case or the queried device is not connected
      */
      bool IsButtonDown(const char* name, uint device = kAny) const;
      /**
      * @brief Test if a button is up on one or any of the devices
      * @param[in] button (sulphur::engine::Button) An enumerator describing which hardware button to use
      * @param[in] device (uint) The device ID of which to process the input, either in the range [0 to suplhur::engine::Input::kMaxGamepads] or sulphur::engine::Input::kAny to use any of the devices
      * @remarks If the button is either a mouse or keyboard button the device value will not be used
      * @see sulphur::engine::Mouse::IsButton
      * @see sulphur::engine::Keyboard::IsButton
      * @see sulphur::engine::ButtonState
      * @return (bool) Will return true if the button on any of the specified devices is in the state sulphur::engine::ButtonState::kUp or false when this is not the case or the queried device is not connected
      */
      bool IsButtonUp(Button button, uint device = kAny) const;
      /**
      * @brief Test if one or more of the mapped buttons is up on one or any of the devices
      * @param[in] name (const char*) A pointer to a null-terminated string containing the name of the mapped button(s)
      * @param[in] device (uint) The device ID of which to process the input, either in the range [0 to suplhur::engine::Input::kMaxGamepads] or sulphur::engine::Input::kAny to use all devices
      * @remarks If the button is either a mouse or keyboard button the device value will not be used
      * @see sulphur::engine::Mouse::IsButton
      * @see sulphur::engine::Keyboard::IsButton
      * @see sulphur::engine::Input::MapButton
      * @see sulphur::engine::ButtonState
      * @return (bool) Will return true if the button on any of the mapped axes or any of the specified devices is in the state sulphur::engine::ButtonState::kUp or false when this is not the case or the queried device is not connected
      */
      bool IsButtonUp(const char* name, uint device = kAny) const;

      /**
      * @brief Test if a mouse is present
      * @return (bool) True if a mouse is present
      */
      bool IsMousePresent() const;
      /**
      * @brief Test if a keyboard is present
      * @return (bool) True if a keyboard is present
      */
      bool IsKeyboardPresent() const;
      /**
      * @brief Test if a specific gamepad is present
      * @param[in] device (uint) The device ID of the gamepad in the range [0 to suplhur::engine::Input::kMaxGamepads]
      * @return (bool) True if the requested gamepad is present
      */
      bool IsGamepadPresent(uint device) const;

      /**
      * @brief Retrieve all the mapped axes associated with a specific name string
      * @param[in] name (const char*) A pointer to a null-terminated string containing the name of the mapped axis/axes
      * @return (sulphur::foundation::Vector <sulphur::engine::Axis>) A copy of the vector containing each of the individual mapped axes
      */
      foundation::Vector<Axis> GetMappedAxes(const char* name) const;
      /**
      * @brief Retrieve all the mapped buttons associated with a specific name string
      * @param[in] name (const char*) A pointer to a null-terminated string containing the name of the mapped button(s)
      * @return (sulphur::foundation::Vector <sulphur::engine::Button>) A copy of the vector containing each of the individual mapped buttons
      */
      foundation::Vector<Button> GetMappedButtons(const char* name) const;
      /**
      * @brief Get a string representing all the unicode characters that were input this frame.
      * @return (const sulphur::foundation::String&) The string containing the characters as generated by the operating system
      * @remarks Obeys keyboard layout, modifier keys, and characters composed using dead keys.
      */
      const foundation::String& GetInputString() const;
      /**
      * @brief Get a vector representing all the individual button-input events that occured since the last frame, in order that the messages were received
      * @return (sulphur::foundation::Vector <sulphur::engine::ButtonEvent>) A copy of the vector containing each of the individual button events
      */
      const foundation::Vector<ButtonEvent>& GetInputData() const;

      /**
      * @brief Setup a string to use as alias to access one or multiple axes
      * @param[in] name (const char*) A pointer to a null-terminated string containing the name of the mapped axis/axes
      * @param[in] axis (sulphur::engine::Axis) An enumerator describing which hardware axis is being refered to
      * @remarks If the string alias has already been used, this will add the axis under that same alias (multiple instances of the same axis are ignored)
      */
      void MapAxis(const char* name, Axis axis);
      /**
      * @brief Setup a string to use as alias to access one or multiple buttons
      * @param[in] name (const char*) A pointer to a null-terminated string containing the name of the mapped button(s)
      * @param[in] button (sulphur::engine::Button) An enumerator describing which hardware button is being refered to
      * @remarks If the string alias has already been used, this will add the button under that same alias (multiple instances of the same axis are ignored)
      */
      void MapButton(const char* name, Button button);
      /**
      * @brief Remove an axis from the axes mapped under the specified name
      * @param[in] name (const char*) A pointer to a null-terminated string containing the name of the mapped axis/axes to unmap
      * @param[in] axis (sulphur::engine::Axis) An enumerator describing which hardware axis to unmap
      * @remarks Does nothing if the axis has not been mapped to the specified name
      */
      void UnmapAxis(const char* name, Axis axis);
      /**
      * @brief Remove a button from the buttons mapped under the specified name
      * @param[in] name (const char*) A pointer to a null-terminated string containing the name of the mapped button(s) to unmap
      * @param[in] button (sulphur::engine::Button) An enumerator describing which hardware button to unmap
      * @remarks Does nothing if the button has not been mapped to the specified name
      */
      void UnmapButton(const char* name, Button button);
      /**
      * @brief Remove all axes mapped under the specified name
      * @param[in] name (const char*) A pointer to a null-terminated string containing the name of the mapped axis/axes to unmap
      * @remarks Does nothing if no axes have been mapped to the specified name
      */
      void UnmapAxes(const char* name);
      /**
      * @brief Remove all buttons mapped under the specified name
      * @param[in] name (const char*) A pointer to a null-terminated string containing the name of the mapped button(s) to unmap
      * @remarks Does nothing if no buttons have been mapped to the specified name
      */
      void UnmapButtons(const char* name);

    private:
      /**
      * @brief Test if a button is in the specified state on one or any of the devices
      * @param[in] button (sulphur::engine::Button) An enumerator describing which hardware button to use
      * @param[in] state (sulphur::engine::ButtonState) An enumerator describing which state to test against
      * @param[in] device (uint) The device ID of which to process the input, either in the range [0 to suplhur::engine::Input::kMaxGamepads] or sulphur::engine::Input::kAny to use any of the devices
      * @remarks If the button is either a mouse or keyboard button the device value will not be used
      * @see sulphur::engine::Mouse::IsButton
      * @see sulphur::engine::Keyboard::IsButton
      * @return (bool) Will return true if the button on any of the specified devices is in the specified state or false when this is not the case or the queried device is not connected
      */
      bool IsButtonState(Button button, ButtonState state, uint device) const;
      /**
      * @brief Test if one or more of the mapped buttons is in the specified state on one or any of the devices
      * @param[in] name (const char*) A pointer to a null-terminated string containing the name of the mapped button(s)
      * @param[in] state (sulphur::engine::ButtonState) An enumerator describing which state to test against
      * @param[in] device (uint) The device ID of which to process the input, either in the range [0 to suplhur::engine::Input::kMaxGamepads] or sulphur::engine::Input::kAny to use all devices
      * @remarks If the button is either a mouse or keyboard button the device value will not be used
      * @see sulphur::engine::Mouse::IsButton
      * @see sulphur::engine::Keyboard::IsButton
      * @see sulphur::engine::Input::MapButton
      * @return (bool) Will return true if the button on any of the mapped axes or any of the specified devices is in the specified state or false when this is not the case or the queried device is not connected
      */
      bool IsButtonState(const char* name, ButtonState state, uint device) const;

      /**
      * @brief Test if a mouse button is in the specified state
      * @param[in] button (sulphur::engine::Button) An enumerator describing which hardware button to use
      * @param[in] state (sulphur::engine::ButtonState) An enumerator describing which state to test against
      * @remarks If the button is not a mouse button this will fail/assert
      * @see sulphur::engine::Mouse::IsButton
      * @return (bool) Will return true if the button is in the specified state or false when this is not the case or a mouse is not connected
      */
      bool IsMouseButtonState(Button button, ButtonState state) const;
      /**
      * @brief Test if a keyboard button is in the specified state
      * @param[in] button (sulphur::engine::Button) An enumerator describing which hardware button to use
      * @param[in] state (sulphur::engine::ButtonState) An enumerator describing which state to test against
      * @remarks If the button is not a keyboard button this will fail/assert
      * @see sulphur::engine::Keyboard::IsButton
      * @return (bool) Will return true if the button is in the specified state or false when this is not the case or a keyboard is not connected
      */
      bool IsKeyboardButtonState(Button button, ButtonState state) const;
      /**
      * @brief Test if a gamepad button is in the specified state on one or any of the devices
      * @param[in] button (sulphur::engine::Button) An enumerator describing which hardware button to use
      * @param[in] state (sulphur::engine::ButtonState) An enumerator describing which state to test against
      * @param[in] device (uint) The device ID of which to process the input, either in the range [0 to suplhur::engine::Input::kMaxGamepads] or sulphur::engine::Input::kAny to use any of the devices
      * @remarks If the button is not a gamepad button this will fail/assert
      * @see sulphur::engine::Gamepad::IsButton
      * @return (bool) Will return true if the button on any of the specified devices is in the specified state or false when this is not the case or the queried device is not connected
      */
      bool IsGamepadButtonState(Button button, ButtonState state, uint device) const;

    protected:
      Mouse mouse; //<! An abstraction class storing the state of the mouse input
      Keyboard keyboard; //<! An abstraction class storing the state of the keyboard input
      Gamepad gamepads[kMaxGamepads]; //<! An array of abstraction classes storing the state of the gamepad input of each device

      foundation::String input_string; //<! A string representing all the unicode characters that were input this frame.
      foundation::Vector<ButtonEvent> input_data; //<! A vector representing all the individual button-input events that occured since the last frame, in order that the messages were received
      
      foundation::HashMultiMap<foundation::String, Axis> axis_map; //<! A container that maps strings to axes
      foundation::HashMultiMap<foundation::String, Button> button_map; //<! A container that maps strings to buttons

    };

    //-------------------------------------------------------------------------
    /**
    * @class sulphur::engine::PlatformInput : sulphur::engine::Input
    * @brief A write-access wrapper for modifying the contents of the input system
    * @see sulphur::engine::Input
    * @see sulphur::engine::InputServer
    * @author Maarten ten Velden
    */
    class PlatformInput : public Input
    {
    public:
      /**
      * @brief Sets the input received during the last frame as the current read-input and sets up a new frame
      * @see sulphur::engine::Mouse::StartFrame
      * @see sulphur::engine::Keyboard::StartFrame
      * @see sulphur::engine::Gamepad::StartFrame
      */
      void StartFrame();

      /**
      * @brief Set the next position of the cursor
      * @param[in] position (glm::vec2) The position, that will be used next frame in normalized screen coordinates, ranged [-1 to 1]
      * @remarks The top left of the screen is [-1,-1] while the bottom-right of the screen is [1,1]
      * @see sulphur::engine::PlatformInput::StartFrame
      */
      void SetMousePosition(glm::vec2 position);
      /**
      * @brief Set the next offset of the mouse's scroll wheel
      * @return (float) The offset value that will be used next frame
      * @remarks A negative scroll offset indicates the user scrolling down on the y-axis, while a positive one indicates the user scrolling up on the y-axis.
      */
      void SetScrollOffset(float offset);
      /**
      * @brief Set the next state of an axis on a specific device
      * @param[in] axis (sulphur::engine::Axis) An enumerator describing which hardware axis to use
      * @param[in] value (float) The value of the axis, that will be used next frame
      * @param[in] device (uint) The device ID of the gamepad in the range [0 to suplhur::engine::Input::kMaxGamepads]
      * @see sulphur::engine::PlatformInput::StartFrame
      */
      void SetAxis(Axis axis, float value, uint device = 0);
      /**
      * @brief Set the next state of a button on a specific device
      * @param[in] button (sulphur::engine::Button) An enumerator describing which hardware button to use
      * @param[in] down (bool) The state of the button, that will be used next frame
      * @param[in] device (uint) The device ID of the gamepad in the range [0 to suplhur::engine::Input::kMaxGamepads]
      * @see sulphur::engine::PlatformInput::StartFrame
      */
      void SetButtonDown(Button button, bool down, uint device = 0);
      
      /**
      * @brief Set if a mouse will be present from the next frame onward
      * @param[in] present (bool) True if the mouse will be present next frame
      * @see sulphur::engine::PlatformInput::StartFrame
      * @remarks Presency is used by the input system to return default values instead of the actual current values when a device is not currently present
      */
      void SetMousePresent(bool present);
      /**
      * @brief Set if a mouse will be present from the next frame onward
      * @param[in] present (bool) True if the mouse will be present next frame
      * @see sulphur::engine::PlatformInput::StartFrame
      * @remarks Presency is used by the input system to return default values instead of the actual current values when a device is not currently present
      */
      void SetKeyboardPresent(bool present);
      /**
      * @brief Set if a specific gamepad will be present from the next frame onward
      * @param[in] present (bool) True if the gamepad will be present next frame
      * @param[in] device (uint) The device ID of the gamepad in the range [0 to suplhur::engine::Input::kMaxGamepads]
      * @see sulphur::engine::PlatformInput::StartFrame
      * @remarks Presency is used by the input system to return default values instead of the actual current values when a device is not currently present
      */
      void SetGamepadPresent(bool present, uint device);

      /**
      * @brief Add a string of characters to the next frame's input string
      * @param[in] string (const char*) A pointer to a null-terminated string containing the character codes to append
      * @see sulphur::engine::PlatformInput::StartFrame
      */
      void AppendInputString(const char* string);
      /**
      * @brief Add a single character to the next frame's input string
      * @param[in] character (char) The character code to append
      * @see sulphur::engine::PlatformInput::StartFrame
      */
      void AppendInputString(char character);
    };

    //-------------------------------------------------------------------------
    /**
    * @struct sulphur::engine::ScriptableInput : public sulphur::engine::ScriptableObject
    * @brief The input system exposed to the scripting environment, statically
    * @author Daniel Konings
    */
    SCRIPT_CLASS() class ScriptableInput : public ScriptableObject
    {

    public:

      SCRIPT_NAME(Input);

      /**
      * @brief Initializes the scriptable input with the input system
      */
      static void Initialize(Input* input);

      /**
      * @see sulphur::engine::Input::IsButtonFalling
      */
      SCRIPT_FUNC(static) bool IsButtonFalling(Button button);

      /**
      * @see sulphur::engine::Input::IsButtonRising
      */
      SCRIPT_FUNC(static) bool IsButtonRising(Button button);

      /**
      * @see sulphur::engine::Input::IsButtonUp
      */
      SCRIPT_FUNC(static) bool IsButtonUp(Button button);

      /**
      * @see sulphur::engine::Input::IsButtonDown
      */
      SCRIPT_FUNC(static) bool IsButtonDown(Button button);

      /**
      * @see sulphur::engine::Input::GetAxis
      */
      SCRIPT_FUNC(static) float GetAxis(Axis axis);

    private:

      static Input* input_; //!< The actual input system
    };
  }
}
