#pragma once
#include "tools/builder/base/flags_base.h"
#include <foundation/utils/template_util.h>
#include <foundation/containers/vector.h>

namespace sulphur 
{
  namespace builder 
  {
    /**
    *@struct sulphur::builder::Syntax
    *@brief structure describing the syntax of a command
    *@author Stan Pepels
    */
    struct Syntax
    {
      const char* key; //!< key value with which to access the command
      foundation::Vector<Flag*> valid_flags; //!< flags that can be used with this command
    };

    /**
    *@class sulphur::builder::CommandInput
    *@brief helper class for queryng the command input
    *@author Stan Pepels
    */
    class CommandInput
    {
      using Flags = foundation::Vector<Flag*>; //!< flags container
      using FlagArgs = foundation::Vector<foundation::String>; //!< arguments used with each flag

    public:
      /**
      *@brief constructor
      */
      CommandInput();

      /**
      *@brief construct input class with flags and arguments
      *@param[in] flags (sulphur::builder::CommandInput::Flags) flags parsed from a command line
      *@param[in] flag_args (sulphur::builder::CommandInput::FlagArgs) arguments belonging to the flag. if no argument is found the entry will be empty
      */
      CommandInput(Flags flags, FlagArgs flag_args);

      /**
      *@brief destructor
      */
      ~CommandInput();

      /**
      *@brief get argument that goes with flag of type T. use this to query a flag that can be used multiple times
      *@param[in] index (unsigned int) index specifying what the flag to query
      */
      template<typename T>
      const char* GetFlagArg(unsigned int index) const;

      /**
      *@brief get argument that goes with flag of type T. use this to query if a flag can only occur once
      */
      template<typename T>
      const char* GetFlagArg() const;

      /**
      *@brief check if flag of type T was used. use this to query a flag that can be used multiple times
      *@param[out] num_occurrences (unsigned int) number of times the flag was used. 
      */
      template<typename T>
      bool HasFlag(unsigned int& num_occurrences) const;

      /**
      *@brief check if flag of type T was used. use this to query if a flag can only occur once
      */
      template<typename T>
      bool HasFlag() const;

      /**
       * @brief Returns the number of flags of a certain type.
       * @tparam T Flag class to get the count from.
       * @return (unsigned int) The number of flags of type T.
       */
      template<typename T>
      unsigned int FlagCount() const;

    private:
      Flags flags_; //!< flags parsed from the command line used to call a command
      FlagArgs flag_args_;//!< arguments belonging to flags parsed from the command line used to call a command
    };

    /**
    *@class sulphur::builder::ICommand
    *@brief base class for commands. a command registered by the command system must be derived from this class
    *@author Stan Pepels
    */
    class ICommand
    {
    public:
      /**
      *@brief constructor
      *@param[in] key used to access this command
      */
      ICommand(const char* key);

      /**
      *@brief destructor
      */
      ~ICommand();

      /**
      *@brief get the command syntax
      */
      Syntax syntax();

      /**
      *@brief get the key passed in the constructor 
      */
      const char* key();

      /**
      *@brief executes the command.
      *@param[in] input (const sulphur::builder::CommandInput&) container holding input from the parsed command line.
      */
      virtual void Run(const CommandInput& input) = 0;

      /**
      *@brief check if the command line first input matches the command key value
      *@param[in] command_line (const char*) command recieved from console or command prompt
      */
      bool operator== (const char* command_line) const;

      /**
      *@brief get the description of the command
      *@remark if not implemented a default description will be returned
      */
      virtual const char* GetDescription() const;

    protected:
      /**
      *@brief specify what flags are valid for use with this command. each type in Args... will be valid.
      */
      template<typename... Args>
      bool SetValidFlags();

      /**
      *@brief set a flag to be allowed multiple times when using this command
      *@param[in] value (bool) value to set. By default flags can only occur once in a command
      */
      template<typename T>
      void AllowMultipleOccurances(bool value);

      /**
      *@brief set a flag to have a paramter passed with it
      *@param[in] value (bool) value to set. By default flags do not take parameters
      */
      template<typename T>
      void HasParameter(bool value);

      /**
       * @brief Set if the flag is optional.
       * @tparam T The type of the flag.
       * @param[in] value (bool) If the flag is optional.
       */
      template<typename T>
      void IsOptional(bool value);

    private:
      template<typename... Args>
      using  derived_from_flag = foundation::and_cond<eastl::is_base_of<Flag, Args>...>; //!< condition to check if all types in Args... are derived from Flag
      Syntax syntax_; //!< syntax of the command as specified in the constructor
    };

    template<typename ...Args>
    inline bool ICommand::SetValidFlags()
    {
      static_assert(derived_from_flag<Args...>::value,
        "invalid flag(s) usind in template parameter pack "
        "SetValidFlags<Args...>(int position). flags must derive from Flag class");
      while (syntax_.valid_flags.empty() == false)
      {
        delete syntax_.valid_flags.back();
        syntax_.valid_flags.pop_back();
      }
      syntax_.valid_flags = { new Args()... };
      return true;
    }

    template<typename T>
    inline void ICommand::AllowMultipleOccurances(bool value)
    {
      static_assert(std::is_base_of<Flag, T>::value,
        "invalid flag query, all flags must be derived from 'Flag'");
      T tmp = T();
      for (Flag* flag : syntax_.valid_flags)
      {
        if (tmp == flag)
        {
          flag->set_allow_multiple(value);
          return;
        }
      }
      // throw an error
    }

    template<typename T>
    inline void ICommand::HasParameter(bool value)
    {
      static_assert(std::is_base_of<Flag, T>::value,
        "invalid flag query, all flags must be derived from 'Flag'");
      T tmp = T();
      for (Flag* flag : syntax_.valid_flags)
      {
        if (*flag == tmp.GetKey())
        {
          flag->set_has_argument(value);
          return;
        }
      }
      // throw an error
    }

    template<typename T>
    inline void ICommand::IsOptional(bool value)
    {
      static_assert(std::is_base_of<Flag, T>::value,
        "invalid flag query, all flags must be derived from 'Flag'");
      T tmp = T();
      for (Flag* flag : syntax_.valid_flags)
      {
        if (*flag == tmp.GetKey())
        {
          flag->set_optional(value);
          return;
        }
      }
      // throw an error
    }

    template<typename T>
    inline const char* CommandInput::GetFlagArg(unsigned int index) const
    {
      unsigned int count = 0; 
      unsigned int curr_index = 0;
      T tmp = T();
      for (Flag* flag : flags_)
      {
        if (*flag == tmp.GetKey())
        {
          if (count == index)
          {
            return flag_args_[curr_index].c_str();
          }
          ++count;
        }
        ++curr_index;
      }
      return "";
    }

    template<typename T>
    inline const char* CommandInput::GetFlagArg() const
    {
      unsigned int curr_index = 0;
      T tmp = T();
      for (Flag* flag : flags_)
      {
        if (*flag == tmp.GetKey())
        {
          return flag_args_[curr_index].c_str();
        }
        ++curr_index;
      }
      return "";
    }

    template<typename T>
    inline bool CommandInput::HasFlag(unsigned int& num_occurrences) const
    {
      static_assert(std::is_base_of<Flag, T>::value,
        "invalid flag query, all flags must be derived from 'Flag'");

      num_occurrences = FlagCount<T>();
      return num_occurrences != 0;
    }

    template<typename T>
    unsigned int CommandInput::FlagCount() const
    {
      static_assert(std::is_base_of<Flag, T>::value,
        "invalid flag query, all flags must be derived from 'Flag'");
      unsigned int count = 0;
      T tmp = T();
      for (Flag* flag : flags_)
      {
        if (*flag == tmp.GetKey())
        {
          ++count;
        }
      }
      return count;
    }

    template<typename T>
    inline bool CommandInput::HasFlag() const
    {
      static_assert(std::is_base_of<Flag, T>::value,
        "invalid flag query, all flags must be derived from 'Flag'");

      T tmp = T();
      for (Flag* flag : flags_)
      {
        if (*flag == tmp.GetKey())
        {
          return true;
        }
      }
      return false;
    }
  }
}