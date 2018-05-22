#pragma once

namespace sulphur
{
  namespace networking
  {
    /**
    * @enum sulphur::networking::ConnectionSetup
    * @brief An enumerator to display our networking setup
    */
    enum struct ConnectionSetup : unsigned int
    {
      kNone, /**< setup is not initialised */
      kHost, /**< Setup is a host */
      kClient /**< Setup is a client */
    };

    /**
    * @enum sulphur::networking::ConnectionStatus
    * @brief An enumerator to display our connection status
    */
    enum struct ConnectionStatus : unsigned int
    {
      kConnecting, //!< connecting to host
      kConnected, //!< connected to a host
      kDisconnecting, //!< trying to disconnect from a host
      kDisconnected //!< Initial value, disconnected from a host
    };
  }
}