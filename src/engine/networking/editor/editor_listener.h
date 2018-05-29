#pragma once
#include <cinttypes>
namespace sulphur
{
  namespace engine
  {
    class MessagePayload;
    namespace editor
    {

      enum struct EditorMessageID : uint32_t;

      /**
      * @class sulphur::engine::editor::IEditorListener
      * @brief An interface that classes listening for messages comming from the editor can derive from
      * @see sulphur::engine::editor::EditorSubscriptionSystem
      * @author Stan Pepels
      */
      class IEditorListener
      {
      public:

        virtual ~IEditorListener() {};
        /**
        * @brief The callback function that is called when a message of a type subscriped to comes in from the editor
        * @param[in] id (sulphur::engine::editor::EditorMessageID) The unique identifier that specifies the message type
        * @param[in] payload (const sulphur::engine::MessagePayload&) The abstract data component of the message.
        * @see sulphur::engine::SubscriptionSystem
        */
        virtual void OnReceive(EditorMessageID id, const MessagePayload& payload) = 0;

      };
    }
  }
}