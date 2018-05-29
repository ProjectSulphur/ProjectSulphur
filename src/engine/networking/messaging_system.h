#pragma once
#include "engine/systems/service_system.h"
#include "engine/networking/subscription_manager.h"

#include <foundation/memory/memory.h>
#include <foundation/containers/vector.h>

namespace sulphur
{
  namespace engine
  {
    enum struct MessageID : uint32_t;
    class MessagePayload;
    /**
    * @class sulphur::engine::MessagingSystem : sulphur::engine::IServiceSystem
    *          <sulphur::engine::MessagingSystem>, sulphur::engine::SubscriptionManager
    * @brief This service represents the entire messaging sytem (network comms and internal processing)
    * @author Maarten ten Velden
    */
    class MessagingSystem : public IServiceSystem<MessagingSystem>, public SubscriptionManager
    {
    public:
      /**
      * @brief Constructor that initializes the system to only process local messages.
      */
      MessagingSystem();

      /**
      * @see sulphur::engine::IServiceSystem::OnInitialize
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;

      /**
      * @see sulphur::engine::IServiceSystem::OnTerminate
      */
      void OnTerminate() override;

      /**
      * @brief Test for incoming messages and distribute them to listeners
      */
      void ReceiveMessages();

      /**
      * @brief Send a message through the local system to listeners
      * @param[in] id (const sulphur::engine::MessagePayload&) The unique identifier that describes the type of message being sent
      * @param[in] payload (const sulphur::engine::MessagePayload&) The data to send along with the message
      */
      void SendLocal(MessageID id, const MessagePayload& payload);

    private:
      foundation::Vector<MessageBuffer> incoming_messages_; //!< Messages that still have to be distributed to listeners (local + network received)
      foundation::Vector<MessageBuffer> outgoing_messages_; //!< Messages that still have to be sent to the editor

    };
  }
}