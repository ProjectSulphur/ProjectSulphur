#include "engine/scripting/wrappers/script_input.h"
#include "engine/scripting/script_utils.h"
#include "engine/scripting/scriptable_values/scriptable_args.h"

sulphur::engine::Input* sulphur::engine::ScriptInput::input_;

void sulphur::engine::ScriptInput::GetButtonFalling(ScriptableArgs* args)
{
  args->Check("S");
  const char * key = ScriptUtils::As<const char*>(args->GetArg(0));
  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), input_->IsButtonFalling(key)));
}

//--------------------------------------------------------------------------
void sulphur::engine::ScriptInput::GetButtonRising(ScriptableArgs* args)
{
  args->Check("S");
  const char * key = ScriptUtils::As<const char*>(args->GetArg(0));
  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), input_->IsButtonRising(key)));
}

//--------------------------------------------------------------------------
void sulphur::engine::ScriptInput::GetButtonDown(ScriptableArgs* args)
{
  args->Check("S");
  const char * key = ScriptUtils::As<const char*>(args->GetArg(0));
  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), input_->IsButtonDown(key)));
}

//--------------------------------------------------------------------------
void sulphur::engine::ScriptInput::GetMousePosition(ScriptableArgs* args)
{
  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), input_->GetMousePosition().x));
  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), input_->GetMousePosition().y));
}

//--------------------------------------------------------------------------
void sulphur::engine::ScriptInput::GetMouseOffset(ScriptableArgs* args)
{
  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), input_->GetMouseOffset().x));
  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), input_->GetMouseOffset().y));
}

//--------------------------------------------------------------------------
void sulphur::engine::ScriptInput::GetScrollOffset(ScriptableArgs* args)
{
  args->AddReturn(ScriptUtils::Instantiate(args->script_system(), input_->GetScrollOffset()));
}

//--------------------------------------------------------------------------
void sulphur::engine::ScriptInput::SetInput(Input& input)
{
  input_ = &input;
}
