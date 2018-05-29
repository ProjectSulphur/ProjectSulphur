#include "input.h"

#include <foundation/logging/logger.h>
#include <algorithm>

#include <lua-classes/input.lua.cc>
#include <lua-classes/mappings.lua.cc>

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    Input::Input() :
      BaseResource("Input")
    {
    }

    //-------------------------------------------------------------------------
    Input* ScriptableInput::input_ = nullptr;

    //-------------------------------------------------------------------------
    void ScriptableInput::Initialize(Input* input)
    {
      input_ = input;
    }

    //-------------------------------------------------------------------------
    bool ScriptableInput::IsButtonFalling(Button button)
    {
      return input_->IsButtonFalling(button, Input::kAny);
    }

    //-------------------------------------------------------------------------
    bool ScriptableInput::IsButtonRising(Button button)
    {
      return input_->IsButtonRising(button, Input::kAny);
    }

    //-------------------------------------------------------------------------
    bool ScriptableInput::IsButtonUp(Button button)
    {
      return input_->IsButtonUp(button, Input::kAny);
    }

    //-------------------------------------------------------------------------
    bool ScriptableInput::IsButtonDown(Button button)
    {
      return input_->IsButtonDown(button, Input::kAny);
    }

    //-------------------------------------------------------------------------
    float ScriptableInput::GetAxis(Axis axis)
    {
      return input_->GetAxis(axis);
    }

    //-------------------------------------------------------------------------
    glm::vec2 Input::GetMousePosition() const
    {
      return mouse.position();
    }
    glm::vec2 Input::GetMouseOffset() const
    {
      if (mouse.present() == false)
      {
        return glm::vec2(0.f);
      }

      return mouse.GetOffset();
    }
    float Input::GetScrollOffset() const
    {
      if (mouse.present() == false)
      {
        return 0.f;
      }

      return mouse.scroll_offset();
    }

    //-------------------------------------------------------------------------
    using AxisReadFunc = void(*)(float, float&, int&);
    inline AxisReadFunc GetAxisReadFunc(AxisReadMode mode)
    {
      switch (mode)
      {
      case AxisReadMode::kFirstNonZero:
        return [](float value, float& final_value, int& denominator)
        {
          if (value != 0.0f)
          {
            final_value = value;
          }
          denominator = 1;
        };
      case AxisReadMode::kAccumulate:
        return [](float value, float& final_value, int& denominator)
        {
          value += final_value;
          ++denominator;
        };
      case AxisReadMode::kGreatest:
        return [](float value, float& final_value, int& denominator)
        {
          if (std::fabs(value) > std::fabs(final_value))
          {
            final_value = value;
          }
          denominator = 1;
        };
      case AxisReadMode::kSmallest:
        return [](float value, float& final_value, int& denominator)
        {
          if (std::fabs(value) < std::fabs(final_value))
          {
            final_value = value;
          }
          denominator = 1;
        };
      default:
        assert(false && "Invalid read-mode specified");
        return nullptr;
      }
    }

    //-------------------------------------------------------------------------
    float Input::GetAxis(Axis axis, uint device, AxisReadMode mode) const
    {
      if (Gamepad::IsAxis(axis) == true)
      {
        if (device == kAny)
        {
          // Iterate and process (based on strategy) requested axis of all gamepad devices.
          auto iterate_axes = [&](AxisReadFunc iterate_func)
          {
            float final_value = 0.0f;
            int denominator = 0;

            for (const Gamepad& gamepad : gamepads)
            {
              iterate_func(gamepad.GetAxis(axis), final_value, denominator);
            }

            return final_value / std::max(denominator, 1);
          };

          return iterate_axes(GetAxisReadFunc(mode));
        }
        else
        {
          // Lookup axis of specific gamepad.
          assert(device < kMaxGamepads && "Invalid gamepad checked");
          if (gamepads[device].present() == false)
          {
            return 0.0f;
          }
          return gamepads[device].GetAxis(axis);
        }
      }

      foundation::DefaultLogger::Log(foundation::Verbosity::kWarning, "Invalid axis checked");
      return 0.0f;
    }
    float Input::GetAxis(const char* name, uint device, AxisReadMode mode) const
    {
      using Iterator = foundation::HashMultiMap<foundation::String, Axis>::const_iterator;
      using RangeIterator = eastl::pair<Iterator, Iterator>;

      // Iterate and process (based on strategy) all mapped axes.
      auto iterate_axes = [&](AxisReadFunc iterate_func)
      {
        float final_value = 0.0f;
        int denominator = 0;

        RangeIterator range = axis_map.equal_range(foundation::String(name));
        for (Iterator it = range.first; it != range.second; ++it)
        {
          iterate_func(GetAxis(it->second, device), final_value, ++denominator);
        }

        return final_value / std::max(denominator, 1);
      };

      return iterate_axes(GetAxisReadFunc(mode));
    }

    //-------------------------------------------------------------------------
    bool Input::IsButtonRising(Button button, uint device) const
    {
      return IsButtonState(button, ButtonState::kRising, device);
    }
    bool Input::IsButtonRising(const char * name, uint device) const
    {
      return IsButtonState(name, ButtonState::kFalling, device);
    }

    //-------------------------------------------------------------------------
    bool Input::IsButtonFalling(Button button, uint device) const
    {
      return IsButtonState(button, ButtonState::kFalling, device);
    }
    bool Input::IsButtonFalling(const char * name, uint device) const
    {
      return IsButtonState(name, ButtonState::kFalling, device);
    }

    //-------------------------------------------------------------------------
    bool Input::IsButtonDown(Button button, uint device) const
    {
      return IsButtonState(button, ButtonState::kDown, device);
    }
    bool Input::IsButtonDown(const char* name, uint device) const
    {
      return IsButtonState(name, ButtonState::kDown, device);
    }

    //-------------------------------------------------------------------------
    bool Input::IsButtonUp(Button button, uint device) const
    {
      return IsButtonState(button, ButtonState::kUp, device);
    }
    bool Input::IsButtonUp(const char * name, uint device) const
    {
      return IsButtonState(name, ButtonState::kUp, device);
    }

    //-------------------------------------------------------------------------
    bool Input::IsMousePresent() const
    {
      return mouse.present();
    }
    bool Input::IsKeyboardPresent() const
    {
      return keyboard.present();
    }
    bool Input::IsGamepadPresent(uint device) const
    {
      assert(device < kMaxGamepads && "Invalid gamepad checked");
      return gamepads[device].present();
    }

    //-------------------------------------------------------------------------
    foundation::Vector<Axis> Input::GetMappedAxes(const char* name) const
    {
      foundation::Vector<Axis> result;

      using Iterator = foundation::HashMultiMap<foundation::String, Axis>::const_iterator;
      using RangeIterator = eastl::pair<Iterator, Iterator>;

      RangeIterator range = axis_map.equal_range(foundation::String(name));
      for (Iterator it = range.first; it != range.second; ++it)
      {
        result.emplace_back(it->second);
      }

      return result;
    }
    foundation::Vector<Button> Input::GetMappedButtons(const char* name) const
    {
      foundation::Vector<Button> result;

      using Iterator = foundation::HashMultiMap<foundation::String, Button>::const_iterator;
      using RangeIterator = eastl::pair<Iterator, Iterator>;

      RangeIterator range = button_map.equal_range(foundation::String(name));
      for (Iterator it = range.first; it != range.second; ++it)
      {
        result.emplace_back(it->second);
      }

      return result;
    }
    const foundation::String& Input::GetInputString() const
    {
      return input_string;
    }
    const foundation::Vector<ButtonEvent>& Input::GetInputData() const
    {
      return input_data;
    }

    //-------------------------------------------------------------------------
    void Input::MapAxis(const char* name, Axis axis)
    {
      assert(Gamepad::IsAxis(axis) == true && "Invalid axis-code passed");

      using Iterator = foundation::HashMultiMap<foundation::String, Axis>::const_iterator;
      using RangeIterator = eastl::pair<Iterator, Iterator>;

      // Test if the axis is already mapped
      RangeIterator range = axis_map.equal_range(foundation::String(name));
      for (Iterator it = range.first; it != range.second; ++it)
      {
        if (it->second == axis)
        {
          return;
        }
      }

      axis_map.insert(eastl::make_pair(foundation::String(name), axis));
    }
    void Input::MapButton(const char* name, Button button)
    {
      assert(
        Keyboard::IsButton(button) == true ||
        Mouse::IsButton(button) == true ||
        Gamepad::IsButton(button) == true && "Invalid button-code passed");

      using Iterator = foundation::HashMultiMap<foundation::String, Button>::const_iterator;
      using RangeIterator = eastl::pair<Iterator, Iterator>;

      // Test if the button is already mapped
      RangeIterator range = button_map.equal_range(foundation::String(name));
      for (Iterator it = range.first; it != range.second; ++it)
      {
        if (it->second == button)
        {
          return;
        }
      }

      button_map.insert(eastl::make_pair(foundation::String(name), button));
    }
    void Input::UnmapAxis(const char* name, Axis axis)
    {
      using Iterator = foundation::HashMultiMap<foundation::String, Axis>::const_iterator;
      using RangeIterator = eastl::pair<Iterator, Iterator>;

      // Find the mapped element matching the specific axis
      RangeIterator range = axis_map.equal_range(foundation::String(name));
      for (Iterator it = range.first; it != range.second; ++it)
      {
        if (it->second == axis)
        {
          axis_map.erase(it);
          return; // As there is only one instance of the same axis present at all times, we can safely break here
        }
      }
    }
    void Input::UnmapButton(const char* name, Button button)
    {
      using Iterator = foundation::HashMultiMap<foundation::String, Button>::const_iterator;
      using RangeIterator = eastl::pair<Iterator, Iterator>;

      // Find the mapped element matching the specific button
      RangeIterator range = button_map.equal_range(foundation::String(name));
      for (Iterator it = range.first; it != range.second; ++it)
      {
        if (it->second == button)
        {
          button_map.erase(it);
          return; // As there is only one instance of the same button present at all times, we can safely break here
        }
      }
    }
    void Input::UnmapAxes(const char* name)
    {
      axis_map.erase(foundation::String(name));
    }
    void Input::UnmapButtons(const char* name)
    {
      button_map.erase(foundation::String(name));
    }

    //-------------------------------------------------------------------------
    bool Input::IsButtonState(Button button, ButtonState state, uint device) const
    {
      if (Keyboard::IsButton(button) == true)
      {
        return IsKeyboardButtonState(button, state);
      }
      else if (Gamepad::IsButton(button) == true)
      {
        return IsGamepadButtonState(button, state, device);
      }
      else if (Mouse::IsButton(button) == true)
      {
        return IsMouseButtonState(button, state);
      }

      foundation::DefaultLogger::Log(foundation::Verbosity::kWarning, "Invalid button checked");

      return false;
    }
    bool Input::IsButtonState(const char* name, ButtonState state, uint device) const
    {
      using Iterator = foundation::HashMultiMap<foundation::String, Button>::const_iterator;
      using RangeIterator = eastl::pair<Iterator, Iterator>;

      RangeIterator range = button_map.equal_range(foundation::String(name));
      for (Iterator it = range.first; it != range.second; ++it)
      {
        if (IsButtonState(it->second, state, device) == true)
        {
          return true;
        }
      }
      return false;
    }

    //-------------------------------------------------------------------------
    bool Input::IsMouseButtonState(Button button, ButtonState state) const
    {
      if (mouse.present() == false)
      {
        return false;
      }

      return mouse.GetButton(button) == state;
    }
    bool Input::IsKeyboardButtonState(Button button, ButtonState state) const
    {
      if (keyboard.present() == false)
      {
        return false;
      }

      return keyboard.GetButton(button) == state;
    }
    bool Input::IsGamepadButtonState(Button button, ButtonState state, uint device) const
    {
      // Compare input for all of the devices
      if (device == kAny)
      {
        for(const Gamepad& gamepad : gamepads)
        {
          if (gamepad.present() == true && gamepad.GetButton(button) == state)
          {
            return true;
          }
        }
        return false;
      }
      // Compare input from a specific device
      else
      {
        if (device >= kMaxGamepads)
        {
          foundation::DefaultLogger::Log(foundation::Verbosity::kWarning, "Invalid gamepad checked");
          return false;
        }

        if(gamepads[device].present() == false)
        {
          return false;
        }
        return gamepads[device].GetButton(button) == state;
      }
    }

    //-------------------------------------------------------------------------
    void PlatformInput::StartFrame()
    {
      mouse.StartFrame();
      keyboard.StartFrame();
      for(Gamepad& gamepad : gamepads)
      {
        gamepad.StartFrame();
      }

      input_data.clear();
      input_string.clear();
    }

    //-------------------------------------------------------------------------
    void PlatformInput::SetButtonDown(Button button, bool down, uint device)
    {
      if (Keyboard::IsButton(button) == true)
      {
        keyboard.SetButton(button, down);
      }
      else if (Gamepad::IsButton(button) == true)
      {
        gamepads[device].SetButton(button, down);
      }
      else if (Mouse::IsButton(button) == true)
      {
        mouse.SetButton(button, down);
      }

      // Save input-event
      ButtonEvent event;
      event.button = button;
      event.down = down;
      event.device = device;
      input_data.emplace_back(event);
    }
    void PlatformInput::SetAxis(Axis axis, float value, uint device)
    {
      assert(Gamepad::IsAxis(axis) == true && "Invalid axis-code passed");
      assert(device < kMaxGamepads && "Invalid gamepad checked");
      gamepads[device].SetAxis(axis, value);
    }
    void PlatformInput::SetMousePosition(glm::vec2 position)
    {
      mouse.SetPosition(position);
    }
    void PlatformInput::SetScrollOffset(float offset)
    {
      mouse.SetScrollOffset(offset);
    }

    //-------------------------------------------------------------------------
    void PlatformInput::SetMousePresent(bool present)
    {
      mouse.SetPresent(present);
    }
    void PlatformInput::SetKeyboardPresent(bool present)
    {
      keyboard.SetPresent(present);
    }
    void PlatformInput::SetGamepadPresent(bool present, uint device)
    {
      assert(device < kMaxGamepads && "Invalid gamepad checked");
      gamepads[device].SetPresent(present);
    }

    //-------------------------------------------------------------------------
    void PlatformInput::AppendInputString(const char* string)
    {
      input_string += string;
    }
    void PlatformInput::AppendInputString(char character)
    {
      input_string += character;
    }
}
}

