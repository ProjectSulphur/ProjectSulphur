#include "engine/networking/subscription_manager.h"
#include "engine/networking/message_listener.h"
#include "engine/networking/messages.h"

#include <foundation/logging/logger.h>

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    void SubscriptionManager::Subscribe(IMessageListener* listener, MessageID id)
    {
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
    void SubscriptionManager::Unsubscribe(IMessageListener* listener, MessageID id)
    {
      SubscriptionList& target_list = subscriptions_[static_cast<uint32_t>(id)];
      target_list.remove(listener);
    }

    //-------------------------------------------------------------------------
    void SubscriptionManager::UnsubscribeAll(IMessageListener* listener)
    {
      for (uint32_t i = 0; i < static_cast<uint32_t>(MessageID::kNumMessages); ++i)
      {
        subscriptions_[i].remove(listener);
      }
    }

    //-------------------------------------------------------------------------
    void SubscriptionManager::NotifySubscribers(MessageID id, const MessagePayload& payload)
    {
      SubscriptionList& target_list = subscriptions_[static_cast<uint32_t>(id)];
      for (SubscriptionList::iterator it = target_list.begin(); it != target_list.end(); ++it)
      {
        (*it)->OnReceive(id, payload);
      }
    }
  }
}