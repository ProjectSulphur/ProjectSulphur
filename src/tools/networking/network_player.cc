#include "tools/networking/network_player.h"

namespace sulphur
{
  namespace networking
  {
    //-------------------------------------------------------------------------
    NetworkPlayer::NetworkPlayer(uint8_t id, ENetPeer* peer, const char* name, 
      const char* ip, float last_heartbeat) :
      peer_(peer),
      id_(id),
      last_heartbeat_(last_heartbeat),
      last_heartbeat_sent_(last_heartbeat),
      round_trip_time_(0)
    {
      memset(name_, '\0', kNameLength);
      memset(ip_, '\0', kIpLength);
      strcpy_s(name_, kNameLength, name);
      strcpy_s(ip_, kIpLength, ip);
    }

    //-------------------------------------------------------------------------
    void NetworkPlayer::set_name(const char* name)
    {
      memset(name_, '\0', kNameLength);
      strcpy_s(name_, kNameLength, name);
    }

    //-------------------------------------------------------------------------
    void NetworkPlayer::set_id(uint8_t id)
    {
      id_ = id;
    }

    //-------------------------------------------------------------------------
    void NetworkPlayer::set_peer(ENetPeer * peer)
    {
      peer_ = peer;
    }

    //-------------------------------------------------------------------------
    void NetworkPlayer::set_last_heartbeat(float heartbeat)
    {
      last_heartbeat_ = heartbeat;
    }

    //-------------------------------------------------------------------------
    void NetworkPlayer::set_last_heartbeat_sent(float heartbeat)
    {
      last_heartbeat_sent_ = heartbeat;
    }

    //-------------------------------------------------------------------------
    void NetworkPlayer::set_round_trip_time(unsigned int rtt)
    {
      round_trip_time_ = rtt;
    }

    //-------------------------------------------------------------------------
    ENetPeer * NetworkPlayer::peer() const
    {
      return peer_;
    }

    //-------------------------------------------------------------------------
    const char* NetworkPlayer::name() const
    {
      return name_;
    }

    //-------------------------------------------------------------------------
    const char* NetworkPlayer::ip() const
    {
      return ip_;
    }

    //-------------------------------------------------------------------------
    uint8_t NetworkPlayer::id() const
    {
      return id_;
    }

    //-------------------------------------------------------------------------
    float NetworkPlayer::last_heartbeat() const
    {
      return last_heartbeat_;
    }

    //-------------------------------------------------------------------------
    float NetworkPlayer::last_heartbeat_sent() const
    {
      return last_heartbeat_sent_;
    }
    unsigned int NetworkPlayer::round_trip_time() const
    {
      return round_trip_time_;
    }
  }
}