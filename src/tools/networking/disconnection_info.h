#pragma once

namespace sulphur
{
  namespace networking
  {
    /**
    * @enum sulphur::networking::DisconnectionInfo
    * @brief An enumerator that displays the type of disconnection
    * @author Joren Bolhuis
    */
    enum struct DisconnectionInfo : unsigned int
    {
      kDisconnected,
      kLostConnection,
      kKicked
    };
  }
}