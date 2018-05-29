#pragma once
#include "tools/networking/network_value.h"
#include "tools/networking/network_player.h"
#include <foundation/containers/vector.h>
#include <functional>

namespace sulphur
{
  namespace networking
  {
    /**
    * @class sulphur::networking::RPCHandle
    * @brief Handle that defines the specifications of an RPC in the system
    * @author Joren Bolhuis
    */
    struct RPCHandle
    {
      RPCHandle() {}
      uint16_t id; //!<The id of the RPC
      foundation::Vector<NetworkValueType> arguments; //!<The value types of the arguments in the function
    };

    /**
    * @class sulphur::networking::RPCData
    * @brief Data structure that contains information about an RPC
    * @author Joren Bolhuis
    */
    struct RPCData
    {
      eastl::function<void(RPCHandle, NetworkPlayerData, const foundation::Vector<NetworkValue>&)> function; //!<The function pointer
      foundation::Vector<NetworkValueType> arguments; //!<The value types of the arguments in the function
    };

    /**
    * @enum sulphur::networking::RPCMode : uint8_t
    * @brief Where the RPC should be send to
    * @author Joren Bolhuis
    */
    enum struct RPCMode : uint8_t
    {
     kAll, //!< Calls function locally and to all other clients, if executed on a client, the message will be send to the host and then distributed to all the other clients except yourself
     kOthers, //!< Sends a call to all clients except yourself, the message will be send to the host and then distributed to all the other clients
     kHost, //!< Calls function on server, if you are the server, it will be called locally
     kBufferedAll, //!<Call functionality just like kAll, but the RPC is buffered for clients that join later. Buffered rpcs are only supported on the host
     kBufferedOthers //!<Call functionality just like kOthers, but the RPC is buffered for clients that join later. Buffered rpcs are only supported on the host
    };
  }
}