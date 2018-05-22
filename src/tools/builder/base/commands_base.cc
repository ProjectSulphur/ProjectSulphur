#include "tools/builder/base/commands_base.h"

namespace sulphur 
{
  namespace builder 
  {
    //-----------------------------------------------------------------------------------------------
    ICommand::ICommand(const char* key)
    {
      syntax_.key = key;
    }

    //-----------------------------------------------------------------------------------------------
    ICommand::~ICommand()
    {
      while (syntax_.valid_flags.size() != 0)
      {
        delete syntax_.valid_flags.back();
        syntax_.valid_flags.pop_back();
      }
    }

    //-----------------------------------------------------------------------------------------------
    bool ICommand::operator==(const char* command_line) const
    {
      foundation::String command = foundation::String(command_line);

      if (command.find_first_of(' ') == command.npos)
      {
        return command == syntax_.key;
      }

      command.erase(command.find_first_of(' '));
      return command == syntax_.key;
    }

    //-----------------------------------------------------------------------------------------------
    const char* ICommand::GetDescription() const
    {
      return "No description available \n";
    }

    //-----------------------------------------------------------------------------------------------
    Syntax ICommand::syntax()
    {
      return syntax_;
    }

    const char* ICommand::key()
    {
      return syntax_.key;
    }

    //-----------------------------------------------------------------------------------------------
    CommandInput::CommandInput()
    {
    }

    //-----------------------------------------------------------------------------------------------
    CommandInput::CommandInput(Flags flags, FlagArgs flag_args) :
      flags_(flags),
      flag_args_(flag_args)
    {
    }

    //-----------------------------------------------------------------------------------------------
    CommandInput::~CommandInput()
    {
    }
  }
}