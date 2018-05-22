#include "engine/scripting/wrappers/script_shader_program.h"
#include "engine/scripting/script_system.h"
#include "engine/assets/shader.h"
#include "engine/scripting/scriptable_values/scriptable_args.h"

void sulphur::engine::ScriptShaderProgram::New(ScriptableArgs* args)
{
  args->Check("");
  ShaderProgram* shader_program = reinterpret_cast<ShaderProgram*>(lua_newuserdata(args->script_system()->lua_state(), sizeof(ShaderProgram)));
  shader_program = new(shader_program) ShaderProgram();
  foundation::SharedPointer<ScriptableValue> script_shader_program = args->script_system()->GetFromStack(-1);
  SetMetaTable(script_shader_program, Name());
  args->AddReturn(script_shader_program);
}

sulphur::foundation::SharedPointer<sulphur::engine::ScriptableValue> 
sulphur::engine::ScriptShaderProgram::Copy(ScriptSystem* script_system, const ShaderProgram& shader)
{
  ShaderProgram* lua_shader_program = reinterpret_cast<ShaderProgram*>(lua_newuserdata(script_system->lua_state(), sizeof(ShaderProgram)));
  lua_shader_program = new(lua_shader_program) ShaderProgram(shader);
  foundation::SharedPointer<ScriptableValue> script_shader_program = script_system->GetFromStack(-1);
  SetMetaTable(script_shader_program, Name());
  return script_shader_program;
}
