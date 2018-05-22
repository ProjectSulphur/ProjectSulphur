#pragma once

namespace sulphur
{
  namespace networking
  {
    /**
    * @enum sulphur::networking::NetworkValueOwner
    * @brief An enumerator of value types that are supported for syncing
    * @author Joren Bolhuis
    */
    enum struct NetworkValueOwner: uint8_t
    {
      kHost, //!< If the host should be the owner of this value
      kMe, //!< If I should be the owner of this value
      kOther //!< If someone else is the owner of this value
    };
  }
}