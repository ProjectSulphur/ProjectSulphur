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

        float postion[3];
        float rotation[4];
        float scale[3];
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
      /**
      * @see sulphur::engine::editor::EditorMessageID::kStoppedPlaying
      */
      using PauseMessage = EditorMessage<EditorMessageID::kPause, Payload>;

      using ContinuePlayingMessage = EditorMessage<EditorMessageID::kContinuePlaying, Payload>;
      /**
      * @struct sulphur::engine::editor::EntitySelectedPayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::EditorMessageID::kEntitySelected
      * @author Maarten ten Velden
      */
      struct EntitySelectedPayload : Payload
      {
        size_t entity_index; //!< Hierarchy index of the entity selected.
      };
      
      /**
      * @see sulphur::engine::editor::EditorMessageID::EntitySelectedPayload
      */
      using EntitySelectedMessage = EditorMessage<EditorMessageID::kEntitySelected, EntitySelectedPayload>;

      /**
      * @struct sulphur::engine::editor::EntityMovedPayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::EditorMessageID::kEntityMoved
      * @author Maarten ten Velden
      */
      struct EntityMovedPayload : Payload
      {
        size_t entity_index; //!< Hierarchy index of the entity selected.
        float x; //!< New X position of the object.
        float y; //!< New Y position of the object.
        float z; //!< New Z position of the object.
      };

      /**
      * @see sulphur::engine::editor::EditorMessageID::EntitySelectedPayload
      */
      using EntityMovedMessage = EditorMessage<EditorMessageID::kEntityMoved, EntityMovedPayload>;

      /**
      * @struct sulphur::engine::editor::EntityRotatePayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::EditorMessageID::kEntityRotated
      * @author Maarten ten Velden
      */
      struct EntityRotatePayload : Payload
      {
        size_t entity_index; //!< Hierarchy index of the entity selected.
        float w; //!< New W component of the rotation quaternion.
        float x; //!< New X component of the rotation quaternion.
        float y; //!< New Y component of the rotation quaternion.
        float z; //!< New Z component of the rotation quaternion.
      };

      /**
      * @see sulphur::engine::editor::EditorMessageID::EntityRotatePayload
      */
      using EntityRotateMessage = EditorMessage<EditorMessageID::kEntityRotated, EntityMovedPayload>;

      /**
      * @struct sulphur::engine::editor::EntityScalePayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::EditorMessageID::kEntityScaled
      * @author Maarten ten Velden
      */
      struct EntityScalePayload : Payload
      {
        size_t entity_index; //!< Hierarchy index of the entity selected.
        float x; //!< New X scale of the object.
        float y; //!< New Y scale of the object.
        float z; //!< New Z scale of the object.
      };

      /**
      * @see sulphur::engine::editor::EditorMessageID::EntitySelectedPayload
      */
      using EntityScaleMessage = EditorMessage<EditorMessageID::kEntityScaled, EntityScalePayload>;

      /**
      * @struct sulphur::engine::editor::SetTransformGizmoPayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::EditorMessageID::kSetTransformGizmo
      * @author Maarten ten Velden
      */
      struct SetTransformGizmoPayload : Payload
      {
        /**
        * @brief Type of transform gizmos available.
        */
        enum struct Type
        {
          kTranslate, //!< Translate gizmo used to move objects
          kRotate, //!< Rotate gizmo used to rotate objects
          kScale //!< Scale gizmo used to scale objects
        };

        int32_t type; //!< The type of gizmo selected
      };

      /**
      * @see sulphur::engine::editor::EditorMessageID::SetTransformGizmoPayload
      */
      using SetTransformGizmoMessage = EditorMessage<EditorMessageID::kSetTransformGizmo, SetTransformGizmoPayload>;
      
      /**
      * @struct sulphur::engine::editor::LoadProjectPayload : sulphur::engine::editor::Payload
      * @see sulphur::engine::editor::EditorMessageID::kSetTransformGizmo
      * @author Maarten ten Velden
      */
      struct LoadProjectPayload : Payload
      {
        unsigned char path[networking::editor::kMaxPayloadSize - 1]; //!< Path to the *.spr file.
      };

      /**
      * @see sulphur::engine::editor::EditorMessageID::LoadProjectPayload
      */
      using LoadProjectMessage = EditorMessage<EditorMessageID::kLoadProject, LoadProjectPayload>;
    }
  }
}