#include "engine/networking/editor/editor_messaging_system.h"

#include "engine/networking/message_listener.h"
#include "engine/networking/messages.h"
#include <foundation/logging/logger.h>
#include <algorithm>

namespace sulphur
{
  namespace engine
  {
    namespace editor
    {
      //-------------------------------------------------------------------------
      EditorMessagingSystem::EditorMessagingSystem(foundation::SharedPointer<editor::ConnectionManager> connection) :
        IServiceSystem("MessagingSystem"),
        connection_(connection)
      {
      }

      //-------------------------------------------------------------------------
      void EditorMessagingSystem::OnInitialize(Application&, foundation::JobGraph&)
      {
      }

      //-------------------------------------------------------------------------
      void EditorMessagingSystem::OnTerminate()
      {
      }

      //-------------------------------------------------------------------------
      void EditorMessagingSystem::ReceiveMessages()
      {
        EditorMessageID id;
        MessagePayload payload;
        while (connection_->RetrieveNextMessage(&id, &payload))
        {
          NotifySubscribers(id, payload);
        }
      }

      //-------------------------------------------------------------------------
      void EditorMessagingSystem::DispatchMessages()
      {
        if (connection_->IsConnected() == false)
        {
          return;
        }

        using MessageIt = foundation::Vector<EditorMessageBuffer>::iterator;
        for (MessageIt it = outgoing_messages_.begin(); it != outgoing_messages_.end(); ++it)
        {
          connection_->SendMessage(it->id, it->payload);
        }
        outgoing_messages_.clear();

        connection_->FlushMessages();
      }

      //-------------------------------------------------------------------------
      void EditorMessagingSystem::SendToEditor(EditorMessageID id, const MessagePayload& payload)
      {
        EditorMessageBuffer buffer;
        buffer.id = id;
        buffer.payload = payload;
        outgoing_messages_.emplace_back(buffer);
      }
    }
  }
}