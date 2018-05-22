#include "engine/scripting/wrappers/script_shader.h"
#include "engine/scripting/script_system.h"
#include "engine/assets/shader.h"
#include "engine/scripting/scriptable_values/scriptable_args.h"

void sulphur::engine::ScriptShader::New(ScriptableArgs* args)
{
  args->Check("");
  Shader* shader = reinterpret_cast<Shader*>(lua_newuserdata(args->script_system()->lua_state(), sizeof(Shader)));
  shader = new(shader) Shader();
  foundation::SharedPointer<ScriptableValue> script_shader = args->script_system()->GetFromStack(-1);
  SetMetaTable( script_shader, Name());
  args->AddReturn(script_shader);
}

sulphur::foundation::SharedPointer<sulphur::engine::ScriptableValue> 
sulphur::engine::ScriptShader::Copy(ScriptSystem* script_system, const Shader& shader)
{
  Shader* lua_shader = reinterpret_cast<Shader*>(lua_newuserdata(script_system->lua_state(), sizeof(Shader)));
  lua_shader = new(lua_shader) Shader(shader);
  foundation::SharedPointer<ScriptableValue> script_shader = script_system->GetFromStack(-1);
  SetMetaTable(script_shader, Name());
  return script_shader;
}
