#include "engine/networking/editor/editor_listener.h"

namespace sulphur
{
  namespace engine
  {
    class AssetSystem;

    namespace editor
    {
      class EditorMessagingSystem;

      /**
      * @class sulphur::engine::editor::AssetSystemListener : sulphur::engine::editor::IEditorListener
      * @brief Class that processed editor messaged related to the asset system.
      * @author Stan Pepels
      */
      class AssetSystemListener : public IEditorListener
      {
      public:
        /**
        * @brief Subscribes the listener to a message system used for processing messages from the editor.
        * @param[in] asset_system (sulphur::engine::AssetSystem&) Asset system to use when processing editor messages.
        * @param[in] message_system (sulphur::engine::editor::EditorMessagingSystem&) Message system to recieve editor messages from.
        */
        AssetSystemListener(AssetSystem& asset_system, EditorMessagingSystem& message_system);

        /**
        * @see sulphur::engine::editor::IEditorListener::OnReceive
        */
        void OnReceive(EditorMessageID id, const MessagePayload& payload) override;
      private:
        AssetSystem* system_; //!< System to use when processing editor messages.
      };
    }
  }
}