#include "messaging_system.h"

#include "engine/networking/message_listener.h"
#include "engine/networking/messages.h"
#include <foundation/logging/logger.h>
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
    }

    //-------------------------------------------------------------------------
    void MessagingSystem::OnTerminate()
    {
    }

    //-------------------------------------------------------------------------
    void MessagingSystem::ReceiveMessages()
    {
      // Process locally-sent messages
      while (incoming_messages_.empty() == false)
      {
        NotifySubscribers(
          incoming_messages_.back().id,
          incoming_messages_.back().payload);

        incoming_messages_.pop_back();
      }
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