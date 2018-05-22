#include "connection_manager.h"

#include "engine/networking/message_out.h"

#include <foundation/logging/logger.h>
#include <tools/networking/export.h>
#include <foundation/utils/timer.h>

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    ConnectionManager::ConnectionManager(bool as_editor)
      : as_editor_(as_editor)
    {
    }

    //-------------------------------------------------------------------------
    ConnectionManager::~ConnectionManager()
    {
      if (IsConnected())
      {
        Disconnect();
      }
    }

    //-------------------------------------------------------------------------
    bool ConnectionManager::RetrieveNextMessage(MessageID* out_id, MessagePayload* out_payload)
    {
      RetrieveMessages();

      if (unprocessed_messages.empty() == true)
      {
        return false;
      }
      else
      {
        *out_id = unprocessed_messages.back().id;
        *out_payload = unprocessed_messages.back().payload;
        unprocessed_messages.pop_back();

        return true;
      }
    }

    void ConnectionManager::RetrieveMessages()
    {
      MessageBuffer buffer;
      while (networking::editor::SNetRetrievePacket(
        reinterpret_cast<uint32_t*>(&buffer.id),
        reinterpret_cast<void*>(buffer.payload.data()),
        buffer.payload.used_bytes()) == true)
      {
        // Verify the message's identifier
        if (static_cast<uint32_t>(buffer.id) < kNumMessageTypes)
        {
          unprocessed_messages.push_back(buffer);
        }

        PS_LOG(Warning, "Unknown message received");
      }
    }

    //-------------------------------------------------------------------------
    void ConnectionManager::SendMessage(MessageID id, const MessagePayload& payload)
    {
      networking::editor::SNetSendData(static_cast<uint32_t>(id), payload.data(), payload.used_bytes());
    }

    //-------------------------------------------------------------------------
    void ConnectionManager::FlushMessages()
    {
      networking::editor::SNetFlushPackets();
    }

    //-------------------------------------------------------------------------
    bool ConnectionManager::Initialize(uint32_t port)
    {
      if (current_port != UINT32_MAX)
      {
        PS_LOG(Warning, "Attempted to initialize more than once");
        return false;
      }

      networking::editor::ErrorMessage err;
      if (as_editor_ == true)
      {
        err = networking::editor::SNetInitEditor();
      }
      else
      {
        err = networking::editor::SNetInitEngine(port);
      }

      if (err != networking::editor::ErrorMessage::kOk)
      {
        PS_LOG(Warning, "SNet initialization failure");
        return false;
      }

      current_port = port;

      return true;
    }

    //-------------------------------------------------------------------------
    bool ConnectionManager::Connect()
    {
      if (IsConnected() == true)
      {
        PS_LOG(Warning, "Attempted to connect whilst already connected");
        return false;
      }

      if (current_port == UINT32_MAX)
      {
        PS_LOG(Warning, "Attempted to connect before initialization");
        return false;
      }

      // Await connection
      foundation::Timer timer;
      timer.Start();
      while (IsConnected() == false)
      {
        // Check for incoming messages (needed to verify connection).
        // Any messages sent between establishment of the connection and confirmation
        // are appended to a temporary list (unprocessed_messages).
        RetrieveMessages();

        // In editor-mode only, we need to actively poll for a connection.
        if (as_editor_ == true)
        {
          networking::editor::SNetEditorConnect("localhost", current_port);
        }

        if (timer.GetElapsedSeconds() >= kTimeOutDelay)
        {
          PS_LOG(Debug, "Connection timed-out");
          return false;
        }
      }

      return true;
    }

    //-------------------------------------------------------------------------
    void ConnectionManager::Disconnect()
    {
      networking::editor::SNetDestroy();
    }

    //-------------------------------------------------------------------------
    bool ConnectionManager::IsConnected() const
    {
      return networking::editor::SNetIsConnected();
    }
  }
}