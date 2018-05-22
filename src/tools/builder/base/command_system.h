#pragma once
#include "tools/builder/base/commands_base.h"
#include <foundation/utils/type_definitions.h>

namespace sulphur 
{
  namespace builder 
  {
    /**
    * @brief error return codes for command system
    */
    enum struct CommandErr : uint16_t
    {
      kInvalidSyntax, //!< wrong syntax used. gets thrown when unique flag is used multiple times.
      kInvalidFlag, //!< wrong flag used. gets thrown if a given flag is not supported with a command.
      kMissingFlag,
      kUnknownCommand,//!< command not found or invalid command input
      kExitCommand, //!< exit command called aplication closes
      kNoError //!< everything okay
    };

    /**
    *@class sulphur::builder::CommandSystem
    *@brief class that manages command line input and registering of commands.  
    *@author Stan Pepels
    */
    class CommandSystem
    {
    public:
      /**
       * @brief Function pointer to a function that handles error messages.
       */
      using ErrorHandler = void(*)(const CommandErr& message);

      /**
      * @brief constructor
      */
      CommandSystem();

      /**
      * @brief destructor
      */
      ~CommandSystem();

      /**
      *@brief set error handler for handling error codes.
      *@param[in] handler (sulphur::builder::ErrorHandler) the handler.
      */
      void set_error_handler(ErrorHandler handler);

      /**
      *@brief register command of type T with arguments Args...
      *@param[in] key (const char*) key to access the command with.
      *@param[in] args <Args...> parameter pack to pass in the constructor of command of type T.
      *@return (unsigned int) unique id used to unregister command. user is responsible for keeping it around.
      */
      template<typename T, typename... Args>
      uint RegisterCommand(const char* key, Args... args);

      /**
      *@brief unregister a command from the command list.
      *@param[in] command_id (unsigned int) command to unregister. value is the id returned when this command was registered.
      *@return (bool) true if removed false if command does not exist.
      *@see sulphur::builder::CommandSystem::RegisterCommand
      */
      bool UnregisterCommand(unsigned int command_id);

      /**
      *@brief execute the next line inputted in the console.
      */
      CommandErr ExecuteNext();

      /**
      *@brief execute single command line.
      */
      CommandErr ExecuteCommandLine(const char* command_line);

      /**
      *@brief print the descriptions of all registered commands.
      */
      void PrintDescriptions() const;
    private:

      ErrorHandler error_handler_;
      foundation::Vector<ICommand*> registered_commands_;
      foundation::Vector<unsigned int> command_ids_;

      unsigned int num_commands_;

      /**
      *@brief handle errors using either the default handler or the handler givin by sulphur::builder::CommandSystem::set_error_handler
      *@param[in] error (sulphur::builder::CommandErr) error to be processed
      *@see sulphur::builder::CommandSystem::set_error_handler
      */
      void HandleError(const CommandErr& error);

      /**
      *@brief validate syntax and parse command line to distinguish flags and parameters
      *@param[in] command_line (const char*) input to be parsed
      *@param[in] syntax (sulphur::builder::Syntax&) struct describing the syntax for the command
      *@param[out] cmd_input (sulphur::builder::CommandInput*) struct for querying the parsed flags and arguments. This gets passed to the command's sulphur::builder::Command::Run function
      *@see sulphur::builder::Command::Run
      */
      CommandErr ValidateSyntax(const char* command_line, Syntax& syntax, CommandInput* cmd_input);

      /**
      *@brief default error handler
      *@param[in] message (const sulphur::builder::CommandErr&) error message to be handled
      */
      static void DefaultErrorHandler(const CommandErr& message);

      const foundation::String help_command; //!< string for printing the description of all available commands
    };

    template<typename T, typename... Args>
    inline uint CommandSystem::RegisterCommand(const char* key, Args... args)
    {
      static_assert(std::is_base_of<ICommand, T>::value,
                    "command is not derived from Command class");

      registered_commands_.push_back(new T(key, args...));
      command_ids_.push_back(num_commands_);
      ++num_commands_;
      return command_ids_.back();
    }
  }
}