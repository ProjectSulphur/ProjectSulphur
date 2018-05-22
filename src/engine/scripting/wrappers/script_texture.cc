#include "engine/scripting/wrappers/script_texture.h"
#include "engine/scripting/script_system.h"
#include "engine/scripting/script_utils.h"
#include "engine/assets/texture.h"
#include "engine/scripting/scriptable_values/scriptable_args.h"

void sulphur::engine::ScriptTexture::New(ScriptableArgs* args)
{
  args->Check("");
  Texture* texture = reinterpret_cast<Texture*>(lua_newuserdata(args->script_system()->lua_state(), sizeof(Texture)));
  texture = new(texture) Texture();
  foundation::SharedPointer<ScriptableValue> script_texture = args->script_system()->GetFromStack(-1);
  SetMetaTable(script_texture, Name());
  args->AddReturn(script_texture);
}

sulphur::foundation::SharedPointer<sulphur::engine::ScriptableValue> 
sulphur::engine::ScriptTexture::Copy(ScriptSystem* script_system, const Texture& material)
{
  Texture* lua_texture = reinterpret_cast<Texture*>(lua_newuserdata(script_system->lua_state(), sizeof(Texture)));
  lua_texture = new(lua_texture) Texture(material);
  foundation::SharedPointer<ScriptableValue> script_texture = script_system->GetFromStack(-1);
  SetMetaTable(script_texture, Name());
  return script_texture;
}
