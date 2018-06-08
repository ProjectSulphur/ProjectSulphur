#include "tranform_system_listener.h"
#include "engine/systems/components/transform_system.h"
#include "engine/networking/editor/editor_messaging_system.h"
#include "engine/networking/editor/editor_message_payloads.h"

namespace sulphur
{
  namespace engine
  {
    namespace editor
    {
     
      //----------------------------------------------------------------------------------------------------
      TransformSystemListener::TransformSystemListener(TransformSystem& transform_system, EditorMessagingSystem& message_system) :
        messaging_system_(&message_system),
        system_(&transform_system)
      {
        messaging_system_->Subscribe(this, EditorMessageID::kEntityMoved);
        messaging_system_->Subscribe(this, EditorMessageID::kEntityRotated);
        messaging_system_->Subscribe(this, EditorMessageID::kEntityScaled);
      }

      //----------------------------------------------------------------------------------------------------
      void TransformSystemListener::OnReceive(EditorMessageID id, const MessagePayload& payload)
      {
        if (id == EditorMessageID::kEntityMoved)
        {
          const EntityMovedPayload& actual_payload =
            payload.AsFormat<editor::EntityMovedPayload>();

          glm::vec3 position{ actual_payload.x, actual_payload.y, actual_payload.z };
          TransformComponent transform = system_->GetByHierarchyIndex(actual_payload.entity_index);
          transform.SetWorldPosition(position);
        }
        else if (id == EditorMessageID::kEntityRotated)
        {
          const EntityRotatePayload& actual_payload =
            payload.AsFormat<editor::EntityRotatePayload>();

          glm::quat rotation{actual_payload.w, actual_payload.x, actual_payload.y, actual_payload.z };
          TransformComponent transform = system_->GetByHierarchyIndex(actual_payload.entity_index);
          transform.SetLocalRotation(rotation);
        }
        else if (id == EditorMessageID::kEntityScaled)
        {
          const EntityScalePayload& actual_payload =
            payload.AsFormat<editor::EntityScalePayload>();

          glm::vec3 scale { actual_payload.x, actual_payload.y, actual_payload.z };
          TransformComponent transform = system_->GetByHierarchyIndex(actual_payload.entity_index);
          transform.SetWorldScale(scale);
        }
      }
    }
  }
}

