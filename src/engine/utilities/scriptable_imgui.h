#pragma once
#include "engine/scripting/scriptable_object.h"
#include "engine/application/application.h"
#include <glm/glm.hpp>

namespace sulphur
{
  namespace engine
  {
    SCRIPT_CLASS() class ScriptableImGuiInputText : public ScriptableObject
    {
    public:
      SCRIPT_NAME(ScriptableImGuiInputText);
      SCRIPT_FUNC() foundation::String text();
      SCRIPT_FUNC() bool result();
      foundation::String text_value;
      bool result_value;
    };

    /**
    * @struct sulphur::engine::ScriptableImGui
    * @brief Imgui exposed to Lua, covers the most used functions, additional functions can be wrapped
    * @author Joren Bolhuis
    */
    SCRIPT_CLASS() class ScriptableImGui : public ScriptableObject
    {          
      /**
      * @enum sulphur::engine::ScriptableImGui::ImGuiInputTextFlags
      */
      SCRIPT_ENUM() enum ImGuiInputTextFlags
      {
        CharsDecimal = 1,   // Allow 0123456789.+-*/
        CharsHexadecimal = 2,   // Allow 0123456789ABCDEFabcdef
        CharsUppercase = 4,   // Turn a..z into A..Z
        CharsNoBlank = 8,   // Filter out spaces, tabs
        AutoSelectAll = 16,   // Select entire text when first taking mouse focus
        EnterReturnsTrue = 32,   // Return 'true' when Enter is pressed (as opposed to when the value was modified)
        CallbackCompletion = 64,   // Call user function on pressing TAB (for completion handling)
        CallbackHistory = 128,   // Call user function on pressing Up/Down arrows (for history handling)
        CallbackAlways = 256,   // Call user function every time. User code may query cursor position, modify text buffer.
        CallbackCharFilter = 512,   // Call user function to filter character. Modify data->EventChar to replace/filter input, or return 1 to discard character.
        AllowTabInput = 1024,  // Pressing TAB input a '\t' character into the text field
        CtrlEnterForNewLine = 2048,  // In multi-line mode, unfocus with Enter, add new line with Ctrl+Enter (default is opposite: unfocus with Ctrl+Enter, add line with Enter).
        NoHorizontalScroll = 4096,  // Disable following the cursor horizontally
        AlwaysInsertMode = 8192,  // Insert mode
        ReadOnly = 16384,  // Read-only mode
        Password = 32768  // Password mode, display all characters as '*'
      };

    public:
      SCRIPT_NAME(ImGui);

      static void Initialize(Application& app);

      /**
      * @brief ImGui begin function
      * @param[in] name (const sulphur::foundation::String&) The name of the window
      * @param[in] size (glm::vec2) The size of the window
      */
      SCRIPT_FUNC(static) void Begin(const foundation::String& name, glm::vec2 size);
      /**
      * @brief ImGui end function
      */
      SCRIPT_FUNC(static) void End();
      /**
      * @brief ImGui begin child function
      * @param[in] name (const sulphur::foundation::String&) The name of the window
      * @param[in] size (glm::vec2) The size of the window
      * @param[in] border (bool) Border yes no?
      */
      SCRIPT_FUNC(static) void BeginChild(const foundation::String& name, glm::vec2 size, bool border);
      /**
      * @brief ImGui begin child function
      * @param[in] name (const sulphur::foundation::String&) The name of the window
      * @param[in] size (glm::vec2) The size of the window
      */
      SCRIPT_FUNC(static) void EndChild();
      /**
      * @brief ImGui button function
      * @param[in] name (const sulphur::foundation::String&) The name of the window
      * @param[in] size (glm::vec2) The size of the window
      */
      SCRIPT_FUNC(static) bool Button(const foundation::String& name, glm::vec2 size);
      /**
      * @brief ImGui text function
      * @param[in] text (const sulphur::foundation::String&) The text to display
      */
      SCRIPT_FUNC(static) void Text(const foundation::String& text);
      /**
      * @brief ImGui set next window position function
      * @param[in] pos (const glm::vec2&) The position
      */
      SCRIPT_FUNC(static) void SetNextWindowPos(glm::vec2 pos);
      /**
      * @brief ImGui same line
      */
      SCRIPT_FUNC(static) void SameLine();
      /**
      * @brief ImGui Input text
      */
      SCRIPT_FUNC(static) ScriptableImGuiInputText InputText(const foundation::String& name, const foundation::String& val, int size, int flags);
    private:
      static ScriptState* script_state_;
    };
  }
}