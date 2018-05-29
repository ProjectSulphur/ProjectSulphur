#include "engine/networking/editor/editor_subscription_manager.h"
#include "engine/networking/editor/editor_listener.h"
#include "engine/networking/editor/editor_messages.h"

#include <foundation/logging/logger.h>

namespace sulphur
{
  namespace engine
  {
    namespace editor
    {

      //-------------------------------------------------------------------------
      void EditorSubscriptionManager::Subscribe(IEditorListener* listener, EditorMessageID id)
      {
        PS_LOG_IF(id == EditorMessageID::kNumMessages, Fatal, "Trying to subscribe to EditorMessageID::kNumMessages");

        SubscriptionList& target_list = subscriptions_[static_cast<uint32_t>(id)];

        if (target_list.empty() == false)
        {
          if (eastl::find(target_list.begin(), target_list.end(), listener) != target_list.end())
          {
            return;
          }
        }

        target_list.push_front(listener);
      }

      //-------------------------------------------------------------------------
      void EditorSubscriptionManager::Unsubscribe(IEditorListener* listener, EditorMessageID id)
      {
        SubscriptionList& target_list = subscriptions_[static_cast<uint32_t>(id)];
        target_list.remove(listener);
      }

      //-------------------------------------------------------------------------
      void EditorSubscriptionManager::UnsubscribeAll(IEditorListener* listener)
      {
        for (uint32_t i = 0; i < static_cast<uint32_t>(EditorMessageID::kNumMessages); ++i)
        {
          subscriptions_[i].remove(listener);
        }
      }

      //-------------------------------------------------------------------------
      void EditorSubscriptionManager::NotifySubscribers(EditorMessageID id, const MessagePayload& payload)
      {
        SubscriptionList& target_list = subscriptions_[static_cast<uint32_t>(id)];
        for (SubscriptionList::iterator it = target_list.begin(); it != target_list.end(); ++it)
        {
          (*it)->OnReceive(id, payload);
        }
      }
    }
  }
}