#include "scriptable_imgui.h"

#include "engine/scripting/scriptable_value.h"
#include "engine/scripting/script_system.h"

#include <engine/graphics/imgui/imgui.h>

#include <lua-classes/scriptable_imgui.lua.cc>

namespace sulphur
{
  namespace engine
  {
    ScriptState* ScriptableImGui::script_state_ = nullptr;

    void ScriptableImGui::Initialize(Application & app)
    {
      script_state_ = app.GetService<ScriptSystem>().script_state();
    }

    //-------------------------------------------------------------------------
    void ScriptableImGui::Begin(const foundation::String& name, glm::vec2 size)
    {
      bool open;
      ImGui::Begin(name.c_str(), &open, size);
    }

    //-------------------------------------------------------------------------
    void ScriptableImGui::End()
    {
      ImGui::End();
    }

    //-------------------------------------------------------------------------
    void ScriptableImGui::BeginChild(const foundation::String & name, glm::vec2 size, bool border)
    {
      ImGui::BeginChild(name.c_str(), size, border);
    }

    //-------------------------------------------------------------------------
    void ScriptableImGui::EndChild()
    {
      ImGui::EndChild();
    }

    //-------------------------------------------------------------------------
    bool ScriptableImGui::Button(const foundation::String & name, glm::vec2 size)
    {
      return ImGui::Button(name.c_str(), size);
    }

    //-------------------------------------------------------------------------
    void ScriptableImGui::Text(const foundation::String& text)
    {
      ImGui::Text("%s", text.c_str());
    }

    //-------------------------------------------------------------------------
    void ScriptableImGui::SetNextWindowPos(glm::vec2 pos)
    {
      ImGui::SetNextWindowPos(pos);
    }

    //-------------------------------------------------------------------------
    void ScriptableImGui::SameLine()
    {
      ImGui::SameLine();
    }

    //-------------------------------------------------------------------------
    ScriptableImGuiInputText ScriptableImGui::InputText(const foundation::String& name, const foundation::String& val, int size, int flags)
    {
      foundation::String buffer = val;
      buffer.resize(size);
    
      bool result = ImGui::InputText(name.c_str(), &buffer[0] , size, flags);

      ScriptableImGuiInputText res;
      res.result_value = result;
      res.text_value = buffer;
      return res;
    }

    //-------------------------------------------------------------------------
    foundation::String ScriptableImGuiInputText::text()
    {
      return text_value;
    }

    //-------------------------------------------------------------------------
    bool ScriptableImGuiInputText::result()
    {
      return result_value;
    }
  }
}