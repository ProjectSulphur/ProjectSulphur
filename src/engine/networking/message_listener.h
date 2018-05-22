#pragma once
#include "engine/networking/message_in.h"

namespace sulphur 
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::IMessageListener
    * @brief An interface that classes listening for messages can derive from
    * @see sulphur::engine::SubscriptionSystem
    * @author Maarten ten Velden
    */
    class IMessageListener
    {
    public:
      /**
      * @brief The callback function that is called when a message of a type subscriped to comes in (either locally or via the net)
      * @param[in] id (sulphur::engine::MessageID&) The unique identifier that specifies the message type
      * @param[in] payload (const sulphur::engine::MessagePayload&) The abstract data component of the message.
      * @see sulphur::engine::SubscriptionSystem
      */
      virtual void OnReceive(MessageID id, const MessagePayload& payload) = 0;

    };
  }
}