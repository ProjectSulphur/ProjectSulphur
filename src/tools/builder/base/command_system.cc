#include "tools/builder/base/command_system.h"
#include "tools/builder/shared/application.h"
#include "tools/builder/base/logger.h"
#include <eastl/algorithm.h>
#include <iostream>

namespace sulphur 
{
  namespace builder 
  {
    //-----------------------------------------------------------------------------------------------
    CommandSystem::CommandSystem(ModelPipeline* model_pipeline,
      MeshPipeline* mesh_pipeline, MaterialPipeline* material_pipeline,
      TexturePipeline* texture_pipeline, ShaderPipeline* shader_pipeline,
      SkeletonPipeline* skeleton_pipeline, AnimationPipeline* animation_pipeline,
      ScriptPipeline* script_pipeline_, AudioPipeline* audio_pipeline_, 
      SceneLoader* scene_loader)
      : help_command("--help"), 
    model_pipeline_(model_pipeline), 
    mesh_pipeline_(mesh_pipeline),
    material_pipeline_(material_pipeline), 
    texture_pipeline_(texture_pipeline), 
    shader_pipeline_(shader_pipeline), 
    skeleton_pipeline_(skeleton_pipeline), 
    animation_pipeline_(animation_pipeline),
    script_pipeline_(script_pipeline_),
    audio_pipeline_(audio_pipeline_),
    scene_loader_(scene_loader)
    {
      error_handler_ = DefaultErrorHandler;
    }

    //-----------------------------------------------------------------------------------------------
    CommandSystem::~CommandSystem()
    {
      while (registered_commands_.size() != 0)
      {
        delete registered_commands_.back();
        registered_commands_.pop_back();
      }
    }

    //-----------------------------------------------------------------------------------------------
    bool CommandSystem::UnregisterCommand(unsigned int command_id)
    {
      for (int i = 0; i < command_ids_.size(); ++i)
      {
        if (command_id == command_ids_[i])
        {
          delete registered_commands_[i];
          command_ids_.erase(command_ids_.begin() + i);
          registered_commands_.erase(registered_commands_.begin() + i);
          break;
        }
      }
      return true;
    }

    //-----------------------------------------------------------------------------------------------
    void CommandSystem::set_error_handler(ErrorHandler handler)
    {
      error_handler_ = handler;
    }

    //-----------------------------------------------------------------------------------------------
    void CommandSystem::HandleError(const CommandErr& error)
    {
      error_handler_(error);
    }

    //-----------------------------------------------------------------------------------------------
    CommandErr CommandSystem::ExecuteNext()
    {
      CommandErr result = CommandErr::kNoError;

      char tmp[254];
      std::cout << "command: ";
      std::cin.getline(tmp, 254);

      foundation::String command_line = foundation::String(tmp);
      command_line.erase(eastl::remove_if(command_line.begin(), command_line.end(), [](char c) {return c == '\"'; }), command_line.end());

      if (command_line == "--exit")
      {
        return CommandErr::kExitCommand;
      }

      if (command_line == "--help")
      {
        PrintDescriptions();
        return CommandErr::kNoError;
      }

      for (ICommand* command : registered_commands_)
      {
        if (*command == command_line.c_str())
        {
          Syntax syntax = command->syntax();
          CommandInput input;

          result = ValidateSyntax(command_line.c_str(), syntax, &input);
          HandleError(result);

          if (result != CommandErr::kNoError)
          {
            return result;
          }

          command->Run(input);
          return CommandErr::kNoError;
        }
      }
      result = CommandErr::kUnknownCommand;
      HandleError(result);
      return CommandErr::kUnknownCommand;
    }

    CommandErr CommandSystem::ExecuteCommandLine(const char* command_line)
    {
      foundation::String cmd = command_line;
      cmd.erase(eastl::remove_if(cmd.begin(), cmd.end(), [](char c) {return c == '\"'; }), cmd.end());

      if (cmd == help_command)
      {
        PrintDescriptions();
        return CommandErr::kNoError;
      }

      CommandErr result;
      for (ICommand* command : registered_commands_)
      {
        if (*command == cmd.c_str())
        {
          Syntax syntax = command->syntax();
          CommandInput input;

          result = ValidateSyntax(cmd.c_str(), syntax, &input);
          HandleError(result);

          if (result != CommandErr::kNoError)
          {
            return result;
          }

          command->Run(input);
          return CommandErr::kNoError;
        }
      }
      result = CommandErr::kUnknownCommand;
      HandleError(result);
      return CommandErr::kUnknownCommand;
    }

    void CommandSystem::PrintDescriptions() const
    {
      std::cout << "output dir: " 
                << Application::out_dir().GetAbsolutePath().GetString().c_str()
                << "\n";

      std::cout << "package output dir: " 
                << Application::package_dir().GetAbsolutePath().GetString().c_str()
                << "\n";

      std::cout << std::endl;

      for (ICommand* command : registered_commands_)
      {
        std::cout << command->key() << "\n";
        std::cout << command->GetDescription() << "\n";
      }
    }

    //-----------------------------------------------------------------------------------------------
    CommandErr CommandSystem::ValidateSyntax(const char* command_line,
                                             Syntax& syntax,
                                             CommandInput* cmd_input)
    {
      foundation::String input = foundation::String(command_line);
      input.erase(0, foundation::String(syntax.key).length());

      if (input.length() == 0 && syntax.valid_flags.size() == 0)
      {
        return CommandErr::kNoError;
      }

      foundation::String args = foundation::String("");
      foundation::Vector<Flag*> found_flags;
      foundation::Vector<foundation::String> flag_args;
      foundation::Vector<foundation::String> command_args;
      
      while (input.find(" -") != input.npos)
      {
        bool valid_flag = false;

        // remove the "-" symbol and find the end of the flag
        input.erase(0, input.find("-") + 1);
        size_t flag_end = input.find(" ");
        foundation::String key = foundation::String();

        key = input.substr(0, flag_end);
        Flag* flag = nullptr;

        // check if this flag is valid for this syntax
        for (Flag* entry : syntax.valid_flags)
        {
          if (*entry == key)
          {
            flag = entry;
            valid_flag = true;
            break;
          }
        }

        if (valid_flag == false)
        {
          continue;
        }

        // check if multiple flags ar allowed
        if (flag->allow_multiple() == false)
        {
          for (Flag* entry : found_flags)
          {
            if (flag == entry)
            {
              valid_flag = false;
              break;
            }
          }
        }
        if (valid_flag == false)
        {
          continue;
        }

        // check if there is an argument that belongs to this flag
        if (flag->has_argument() == true)
        {
          size_t arg_start = input.find_first_not_of(" ", flag_end);
          size_t arg_end = arg_start;
          bool found = false;
          while (found == false)
          {
            arg_end = input.find_first_of("-", arg_end);
            if (arg_end != input.npos && isspace(input[arg_end - 1]) == false)
            {
              arg_end++;
              continue;
            }
            else
            {
              found = true;
            }
          }

          if (arg_end - arg_start == 0 || arg_start == input.npos)
          {
            flag_args.push_back("");
          }
          else
          {
            foundation::String arg;
            arg = input.substr(arg_start, arg_end - arg_start); 
            arg_end = arg.find_last_not_of(" ", arg.npos);
            ++arg_end >= arg.length() ? 
              arg = arg.substr(0, arg.npos) : arg = arg.substr(0, arg_end);
            flag_args.push_back(arg);
            flag_end = arg_end;
          }
        }
        else
        {
          flag_args.push_back("");
        }
        found_flags.push_back(flag);
      }

      for (Flag* flag : syntax.valid_flags)
      {
        if (flag->optional() == false)
        {
          eastl::vector<Flag*>::iterator i = 
            eastl::find_if(found_flags.begin(), found_flags.end(), [&flag](Flag* in)
          {
            return flag->GetKey() == in->GetKey();
          });

          if (i == found_flags.end())
          {
            return CommandErr::kMissingFlag;
          }
        }
      }

      *cmd_input = CommandInput(found_flags, flag_args);
      return CommandErr::kNoError;
    }

    //-----------------------------------------------------------------------------------------------
    void CommandSystem::DefaultErrorHandler(const CommandErr& message)
    {
      switch (message)
      {
      case CommandErr::kInvalidFlag:
        PS_LOG_BUILDER(Error, "invalid flag used in command");
        break;
      case CommandErr::kInvalidSyntax:
        PS_LOG_BUILDER(Error, "invalid command syntax");
        break;
      case CommandErr::kUnknownCommand:
        PS_LOG_BUILDER(Error, "unknown command");
        break;
      case CommandErr::kMissingFlag:
        PS_LOG_BUILDER(Error, "missing mandatory flag");
        break;
      default: ;
      }
    }
  }
}
