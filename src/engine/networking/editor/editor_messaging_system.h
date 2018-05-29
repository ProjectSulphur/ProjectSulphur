#pragma once
#include "engine/systems/service_system.h"
#include "engine/networking/editor/editor_subscription_manager.h"
#include "engine/networking/editor/connection_manager.h"

#include <foundation/memory/memory.h>
#include <foundation/containers/vector.h>

namespace sulphur
{
  namespace engine
  {
    namespace editor
    {

      /**
      * @class sulphur::engine::editor::EditorMessagingSystem : sulphur::engine::editor::IServiceSystem
      *          <sulphur::engine::editor::EditorMessagingSystem>, sulphur::engine::editor::EditorSubscriptionManager
      * @brief This service is used for communicating with the editor only. To send messages locally see sulphur::engine::MessagingSystem
      * @see sulphur::engine::MessagingSystem
      * @author Stan Pepels
      */
      class EditorMessagingSystem : public IServiceSystem<EditorMessagingSystem>, public EditorSubscriptionManager
      {
      public:
        /**
        * @brief Constructor that injects a valid connection object
        * @param[in] connection (foundation::SharedPointer<sulphur::engine::editor::ConnectionManager>) The connection manager to get messages from.
        */
        EditorMessagingSystem(foundation::SharedPointer<editor::ConnectionManager> connection);

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
        * @brief Send queued messages to the editor
        */
        void DispatchMessages();

        /**
        * @brief Send a message over the network to the editor
        * @param[in] id (const sulphur::engine::editor::EditorMessageID) The unique identifier that describes the type of message being sent
        * @param[in] payload (const sulphur::engine::MessagePayload&) The data to send along with the message
        */
        void SendToEditor(EditorMessageID id, const MessagePayload& payload);

      private:
        foundation::SharedPointer<ConnectionManager> connection_; //!< A reference to the object that managers the engine-to-editor connection and communication

        foundation::Vector<EditorMessageBuffer> incoming_messages_; //!< Messages that still have to be distributed to listeners.
        foundation::Vector<EditorMessageBuffer> outgoing_messages_; //!< Messages that still have to be sent to the editor

      };
    }
  }
}