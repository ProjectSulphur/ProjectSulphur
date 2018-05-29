#pragma once
#include <foundation/logging/logger.h>

#define PS_LOG_BUILDER(verbosity, message, ...) \
  BuilderLogger::Log(sulphur::foundation::Verbosity::k##verbosity, message, \
              ##__VA_ARGS__);

namespace sulphur 
{
  namespace builder 
  {
    /**
    * @struct sulphur::builder::BuilderTarget
    * @brief Logging target for builder.
    * @author Timo van Hees
    */
    struct BuilderTarget
    {
      /**
      * @brief Print the message directly stdout
      * @param[in] message (const sulphur::foundation::String&) The message to print
      */
      static void Print(const foundation::String& message)
      {
        std::cout << message.c_str() << std::endl;
      }
    };

    /**
    * @struct sulphur::builder::BuilderFormat
    * @brief Printf style formatter for builder
    * @author Timo van Hees
    */
    struct BuilderFormat
    {
      /**
      * @brief Perform printf style formatting on a string
      * @tparam    Args (typename...) Parameter pack of argument types
      * @param[in] verbosity (sulphur::foundation::Verbosity) The verbosity
      * @param[in] message (const sulphur::foundation::String&) The printf style format string
      * @param[in] args (Args&&...) Arguments to insert into the formatted string
      * @return (sulphur::foundation::String) The formatted string
      */
      template<typename ...Args>
      static foundation::String Format(foundation::Verbosity verbosity, 
        const foundation::String& message, Args&&... args)
      {
        foundation::String format_string = "";
        switch (verbosity) 
        { 
        case foundation::Verbosity::kDebug: 
          format_string = "[Debug] "; 
          break;
        case foundation::Verbosity::kInfo:
          format_string = "[Info] ";
          break;
        case foundation::Verbosity::kAssert:
          format_string = "[Assert] ";
          break;
        case foundation::Verbosity::kWarning:
          format_string = "[Warning] ";
          break;
        case foundation::Verbosity::kError:
          format_string = "[Error] ";
          break;
        case foundation::Verbosity::kFatal:
          format_string = "[Fatal] ";
          break;
        default: ;
        }

        format_string += message;

        char buff[1024];
        sprintf_s(buff, format_string.c_str(), args...);
        return buff;
      }
    };

    using BuilderLogger = foundation::Logger<
      foundation::LoggingChannel::kDefault, BuilderFormat, BuilderTarget>;
  }
}
