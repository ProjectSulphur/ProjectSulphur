#pragma once
#include "engine/networking/editor/editor_listener.h"

namespace sulphur
{
  namespace engine
  {
    class TransformSystem;

    namespace editor
    {
      class EditorMessagingSystem;

      /**
      * @class sulphur::engine::EntitySystemListener : sulphur::engine::IEditorListener
      * @brief Class that processed editor messaged related to the entity system.
      * @author Stan Pepels
      */
      class TransformSystemListener : public IEditorListener
      {
      public:
        /**
        * @brief Subscribes the listener to a message system used for processing messages from the editor.
        * @param[in] asset_system (sulphur::engine::EntitySystem&) Asset system to use when processing editor messages.
        * @param[in] message_system (sulphur::engine::EditorMessagingSystem&) Message system to recieve editor messages from.
        */
        TransformSystemListener(TransformSystem& transform_system, EditorMessagingSystem& message_system);

        /**
        * @see sulphur::engine::IEditorListener::OnReceive
        */
        void OnReceive(EditorMessageID id, const MessagePayload& payload) override;
      private:
        EditorMessagingSystem* messaging_system_;
        TransformSystem* system_;
      };
    }
  }
}