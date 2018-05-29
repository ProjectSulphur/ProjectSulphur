#pragma once
#include "engine/networking/messages.h"

namespace sulphur
{
  namespace engine
  {
    namespace editor
    {
      /**
      * @brief An enumerator containing the identifiers of all the messages that can be sent/received
      */
      enum struct EditorMessageID : uint32_t
      {
        kWindowHandle = 0,
        kEntityCreated, //!< The creation of a new entity
        kEntityDestroyed, //!< The destruction of an existing entity
        kEntityReparented, //!< The change of an existing entity's parent
        kAssetInstantiated, //!< The instantiation of an asset in the world
        kComponentAdded, //!< The creation of a new component on an entity
        kComponentRemoved, //!< The destruction of an existing component on an entity
        kCacheChanged, //!< Notification that the asset-cache must be reloaded

        kFastBackward, //!< A request from the editor to start rewinding
        kPreviousFrame,  //!< A request from the editor to rewind one frame
        kStartedPlaying, //!< A request from the editor to start playing the game
        kStoppedPlaying, //!< A request from the editor to stop playing the game
        kNextFrame,  //!< A request from the editor to forward one frame
        kFastForward,  //!< A request from the editor to fast forward

        kStartRewinding, //!< A request from the editor to start rewinding
        kStopRewinding, //!< A request from the editor to stop rewinding
        kObjectSelected, //!< An object got selected via a control in the editor.
        kSetTransformGizmo, //!< Sets the current type of transform gizmo to use.
        kEntitySelected,
        kEntityMoved,
        kEntityRotated,
        kEntityScaled,

        kLoadProject,
        kNumMessages, //!< The amount of unique messages that can be received
      };

      /**
      * @struct sulphur::engine::EditorMessageBuffer
      * @brief A small structure that can be used to represent a message whose type is unknown at compile time
      * @author Maarten ten Velden
      */
      struct EditorMessageBuffer
      {
        EditorMessageID id; //!< The unique identifier that specifies the message's type
        MessagePayload payload; //!< The data component of the message
      };

      /**
      * @class sulphur::engine::EditorMessage <sulphur::engine::MessageType, TPayload>
      * @brief A structure that makes sending messages easier by providing an interface that wraps the individual message components.
      * @tparam actual_id (EditorMessageID) The unique identifier that specifies the message type
      * @tparam TPayload (typename) The class that implements the data component of the message
      * @remarks For an example of how this might be used see sulphur::engine::PODMessage
      * @author Maarten ten Velden
      */
      template<EditorMessageID actual_id, typename TPayload>
      class EditorMessage
      {
        static_assert(eastl::is_base_of<Payload, TPayload>::value, "TPayload must be derived from sulphur::engine::Payload");
        static_assert(sizeof(TPayload) < networking::editor::kMaxPayloadSize, "TPayload carries a payload bigger than the maximum allowed size");

      public:

        /**
        * @brief Accessor to edit the internal payload data
        */
        TPayload& payload();
        /**
        * @brief Constant getter to read the internal payload data
        */
        const TPayload& payload() const;
        /**
        * @brief Compile-time constant getter to get the message identifier.
        */
        constexpr EditorMessageID id() const;

      private:
        TPayload payload_;
      };

      //-------------------------------------------------------------------------
      template<EditorMessageID actual_id, typename TPayload>
      inline const TPayload& EditorMessage<actual_id, TPayload>::payload() const
      {
        return payload_;
      }

      //-------------------------------------------------------------------------
      template<EditorMessageID actual_id, typename TPayload>
      inline TPayload& EditorMessage<actual_id, TPayload>::payload()
      {
        return payload_;
      }

      //-------------------------------------------------------------------------
      template<EditorMessageID actual_id, typename TPayload>
      inline constexpr EditorMessageID EditorMessage<actual_id, TPayload>::id() const
      {
        return actual_id;
      }
    }
  }
}