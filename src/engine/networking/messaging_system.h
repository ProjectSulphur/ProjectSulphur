#pragma once
#include "engine/systems/service_system.h"
#include "engine/networking/message_in.h"
#include "engine/networking/connection_manager.h"
#include "engine/networking/subscription_manager.h"

#include <foundation/containers/vector.h>

namespace sulphur 
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::MessagingSystem : sulphur::engine::IServiceSystem <sulphur::engine::MessagingSystem>
    * @brief This service represents the entire messaging sytem (network comms and internal processing)
    * @author Maarten ten Velden
    */
    class MessagingSystem : public IServiceSystem<MessagingSystem>
    { 
    public:
      /**
       * @brief Default constructor
       */
      MessagingSystem();

      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;
      void OnTerminate() override;

      /**
      * @brief Test for incoming messages and distribute them to listeners
      */
      void ReceiveMessages();
      /**
      * @brief Send queued messages to the editor
      */
      void DispatchMessages();

      /**
      * @brief Send a message over the network to the editor
      * @param[in] id (const sulphur::engine::MessagePayload&) The unique identifier that describes the type of message being sent
      * @param[in] payload (const sulphur::engine::MessagePayload&) The data to send along with the message
      */
      void SendToEditor(MessageID id, const MessagePayload& payload);
      /**
      * @brief Send a message through the local system to listeners
      * @param[in] id (const sulphur::engine::MessagePayload&) The unique identifier that describes the type of message being sent
      * @param[in] payload (const sulphur::engine::MessagePayload&) The data to send along with the message
      */
      void SendLocal(MessageID id, const MessagePayload& payload);

      ConnectionManager connection_; //!< An object that managers the engine-to-editor connection and communication
      SubscriptionManager subscriptions_; //!< An object that manages message subscriptions

    private:
      foundation::Vector<MessageBuffer> incoming_messages_; //!< Messages that still have to be distributed to listeners (local + network received)
      foundation::Vector<MessageBuffer> outgoing_messages_; //!< Messages that still have to be sent to the editor

    };

  }
}