#pragma once
#include "tools/networking/enet_fwd.h"
#include <foundation/containers/string.h>

namespace sulphur
{
  namespace networking
  {
    const size_t kMaxMessageSize = 128;//!< maximum size of the message
    const size_t kMaxPacketSize = 512;//!< maximum size of the payload
    /**
    * @class sulphur::networking::Message
    * @brief message description
    * @author Joren Bolhuis
    */
    struct Message
    {
      ENetPeer* peer = nullptr; //!< The peer we want to send to
      uint8_t data[kMaxMessageSize - 1] = {}; //!< The data we want to send
      size_t size = 0; //!< The size of the data
      bool reliable = true; //!< If this message should be reliable
    };

    /**
    * @class sulphur::networking::QueuedMessage
    * @brief queued message description
    * @remarks This struct has the message identifier included
    * @author Joren Bolhuis
    */
    struct QueuedMessage
    {
      ENetPeer* peer = nullptr; //!< The peer we want to send to
      uint8_t data[kMaxMessageSize] = {};  //!< The data we want to send
      size_t size = 0; //!< The size of the data
      bool reliable = true; //!< If the message should be reliable
    };

    /**
    * @class sulphur::networking::Packet
    * @brief The packet containing collected messages
    * @author Joren Bolhuis
    */
    struct Packet
    {
      ENetPeer* peer = nullptr; //!< The peer we want to send to
      uint8_t data[kMaxPacketSize] = {}; //!< The data we want to send
      size_t size = 0; //!< The size of the data
      bool reliable = true; //!< If the packet should be reliable
    };

    /**
    * @class sulphur::networking::PacketKey
    * @brief Key used in a map to differentiate the packets
    * @author Joren Bolhuis
    */
    struct PacketKey
    {
      /**
      * @brief Constructor
      * @param[in] p (ENetPeer*) The peer
      * @param[in] r (bool) Reliable or not
      */
      PacketKey(ENetPeer* p, bool r) : peer(p), reliable(r) {}
      ENetPeer* peer; //!< The peer the packet should be send to
      bool reliable; //!< If this packet should be reliable

      /**
      * @brief Check if packetkey is less
      * @param[in] other (const sulphur::networking::PacketKey&) The value to compare
      * @return (bool) Whether this one is less
      */
      bool operator<(const PacketKey& other)
      {
        if (peer == other.peer)
        {
          return reliable < other.reliable;
        }
        else
        {
          return peer < other.peer;
        }
      }

    };
    /**
    * @brief Check if packetkey is less
    * @param[in] a (const sulphur::networking::PacketKey&) The value to compare
    * @param[in] b (const sulphur::networking::PacketKey&) The value to compare
    * @return (bool) Whether a is less
    */
    inline bool operator<(const PacketKey& a, const PacketKey& b)
    {
      if (a.peer == b.peer)
      {
        return a.reliable < b.reliable;
      }
      else
      {
        return a.peer < b.peer;
      }
    }
  }
}