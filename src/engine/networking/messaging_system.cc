#include "messaging_system.h"

#include "engine/networking/message_listener.h"

#include <foundation/logging/logger.h>
#include <tools/networking/export.h>

#include <algorithm>

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    MessagingSystem::MessagingSystem() :
      IServiceSystem("MessagingSystem")
    {
    }

    //-------------------------------------------------------------------------
    void MessagingSystem::OnInitialize(Application&, foundation::JobGraph&)
    {
#ifdef PS_EDITOR
      if (connection_.Initialize() == false)
      {
        // @note unsure if we should really assert here...
        PS_LOG(Assert, "Failed to initialize connection");
      }
      
      do
      {
        // wait for connection...
      } while (connection_.Connect() == false);
#endif
    }

    //-------------------------------------------------------------------------
    void MessagingSystem::OnTerminate()
    {
    }

    //-------------------------------------------------------------------------
    void MessagingSystem::ReceiveMessages()
    {
      // Process network-sent messages
      MessageID id;
      MessagePayload payload;
      while (connection_.RetrieveNextMessage(&id, &payload))
      {
        subscriptions_.NotifySubscribers(id, payload);
      }
      // Process locally-sent messages
      while (incoming_messages_.empty() == false)
      {
        subscriptions_.NotifySubscribers(
          incoming_messages_.back().id,
          incoming_messages_.back().payload);

        incoming_messages_.pop_back();
      }
    }

    //-------------------------------------------------------------------------
    void MessagingSystem::DispatchMessages()
    {
      if(connection_.IsConnected() == false)
      {
        return;
      }
      
      using MessageIt = foundation::Vector<MessageBuffer>::iterator;
      for (MessageIt it = outgoing_messages_.begin(); it != outgoing_messages_.end(); ++it)
      {
        connection_.SendMessage(it->id, it->payload);
      }
      outgoing_messages_.clear();

      connection_.FlushMessages();
    }

    //-------------------------------------------------------------------------
    void MessagingSystem::SendToEditor(MessageID id, const MessagePayload& payload)
    {
      MessageBuffer buffer;
      buffer.id = id;
      buffer.payload = payload;
      outgoing_messages_.emplace_back(buffer);
    }

    //-------------------------------------------------------------------------
    void MessagingSystem::SendLocal(MessageID id, const MessagePayload& payload)
    {
      MessageBuffer buffer;
      buffer.id = id;
      buffer.payload = payload;
      incoming_messages_.emplace_back(buffer);
    }
  }
}