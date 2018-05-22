#pragma once
#include "tools/networking/enet_fwd.h"
#include <foundation/containers/string.h>

namespace sulphur
{
  namespace networking
  {
    const size_t kNameLength = 64; //!<The max size of a name
    const size_t kIpLength = 64; //!<The max size of an ip

    /**
    * @struct sulphur::networking::NetworkPlayerData
    * @brief Struct that holds player data that is exposed to the outside world
    * @author Joren Bolhuis
    */
    struct NetworkPlayerData
    {
      NetworkPlayerData() : id(0) {};
      char name[kNameLength]; //!<The name
      uint8_t id; //!<The id
    };

    /**
    * @class sulphur::networking::NetworkPlayer
    * @brief Class that holds data about a networked player
    * @author Joren Bolhuis
    */
    class NetworkPlayer
    {
    public:
      /**
      * @brief Construction of a networkplayer
      * @param[in] id (unsigned int) The id of the player
      * @param[in] peer (ENetPeer*) The peer of the player, if we are a client,
      we dont have information about peers to other clients other then the host
      * @param[in] name (const char*) The player name
      * @param[in] ip (const char*) The ip of the player, only available if we are a host
      * @param[in[ last_heartbeat (float) The time of the heartbeat on connection
      */
      NetworkPlayer(uint8_t id, ENetPeer* peer, const char* name,
        const char* ip, float last_heartbeat);
      /**
      * @brief Destruction of a networkplayer
      */
      ~NetworkPlayer() {};
      /**
      * @brief Set the player name
      * @param[in] name (const char*) The name we want to use
      */
      void set_name(const char* name);
      /**
      * @brief Set the player id
      * @param[in] id (unsigned int) The id we want to use
      */
      void set_id(uint8_t id);
      /**
      * @brief Set the player peer
      * @param[in] peer (ENetPeer*) The peer we want to use
      */
      void set_peer(ENetPeer* peer);
      /**
      * @brief Set the last heartbeat
      * @param[in] heartbeat (float) The last heartbeat we want to set
      */
      void set_last_heartbeat(float heartbeat);
      /**
      * @brief Set the last time we sent a heartbeat
      * @param[in] heartbeat (float) The last heartbeat we want to set
      */
      void set_last_heartbeat_sent(float heartbeat);
      /**
      * @brief Retrieve the peer
      * @return (ENetPeer*) the peer
      */
      ENetPeer* peer() const;
      /**
      * @brief Retrieve the name
      * @return (const char*) the name
      */
      const char* name() const;
      /**
      * @brief Retrieve the ip
      * @return (const char*) the ip
      */
      const char* ip() const;
      /**
      * @brief Retrieve the id
      * @return (unsigned int) the id
      */
      uint8_t id() const;
      /**
      * @brief Retrieve the last heartbeat
      * @return (float) the heartbeat
      */
      float last_heartbeat() const;
      /**
      * @brief Retrieve the last heartbeat sent
      * @return (float) the heartbeat
      */
      float last_heartbeat_sent() const;
    private:
      ENetPeer * peer_; //!<The peer
      char name_[kNameLength]; //!<The name
      char ip_[kIpLength]; //!<The ip
      uint8_t id_; //!<The id
      float last_heartbeat_; //!<The last time we recieved a heartbeat
      float last_heartbeat_sent_; //!<The last time we sent a heartbeat
    };
  }
}