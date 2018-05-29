#pragma once
#include "engine/networking/editor/message_out.h"

#include <foundation/utils/asset_definitions.h>

#include <glm/glm.hpp>

#include <cinttypes>

namespace sulphur 
{
  namespace engine 
  {
    namespace editor
    {
      /**
      * Adding a custom message requires the following steps to be completed:
      * 1. Add an enumerator for the new message to sulphur::engine::editor::MessageID
      * 2. Declare a struct that inherits from sulphur::engine::editor::Payload in this file that
      *    implement the data component of the message you are adding.
      * 3. Declare an alias that specifies sulphur::engine::Message <sulphur::engine::editor::MessageID, TPayload>
      *    passing it the identifier and the payload class
      */

      /**
      * @struct sulphur::engine::editor::PODPayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::MessageID::kPOD
      * @author Maarten ten Velden
      */
      struct PODPayload : Payload
      {
        byte data[networking::editor::kMaxPayloadSize];
      };
      using PODMessage = Message<MessageID::kPOD, PODPayload>;

      /**
      * @struct sulphur::engine::editor::StringPayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::MessageID::kString
      * @author Maarten ten Velden
      */
      struct StringPayload : Payload
      {
        char string[64];
      };
      using StringMessage = Message<MessageID::kString, StringPayload>;

      /**
      * @struct sulphur::engine::editor::WindowHandlePayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::MessageID::kWindowHandle
      * @author Maarten ten Velden
      */
      struct WindowHandlePayload : Payload
      {
        int64_t handle; //!< The window handle to render to
      };
      /**
      * @see sulphur::engine::editor::MessageID::kWindowHandle
      */
      using WindowHandleMessage = Message<MessageID::kWindowHandle, WindowHandlePayload>;

      /**
      * @struct sulphur::engine::editor::EntityCreatedPayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::MessageID::kEntityCreated
      * @author Maarten ten Velden
      */
      struct EntityCreatedPayload : Payload
      {
        size_t entity_index; //!< The index in the hierarchy of the new entity
        size_t sibling_index; //!< The sibling index of the new entity
        size_t parent_index; //!< The index in the hierarchy of the new entity's parent (or PS_SIZE_MAX to indicate no parent)
      };
      /**
      * @see sulphur::engine::editor::MessageID::kEntityCreated
      */
      using EntityCreatedMessage = Message<MessageID::kEntityCreated, EntityCreatedPayload>;

      /**
      * @struct sulphur::engine::editor::EntityDestroyedPayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::MessageID::kEntityDestroyed
      * @author Maarten ten Velden
      */
      struct EntityDestroyedPayload : Payload
      {
        size_t entity_index; //!< The index in the hierarchy of the entity to be destroyed
      };
      /**
      * @see sulphur::engine::editor::MessageID::kEntityDestroyed
      */
      using EntityDestroyedMessage = Message<MessageID::kEntityDestroyed, EntityDestroyedPayload>;

      /**
      * @struct sulphur::engine::editor::EntityCreatedPayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::MessageID::kEntityCreated
      * @author Maarten ten Velden
      */
      struct EntityReparentedPayload : Payload
      {
        size_t entity_old_index; //!< The index before reparenting in the hierarchy of the entity
        size_t new_parent_old_index; //!< The index before reparenting in the hierarchy of the entity's new parent (or PS_SIZE_MAX to indicate no parent)
		size_t new_sibling_index; //!< The index after reparenting in the hierarchy of the entity
      };
      /**
      * @see sulphur::engine::editor::MessageID::kEntityCreated
      */
      using EntityReparentedMessage = Message<MessageID::kEntityReparented, EntityReparentedPayload>;

      /**
      * @struct sulphur::engine::editor::AssetInstantiatedPayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::MessageID::kAssetInstantiated
      * @todo (Maarten) This message is going to be deprecated soon and replaced with a message
      *   that does both entity creation and attaches a model component. Currently the entity
      *   created for this asset is not tracked by the editor (and therefore not visible in the
      *   hierarchy viewer).
      * @author Maarten ten Velden
      */
      struct AssetInstantiatedPayload : Payload
      {
        foundation::AssetID asset_id;
      };
      /**
      * @see sulphur::engine::editor::MessageID::kAssetInstantiated
      * @todo (Maarten) Deprecate this message
      */
      using AssetInstantiatedMessage = Message<MessageID::kAssetInstantiated, AssetInstantiatedPayload>;

      /**
      * @struct sulphur::engine::editor::ComponentAddedPayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::MessageID::kComponentAdded
      * @author Maarten ten Velden
      */
      struct ComponentAddedPayload : Payload
      {
        size_t entity_index; //!< The index in the hierarchy of the entity to add the component to
        size_t component_type_id; //!< The ID of component system to target
      };
      /**
      * @see sulphur::engine::editor::MessageID::kComponentAdded
      */
      using ComponentAddedMessage = Message<MessageID::kComponentAdded, ComponentAddedPayload>;

      /**
      * @struct sulphur::engine::editor::ComponentRemovedPayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::MessageID::kComponentRemoved
      * @author Maarten ten Velden
      */
      struct ComponentRemovedPayload : Payload
      {
        size_t entity_index; //!< The index in the hierarchy of the entity from which to remove the component
        size_t component_type_id; //!< The ID of component system to target
      };
      /**
      * @see sulphur::engine::editor::MessageID::kComponentRemoved
      */
      using ComponentRemovedMessage = Message<MessageID::kComponentRemoved, ComponentRemovedPayload>;

      /**
      * @see sulphur::engine::editor::MessageID::kCacheChanged
      */
      using CacheChangedMessage = Message<MessageID::kCacheChanged, Payload>;

      /**
      * @see sulphur::engine::editor::MessageID::kFastBackward
      */
      using FastBackwardMessage = Message<MessageID::kFastBackward, Payload>;
      /**
      * @see sulphur::engine::editor::MessageID::kPreviousFrame
      */
      using PreviousFrameMessage = Message<MessageID::kPreviousFrame, Payload>;
      /**
      * @see sulphur::engine::editor::MessageID::kStartedPlaying
      */
      using StartedPlayingMessage = Message<MessageID::kStartedPlaying, Payload>;
      /**
      * @see sulphur::engine::editor::MessageID::kStoppedPlaying
      */
      using StoppedPlayingMessage = Message<MessageID::kStoppedPlaying, Payload>;
      /**
      * @see sulphur::engine::editor::MessageID::kNextFrame
      */
      using NextFrameMessage = Message<MessageID::kNextFrame, Payload>;
      /**
      * @see sulphur::engine::editor::MessageID::kFastForward
      */
      using FastForwardMessage = Message<MessageID::kFastForward, Payload>;
    }
  }
}