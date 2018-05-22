#include "foundation/logging/logger.h"
namespace sulphur
{
  namespace foundation
  {
    LoggingConfiguration LoggerBase::configuration_;

    bool LoggerBase::ChannelFiltered(LoggingChannel channel)
    {
      return (configuration_.IsChannelActive(channel) == false);
    }
    void LoggerBase::FilterChannel( LoggingChannel channel, bool filtered )
    {
      configuration_.SetChannelActive( channel, filtered );
    }
    const char* LoggerBase::ChannelName(LoggingChannel channel)
    {
      switch (channel)
      {
      case sulphur::foundation::LoggingChannel::kDefault:
        return "Default";
      case sulphur::foundation::LoggingChannel::kEngine:
        return "Engine";
      case sulphur::foundation::LoggingChannel::kGraphics:
        return "Graphics";
      case sulphur::foundation::LoggingChannel::kPhysics:
        return "Physics";
      case sulphur::foundation::LoggingChannel::kBuilder:
        return "Builder";
      case sulphur::foundation::LoggingChannel::kScripting:
        return "Scripting";
      case sulphur::foundation::LoggingChannel::kMemory:
        return "Memory";
      case sulphur::foundation::LoggingChannel::kNetworking:
        return "Networking";
      case sulphur::foundation::LoggingChannel::kNumChannels:
        return "NumChannels";
      default:
        return "Unknown";
      }
    }
  }
}