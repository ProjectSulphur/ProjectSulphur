#pragma once

/**
* @file foundation/logging/logger.h
*/
#include "foundation/logging/logger_configuration.h"
#include "foundation/containers/string.h"
#include <iostream>
#include <cassert>

/**
* @def PS_LOG_WITH(logger, verbosity, message, ...)
* @brief Log using the specified logger. Automatically adds function, line, and file information.
* @see sulphur::foundation::Logger::Log(Verbosity verbosity, const String& message, Args... args)
* @param[in] logger        The logger to log with
* @param[in] verbosity     The verbosity level used, this must match one of the values
*                          found in (sulphur::foundation::Verbosity) without the 'k' prefix.
* @param[in] message       The message to pass through to the logger
* @param[in] ...           Optional arguments to be passed through
* @author Kenneth Buijssen
*/
#define PS_LOG_WITH(logger, verbosity, message, ...) \
  logger::Log(sulphur::foundation::Verbosity::k##verbosity, message, \
              ##__VA_ARGS__, __FUNCTION__, __LINE__, __FILE__ );

/**
* @def PS_LOG(verbosity, message, ...)
* @brief Log using the default logger. Automatically adds function, line, and file information
* @see PS_LOG_WITH(logger, verbosity, message, ...)
* @see sulphur::foundation::Logger::Log(Verbosity verbosity, const String& message, Args... args)
* @author Kenneth Buijssen
*/
#define PS_LOG(verbosity, message, ...) \
  PS_LOG_WITH(sulphur::foundation::DefaultLogger, verbosity, message, ##__VA_ARGS__);

/**
* @def PS_LOG_IF_WITH(logger, condition, verbosity, message, ...)
* @brief Log using the specified logger if the condition has been met
* @see PS_LOG_WITH(logger, verbosity, message, ...)
* @see sulphur::foundation::Logger::Log(Verbosity verbosity, const String& message, Args... args)
* @param[in] condition If this condition evaluates to true the statement is logged
* @author Kenneth Buijssen
*/
#define PS_LOG_IF_WITH(logger, condition, verbosity, message, ...) \
  if (condition) { PS_LOG_WITH(logger, verbosity, message, ##__VA_ARGS__); }

/**
* @def PS_LOG_IF(condition, verbosity, message, ...)
* @brief Log using the default logger if the condition has been met
* @see PS_LOG_WITH(logger, verbosity, message, ...)
* @see sulphur::foundation::Logger::Log(Verbosity verbosity, const String& message, Args... args)
* @param[in] condition If this condition evaluates to true the statement is logged
* @author Kenneth Buijssen
*/
#define PS_LOG_IF(condition, verbosity, message, ...) \
  PS_LOG_IF_WITH(sulphur::foundation::DefaultLogger, condition, verbosity, message, ##__VA_ARGS__);

/**
* @def PS_LOG_ONCE_WITH(logger, verbosity, message, ...)
* @brief Log once using the specified logger over the lifetime of the engine
* @see PS_LOG_WITH(logger, verbosity, message, ...)
* @see sulphur::foundation::Logger::Log(Verbosity verbosity, const String& message, Args... args)
* @author Kenneth Buijssen
*/
#define PS_LOG_ONCE_WITH(logger, verbosity, message, ...)                      \
  {                                                                            \
    static bool first_log = true;                                              \
    PS_LOG_IF_WITH(logger, first_log == true, verbosity, message, ##__VA_ARGS__);\
    first_log = false;                                                         \
  }

/**
* @def PS_LOG_ONCE(verbosity, message, ...)
* @brief Log once using the default logger over the lifetime of the engine
* @see PS_LOG_WITH(logger, verbosity, message, ...)
* @see sulphur::foundation::Logger::Log(Verbosity verbosity, const String& message, Args... args)
* @author Kenneth Buijssen
*/
#define PS_LOG_ONCE(verbosity, message, ...) \
  PS_LOG_ONCE_WITH(sulphur::foundation::DefaultLogger, verbosity, message, ##__VA_ARGS__);

namespace sulphur
{
  namespace foundation
  {
    /**
     * @enum sulphur::foundation::Verbosity
     * @brief Different levels of verbosity accepted by the logger
     * @author Raymi Klingers
     */
    enum struct Verbosity
    {
      kDebug,   //!< Debug only info statements, only usefull when debugging
      kInfo,    //!< Information statements, only usefull when debugging
      kAssert,  //!< Logged asserts, only used on debug builds
      kWarning, //!< Potential issues, from which is automatically recovered
      kError,   //!< Errors, current operation cannot continue. Engine might recover
      kFatal    //!< Severe errors, the application is forced to exit
    };

    /**
     * @struct sulphur::foundation::DefaultFormat
     * @brief Printf style formattter
     * @author Raymi Klingers
     */
    struct DefaultFormat
    {
      /**
       * @brief Perform printf style formatting on a string
       * @tparam    Args (typename...) Variadic list of argument types
       * @param[in] message (const sulphur::foundation::String&) The printf style format string
       * @param[in] args (Args&&...) Arguments to insert into the formatted string
       * @return (sulphur::foundation::String) The formatted string
       * @author Raymi Klingers
       */
      template<typename ...Args>
      static String Format(const sulphur::foundation::String& message, Args&&... args)
      {
        char buff[1024];// Extent string class?
#ifdef PS_PS4
#pragma clang diagnostic push 
#pragma clang diagnostic ignored "-Wformat-security"
#endif
        sprintf_s(buff, message.c_str(), args...);// Make this typesafe?
#ifdef PS_PS4
#pragma clang diagnostic pop
#endif
        return buff;
      }
    };

    /**
    * @struct sulphur::foundation::FunctionLineAndFileFormat
    * @brief Printf style formattter which automatically appends line and file
    * @author Raymi Klingers
    */
    struct FunctionLineAndFileFormat
    {
      /**
      * @brief Perform printf style formatting on string,
      *        while also postfixing it with the file and line
      * @tparam    Args (typename...) Variadic list of argument types
      * @param[in] message (const sulphur::foundation::String&) The printf style format string
      * @param[in] args    (Args&&...) Arguments to insert into the formatted string
      * @return (sulphur::foundation::String) The formatted string
      * @author Raymi Klingers
      */
      template<typename ...Args>
      static String Format(const String& message, Args&&... args)
      {
        String formatted_message = message + " at %s - line #%i in file : \n%s\n";
        char buff[2048];// Extent string class?
        sprintf_s(buff, formatted_message.c_str(), args...);
        return buff;
      }
    };

    /**
     * @struct sulphur::foundation::DefaultTarget
     * @brief Logging target which prints directly to stdout
     * @author Raymi Klingers
     */
    struct DefaultTarget
    {
      /**
       * @brief Print the message directly stdout
       * @param[in] message (const sulphur::foundation::String&) The message to print
       */
      static void Print(const String& message)
      {
        std::cout << message.c_str() << std::endl;
      }
    };

    /**
    * @class sulphur::foundation::LoggerBase
    * @brief Base of the logger templates which contains the configuration as
    * that has to be shared between all loggers
    * @author Raymi Klingers
    */
    class LoggerBase
    {
    public:
      /**
      * @brief Sets whether the channel is filtered
      * @param[in] channel (sulphur::foundation::LoggingChannel) The channel
      * @param[in] filtered (bool) Filtered or not
      */
      static void FilterChannel(LoggingChannel channel, bool filtered);

      /**
      * @brief Gets whether the channel is filtered
      * @param[in] channel (sulphur::foundation::LoggingChannel) The channel
      * @return (bool) Whether it is filtered
      */
      static bool ChannelFiltered(LoggingChannel channel);

      /**
      * @brief Gets the name of the logging channel
      * @param[in] channel (sulphur::foundation::LoggingChannel) The channel
      * @return (const char*) The name of the channel
      */
      static const char* ChannelName(LoggingChannel channel);
    protected:
      static LoggingConfiguration configuration_;
    };


    /**
     * @class sulphur::foundation::Logger
     * @brief Logger used to format and send messages to various outputs
     * @tparam Channel (sulphur::foundation::LoggingChannel) The channels the logger accepts
     * @tparam Format (typename) The formatter to use for the logger
     * @tparam Target (typename) The output target for the logger
     * @author Raymi Klingers
     */
    template<LoggingChannel Channel, typename Format, typename Target>
    class Logger : public LoggerBase
    {
    protected:

    public:
      /**
       * @brief Format a message before sending it to the loggers target output
       * @tparam Args (typename...) Type of the additional arguments being passed to the logger
       * @param[in] verbosity (sulphur::foundation::Verbosity) The verbosity of
       *            the statement being logged
       * @param[in] message (const sulphur::foundation::String&) The printf format string to log
       * @param[in] args (Args...) The additonal arguments to be passed to the format string
       */
      template<typename ...Args>
      static void Log(Verbosity verbosity, const String& message, Args... args);
      
    };

    /**
     * @brief Default logger which prints all channels to stdout
     * @author Raymi Klingers
     */
     // Default logger prints to stdout
    using DefaultLogger = Logger<LoggingChannel::kDefault, DefaultFormat, DefaultTarget>;

    /**
    * @brief Logger which prints all channels to stdout while appending line and file information
    * @author Raymi Klingers
    */
    using LineAndFileLogger = Logger<LoggingChannel::kDefault,
      FunctionLineAndFileFormat,
      DefaultTarget>;

    template<LoggingChannel Channel, typename Format, typename Target>
    template<typename ... Args>
    inline void Logger<Channel, Format, Target>::Log(Verbosity verbosity, const String& message, Args... args)
    {
      if (LoggerBase::configuration_.IsChannelActive(Channel))
      {
        switch (verbosity)
        {
        case sulphur::foundation::Verbosity::kInfo:
        case sulphur::foundation::Verbosity::kWarning:
        case sulphur::foundation::Verbosity::kError:
          Target::Print(Format::Format(message, args...));
          break;
        case sulphur::foundation::Verbosity::kFatal:
          Target::Print(Format::Format(message, args...));
          exit(-1);
          break;
        case sulphur::foundation::Verbosity::kAssert:
          assert(verbosity != Verbosity::kAssert && verbosity != Verbosity::kFatal);
        case sulphur::foundation::Verbosity::kDebug:
#ifdef _DEBUG
          Target::Print(Format::Format(message, args...));
#endif
          break;
        default:
          Target::Print("Invalid verbosity level");
        }
      }
    }
  }
}