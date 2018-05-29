#pragma once
#include "engine/networking/editor/editor_messages.h"

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
      * @struct sulphur::engine::editor::WindowHandlePayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::EditorMessageID::kWindowHandle
      * @author Maarten ten Velden
      */
      struct WindowHandlePayload : Payload
      {
        int64_t handle; //!< The window handle to render to
      };
      /**
      * @see sulphur::engine::editor::EditorMessageID::kWindowHandle
      */
      using WindowHandleMessage = EditorMessage<EditorMessageID::kWindowHandle, WindowHandlePayload>;

      /**
      * @struct sulphur::engine::editor::EntityCreatedPayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::EditorMessageID::kEntityCreated
      * @author Maarten ten Velden
      */
      struct EntityCreatedPayload : Payload
      {
        size_t entity_index; //!< The index in the hierarchy of the new entity
        size_t sibling_index; //!< The sibling index of the new entity
        size_t parent_index; //!< The index in the hierarchy of the new entity's parent (or PS_SIZE_MAX to indicate no parent)
      };
      /**
      * @see sulphur::engine::editor::EditorMessageID::kEntityCreated
      */
      using EntityCreatedMessage = EditorMessage<EditorMessageID::kEntityCreated, EntityCreatedPayload>;

      /**
      * @struct sulphur::engine::editor::EntityDestroyedPayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::EditorMessageID::kEntityDestroyed
      * @author Maarten ten Velden
      */
      struct EntityDestroyedPayload : Payload
      {
        size_t entity_index; //!< The index in the hierarchy of the entity to be destroyed
      };
      /**
      * @see sulphur::engine::editor::EditorMessageID::kEntityDestroyed
      */
      using EntityDestroyedMessage = EditorMessage<EditorMessageID::kEntityDestroyed, EntityDestroyedPayload>;

      /**
      * @struct sulphur::engine::editor::EntityCreatedPayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::EditorMessage::kEntityCreated
      * @author Maarten ten Velden
      */
      struct EntityReparentedPayload : Payload
      {
        size_t entity_old_index; //!< The index before reparenting in the hierarchy of the entity
        size_t new_parent_old_index; //!< The index before reparenting in the hierarchy of the entity's new parent (or PS_SIZE_MAX to indicate no parent)
        size_t new_sibling_index; //!< The index after reparenting in the hierarchy of the entity
      };
      /**
      * @see sulphur::engine::editor::EditorMessage::kEntityCreated
      */
      using EntityReparentedMessage = EditorMessage<EditorMessageID::kEntityReparented, EntityReparentedPayload>;

      /**
      * @struct sulphur::engine::editor::AssetInstantiatedPayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::EditorMessageID::kAssetInstantiated
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
      * @see sulphur::engine::editor::EditorMessageID::kAssetInstantiated
      * @todo (Maarten) Deprecate this message
      */
      using AssetInstantiatedMessage = EditorMessage<EditorMessageID::kAssetInstantiated, AssetInstantiatedPayload>;

      /**
      * @struct sulphur::engine::editor::ComponentAddedPayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::EditorMessageID::kComponentAdded
      * @author Maarten ten Velden
      */
      struct ComponentAddedPayload : Payload
      {
        size_t entity_index; //!< The index in the hierarchy of the entity to add the component to
        size_t component_type_id; //!< The ID of component system to target
      };
      /**
      * @see sulphur::engine::editor::EditorMessageID::kComponentAdded
      */
      using ComponentAddedMessage = EditorMessage<EditorMessageID::kComponentAdded, ComponentAddedPayload>;

      /**
      * @struct sulphur::engine::editor::ComponentRemovedPayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::EditorMessageID::kComponentRemoved
      * @author Maarten ten Velden
      */
      struct ComponentRemovedPayload : Payload
      {
        size_t entity_index; //!< The index in the hierarchy of the entity from which to remove the component
        size_t component_type_id; //!< The ID of component system to target
      };
      /**
      * @see sulphur::engine::editor::EditorMessageID::kComponentRemoved
      */
      using ComponentRemovedMessage = EditorMessage<EditorMessageID::kComponentRemoved, ComponentRemovedPayload>;

      /**
      * @see sulphur::engine::editor::EditorMessageID::kCacheChanged
      */
      using CacheChangedMessage = EditorMessage<EditorMessageID::kCacheChanged, Payload>;

      /**
      * @see sulphur::engine::editor::EditorMessageID::kStartedPlaying
      */
      using StartedPlayingMessage = EditorMessage<EditorMessageID::kStartedPlaying, Payload>;
      /**
      * @see sulphur::engine::editor::EditorMessageID::kStoppedPlaying
      */
      using StoppedPlayingMessage = EditorMessage<EditorMessageID::kStoppedPlaying, Payload>;


      struct EntitySelectedPayload : Payload
      {
        size_t entity_index;
      };
      
      using EntitySelectedMessage = EditorMessage<EditorMessageID::kEntitySelected, EntitySelectedPayload>;

      struct EntityMovedPayload : Payload
      {
        size_t entity_index;
        float x;
        float y;
        float z;
      };

      using EntityMovedMessage = EditorMessage<EditorMessageID::kEntityMoved, EntityMovedPayload>;

      struct EntityRotatePayload : Payload
      {
        size_t entity_index;
        float w;
        float x;
        float y;
        float z;
      };

      using EntityRotateMessage = EditorMessage<EditorMessageID::kEntityRotated, EntityMovedPayload>;

      struct EntityScalePayload : Payload
      {
        size_t entity_index;
        float x;
        float y;
        float z;
      };

      using EntityScaleMessage = EditorMessage<EditorMessageID::kEntityScaled, EntityScalePayload>;

      struct SetTransformGizmoPayload : Payload
      {
        enum struct Type
        {
          kTranslate,
          kRotate,
          kScale
        };

        int32_t type;
      };

      using SetTransformGizmoMessage = EditorMessage<EditorMessageID::kSetTransformGizmo, SetTransformGizmoPayload>;
      
      struct LoadProjectPayload : Payload
      {
        char path[networking::editor::kMaxPayloadSize - 1];
      };

      using LoadProjectMessage = EditorMessage<EditorMessageID::kLoadProject, LoadProjectPayload>;
    }
  }
}