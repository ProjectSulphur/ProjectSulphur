#pragma once
#include "engine/networking/editor/editor_messages.h"
#include <foundation/containers/list.h>

namespace sulphur
{
  namespace engine
  {
    namespace editor
    {

      class IEditorListener;

      /**
      * @class sulphur::engine::editor::EditorSubscriptionManager
      * @brief A class that allows listener classes to subscribe to messages and distributes incoming messages amongst them
      * @see sulphur::engine::editor::IEditorListener
      * @author Stan Pepels
      */
      class EditorSubscriptionManager
      {
      public:
        /**
        * @brief Subscribe a listener to listen to incoming messages of a given type
        * @param[in] listener (sulphur::engine::editor::IEditorListener*) The listener object
        * @param[in] id (sulphur::engine::editor::EditorMessageID) The type of message the object wants to listen for
        */
        void Subscribe(IEditorListener* listener, EditorMessageID id);
        /**
        * @brief Unsubscribe a listener from incoming messages of a currently subscribed-to type
        * @param[in] listener (sulphur::engine::editor::IEditorListener*) The listener object
        * @param[in] id (sulphur::engine::editor::EditorMessageID) The type of message the object wants to stop listening for
        * @remarks Does nothing if the object is not subscribed to the message type that was passed
        */
        void Unsubscribe(IEditorListener* listener, EditorMessageID id);
        /**
        * @brief Unsubscribe a listener from incoming messages of all currently subscribed-to types
        * @param[in] listener (sulphur::engine::editor::IEditorListener*) The listener object
        * @remarks Does nothing if the object is not subscribed to any message types
        */
        void UnsubscribeAll(IEditorListener* listener);

      protected:
        /**
        * @brief Notify all subscribers of an incoming message.
        * @param[in] id (sulphur::engine::editor::EditorMessageID) The unique identifier that specifies the message's type
        * @param[in] payload (const sulphur::engine::MessagePayload&) The payload of the message
        * @remarks The passed payload may be smaller than the sulphur::engine::editor::MessagePayload object
        *          (depending on the type) and should be cast to the desired type
        */
        void NotifySubscribers(EditorMessageID id, const MessagePayload& message);

      private:
        using SubscriptionList = foundation::List<IEditorListener*>; //!< The container used for tracking subscribers
        SubscriptionList subscriptions_[static_cast<uint32_t>(EditorMessageID::kNumMessages)]; //!< The subscribers per message type (one for each type)

      };
    }
  }
}