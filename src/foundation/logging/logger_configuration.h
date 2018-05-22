#pragma once
#include "foundation/containers/bitset.h"
namespace sulphur
{
  namespace foundation
  {
    /**
    * @enum sulphur::foundation::LoggingChannel
    * @brief Logging channel bitmask used to filter logging
    * @author Raymi Klingers
    */
    enum struct LoggingChannel
    {
      kDefault,
      kEngine,
      kGraphics,
      kPhysics,
      kBuilder,
      kScripting,
      kMemory,
      kNetworking,
      kNumChannels
    };

    /**
    * @class sulphur::foundation::LoggingConfiguration
    * @brief Contains the configuration of the loggers which currently is
    * what channels are active so that users can filter out everything they do
    * not need.
    * @author Raymi Klingers
    */
    class LoggingConfiguration
    {
    public:
      /**
      * @brief Sets the activeness of a channel. When inactive it won't log anything
      * @param[in] verbosity (sulphur::foundation::LoggingChannel) The chaneel
      * @param[in] active (bool) Active or inactive
      */
      void SetChannelActive( LoggingChannel channel, bool active );
      /**
      * @brief Gets whether a channel is active
      * @param[in] verbosity (sulphur::foundation::LoggingChannel) The chaneel
      * @return (bool) Is the current channel active?
      */
      bool IsChannelActive( LoggingChannel channel ) const;
    private:
      /**
      * @brief The bitset that contains whether channels are active or not.
      */
      eastl::bitset<static_cast< size_t >( LoggingChannel::kNumChannels )> disabled_channels_;
    };
  }
}