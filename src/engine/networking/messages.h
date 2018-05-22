#pragma once
#include "engine/networking/message_out.h"

#include <glm/glm.hpp>

#include <cinttypes>

namespace sulphur 
{
  namespace engine 
  {
    /**
    * Adding a custom message requires the following steps to be completed:
    * 1. Add an enumerator for the new message to sulphur::engine::MessageID
    * 2. Declare a struct that inherits from sulphur::engine::Payload in this file that
    *    implement the data component of the message you are adding.
    * 3. Declare an alias that specifies sulphur::engine::Message <sulphur::engine::MessageID, TPayload>
    *    passing it the identifier and the payload class
    */

    /**
    * @struct sulphur::engine::PODPayload : sulphur::engine::Payload
    * @see sulphur::engine::MessageID::kPOD
    * @author Maarten ten Velden
    */
    struct PODPayload : Payload
    {
      byte data[networking::editor::kMaxPayloadSize];
    };
    using PODMessage = Message<MessageID::kPOD, PODPayload>;
    
    /**
    * @struct sulphur::engine::StringPayload : sulphur::engine::Payload
    * @see sulphur::engine::MessageID::kString
    * @author Maarten ten Velden
    */
    struct StringPayload : Payload
    {
      char string[64];
    };
    using StringMessage = Message<MessageID::kString, StringPayload>;

    /**
    * @struct sulphur::engine::WindowHandlePayload : sulphur::engine::Payload
    * @see sulphur::engine::MessageID::kWindowHandle
    * @author Maarten ten Velden
    */
    struct WindowHandlePayload : Payload
    {
      int64_t handle;
    };
    using WindowHandleMessage = Message<MessageID::kWindowHandle, WindowHandlePayload>;
    
    /**
    * @struct sulphur::engine::EntityCreatedPayload : sulphur::engine::Payload
    * @see sulphur::engine::MessageID::kEntityCreated
    * @author Maarten ten Velden
    */
    struct EntityCreatedPayload : Payload
    {
      size_t entity_id; //!< The ID of the new entity
    };
    using EntityCreatedMessage = Message<MessageID::kEntityCreated, EntityCreatedPayload>;

    /**
    * @struct sulphur::engine::EntityDestroyedPayload : sulphur::engine::Payload
    * @see sulphur::engine::MessageID::kEntityDestroyed
    * @author Maarten ten Velden
    */
    struct EntityDestroyedPayload : Payload
    {
      size_t entity_id; //!< The ID of the entity to be destroyed
    };
    using EntityDestroyedMessage = Message<MessageID::kEntityDestroyed, EntityDestroyedPayload>;

    /**
    * @struct sulphur::engine::ComponentAddedPayload : sulphur::engine::Payload
    * @see sulphur::engine::MessageID::kComponentAdded
    * @author Maarten ten Velden
    */
    struct ComponentAddedPayload : Payload
    {
      size_t entity_id; //!< The ID of the entity to which the component was attached
      size_t component_type_id; //!< The ID of component system to target
    };
    using ComponentAddedMessage = Message<MessageID::kComponentAdded, ComponentAddedPayload>;

    /**
    * @struct sulphur::engine::ComponentRemovedPayload : sulphur::engine::Payload
    * @see sulphur::engine::MessageID::kComponentRemoved
    * @author Maarten ten Velden
    */
    struct ComponentRemovedPayload : Payload
    {
      size_t entity_id; //!< The ID of the entity from which the component was removed
      size_t component_type_id; //!< The ID of component system to target
      size_t component_id; //!< The ID of the component to be removed
    };
    using ComponentRemovedMessage = Message<MessageID::kComponentRemoved, ComponentRemovedPayload>;

    /**
    * @struct sulphur::engine::ClickRaycastPayload : sulphur::engine::Payload
    * @see sulphur::engine::MessageID::kClickRaycast
    * @author Maarten ten Velden
    */
    struct ClickRaycastPayload : Payload
    {
      size_t camera_component_id; //!< The ID of the camera component to target
      glm::vec2 screen_position; //!< The normalized position (from 0 to 1) inside the camera's viewport
    };
    using ClickRaycastMessage = Message<MessageID::kClickRaycast, ClickRaycastPayload>;

    /**
    * @struct sulphur::engine::RaycastPayload : sulphur::engine::Payload
    * @see sulphur::engine::MessageID::kRaycast
    * @author Maarten ten Velden
    */
    struct RaycastPayload : Payload
    {
      glm::vec3 origin; //!< The origin of the ray in world-space.
      glm::vec3 direction; //!< The direction of the ray in world-space.
    };
    using RaycastMessage = Message<MessageID::kRaycast, RaycastPayload>;

    /**
    * @struct sulphur::engine::RaycastResultPayload : sulphur::engine::Payload
    * @see sulphur::engine::MessageID::kRaycastResult
    * @author Maarten ten Velden
    */
    struct RaycastResultPayload : Payload
    {
      glm::vec3 origin; //!< The origin of the ray in world-space.
      glm::vec3 direction; //!< The direction of the ray in world-space.
      float t; //!< The point along the ray at which something was hit equal to -1 if nothing was hit
      size_t entity_id; //!< The ID of the entity that was hit
    };
    using RaycastResultMessage = Message<MessageID::kRaycastResult, RaycastResultPayload>;

  }
}