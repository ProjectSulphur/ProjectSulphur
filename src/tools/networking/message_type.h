#pragma once

namespace sulphur
{
  namespace networking
  {
    /**
    * @enum sulphur::networking::PacketDelivery
    * @brief An enumerator to pick the type of delivering our packet
    */
    enum struct PacketDelivery : unsigned int
    {
      kSend, //!< regular send to peer
      kBroadcast //!< broadcast to all clients
    };

    /**
    * @enum sulphur::networking::MessageDataType
    * @brief An enumerator to identify our message
    */
    enum struct MessageDataType : uint8_t
    {
      kName, //!< name identifier
      kUpdateName, //!< name identifier
      kPlayerID, //!< player id identifier
      kPlayerAdd, //!< player add identifier
      kPlayerDelete, //!< player delete identifier
      kHeartBeat, //!< heartbeat identifier
      kRPC, //!< rpc identifier
      kValueSync, //!< valuesync identifier
      kValueSyncValidation //!< valuesync validation identifier
    };
  }
}