#include "gizmo_system.h"
#include "engine/core/editor/translate_gizmo.h"
#include "engine/core/editor/rotate_gizmo.h"
#include "engine/core/editor/scale_gizmo.h"
#include "engine/input/input.h"

#include "engine/systems/components/camera_system.h"
#include "engine/systems/components/transform_system.h"
#include "engine/networking/editor/editor_message_payloads.h"
#include "engine/networking/editor/editor_messaging_system.h"

#include <foundation/job/job_graph.h>
#include <foundation/job/data_policy.h>
#include <foundation/math/ray.h>

namespace sulphur
{
  namespace engine
  {
    editor::SetTransformGizmoPayload::Type curr_type = editor::SetTransformGizmoPayload::Type::kTranslate;
    //-----------------------------------------------------------------------------------------------
    GizmoSystem::GizmoSystem() :
      IOwnerSystem("GizmoSysem")
    {
    }

    //-----------------------------------------------------------------------------------------------
    void GizmoSystem::OnInitialize(Application& app, foundation::JobGraph& graph)
    {
      World& world = app.GetService<WorldProviderSystem>().GetWorld();
      transform_system_ = &world.GetComponent<TransformSystem>();

      translate_gizmo_ = foundation::Memory::Construct<TranslateGizmo>();
      rotate_gizmo_ = foundation::Memory::Construct<RotateGizmo>();
      scale_gizmo_ = foundation::Memory::Construct<ScaleGizmo>();

      translate_gizmo_->Initialize();
      rotate_gizmo_->Initialize();
      scale_gizmo_->Initialize();



      app.GetService<editor::EditorMessagingSystem>().Subscribe(this, editor::EditorMessageID::kEntitySelected);
      app.GetService<editor::EditorMessagingSystem>().Subscribe(this, editor::EditorMessageID::kSetTransformGizmo);

      auto process_input = [](
        const Input& input,
        CameraSystem& camera_system,
        GizmoSystem& gizmo_system,
        editor::EditorMessagingSystem& message_system)
      {
        CameraComponent camera = camera_system.current_camera();
        if (camera.IsValid() == false || gizmo_system.transform_gizmo_.first == nullptr)
        {
          return;
        }

        TransformComponent transform = camera.GetTransform();
        glm::vec2 mouse_pos = input.GetMousePosition();
        glm::vec3 ray_near = camera.ViewportToWorldPoint({ mouse_pos.x, -mouse_pos.y, 0.0f });
        glm::vec3 ray_far = camera.ViewportToWorldPoint({ mouse_pos.x, -mouse_pos.y, 1.0f });
        glm::vec3 direction = glm::normalize(ray_far - ray_near);
        foundation::Ray camera_to_world{ ray_near,  direction };
        GizmoBase* transform_gizmo = gizmo_system.transform_gizmo_.first;

        if (input.IsButtonFalling(Button::kLeftMouse))
        {
          if (transform_gizmo->Select(camera_to_world) == true)
          {
            transform_gizmo->ManipulationStart(camera_to_world, gizmo_system.transform_gizmo_.second);
          }
        }
        else if (input.IsButtonRising(Button::kLeftMouse))
        {
          if (transform_gizmo->is_manipulating() == true)
          {
            transform_gizmo->ManipulationEnd();
          }
        }

        if (transform_gizmo->is_manipulating() == true)
        {
          transform_gizmo->ManipulateTransforms(gizmo_system.transform_gizmo_.second,
            camera_to_world,
            transform,
            message_system);
        }

      };

      auto draw_gizmo = [](
        CameraSystem& camera_system,
        GizmoSystem& gizmo_system)
      {
        CameraComponent camera = camera_system.current_camera();
        if (camera.IsValid() == false || gizmo_system.transform_gizmo_.first == nullptr)
        {
          return;
        }
        GizmoBase* transform_gizmo = gizmo_system.transform_gizmo_.first;
        transform_gizmo->DrawGizmo(gizmo_system.transform_gizmo_.second, camera);
      };

      auto send_changes = [](
        GizmoSystem& gizmo_system,
        editor::EditorMessagingSystem& message_system)
      {
        if (gizmo_system.transform_gizmo_.first == nullptr)
        {
          return;
        }
        GizmoBase* transform_gizmo = gizmo_system.transform_gizmo_.first;
        transform_gizmo->RequestChange(message_system, gizmo_system.transform_gizmo_.second);
      };

      foundation::Job input_job = foundation::make_job("gizmo_system_handle_input",
        "editor_update",
        process_input,
        foundation::bind_write(app.platform().input()),
        foundation::bind_write(world.GetComponent<CameraSystem>()),
        foundation::bind_write(*this),
        foundation::bind_write(app.GetService<editor::EditorMessagingSystem>()));
      graph.Add(eastl::move(input_job));


      foundation::Job request_change_job = foundation::make_job("gizmo_system_request_change",
        "editor_fixed_update",
        send_changes,
        foundation::bind_write(*this),
        foundation::bind_write(app.GetService<editor::EditorMessagingSystem>()));
      graph.Add(eastl::move(request_change_job));

      foundation::Job draw_job = foundation::make_job("gizmo_system_draw",
        "renderer_startframe",
        draw_gizmo,
        foundation::bind_write(world.GetComponent<CameraSystem>()),
        foundation::bind_write(*this));
      graph.Add(eastl::move(draw_job));
    }

    //-----------------------------------------------------------------------------------------------
    void GizmoSystem::OnTerminate()
    {

    }

    //-----------------------------------------------------------------------------------------------
    void GizmoSystem::OnReceive(editor::EditorMessageID id, const MessagePayload& payload)
    {

      if (id == editor::EditorMessageID::kEntitySelected)
      {
        if (transform_gizmo_.first == nullptr)
        {
          switch (curr_type)
          {
          case editor::SetTransformGizmoPayload::Type::kTranslate:
            transform_gizmo_.first = translate_gizmo_;
            break;
          case editor::SetTransformGizmoPayload::Type::kRotate:
            transform_gizmo_.first = rotate_gizmo_;
            break;
          case editor::SetTransformGizmoPayload::Type::kScale:
            transform_gizmo_.first = scale_gizmo_;
            break;
          }
        }
        const editor::EntitySelectedPayload& actual_payload =
          payload.AsFormat<editor::EntitySelectedPayload>();

        transform_gizmo_.second.clear();
        TransformComponent transform = transform_system_->GetByHierarchyIndex(actual_payload.entity_index);
        if (transform.IsValid() == false)
        {
          PS_LOG(Error, "Invalid transform specified");
          return;
        }
        transform_gizmo_.second.push_back(transform);
      }
      else if (id == editor::EditorMessageID::kSetTransformGizmo)
      {
        const editor::SetTransformGizmoPayload& actual_payload =
          payload.AsFormat<editor::SetTransformGizmoPayload>();

        editor::SetTransformGizmoPayload::Type type =
          static_cast<editor::SetTransformGizmoPayload::Type>(actual_payload.type);

        if (transform_gizmo_.first != nullptr && type != curr_type)
        {
          switch (type)
          {
          case editor::SetTransformGizmoPayload::Type::kTranslate:
            transform_gizmo_.first = translate_gizmo_;
            break;
          case editor::SetTransformGizmoPayload::Type::kRotate:
            transform_gizmo_.first = rotate_gizmo_;
            break;
          case editor::SetTransformGizmoPayload::Type::kScale:
            transform_gizmo_.first = scale_gizmo_;
            break;
          }
        }
        curr_type = type;
      }
    }
  }
}
