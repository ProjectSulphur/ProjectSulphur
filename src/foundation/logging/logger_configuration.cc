#include "logger_configuration.h"
namespace sulphur
{
  namespace foundation
  {
    void LoggingConfiguration::SetChannelActive( LoggingChannel channel, bool active )
    {
      disabled_channels_.set( static_cast< size_t >( channel ), active );
    }
    bool LoggingConfiguration::IsChannelActive( LoggingChannel channel ) const
    {
      return ( disabled_channels_.test( static_cast< size_t >( channel ) ) == false );
    }
  }
}