#include "free_cam_system.h"

#include "engine/systems/components/transform_system.h"
#include "engine/systems/components/camera_system.h"
#include "engine/input/input.h"

#include <foundation/job/job_graph.h>
#include <foundation/job/data_policy.h>
#include <foundation/utils/frame.h>

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    FreeCamSystem::FreeCamSystem() :
      IOwnerSystem("FreeCamSystem")
    {
      move_sensitivity_ = glm::vec3(50.0f);
      look_sensitivity_ = glm::vec2(5.0f);
      mouse_speed_ = 50.0f; //!< Mouse offset is scaled by a factor of 50
    }

    //-------------------------------------------------------------------------
    void FreeCamSystem::OnInitialize(Application& app, foundation::JobGraph& job_graph)
    {
      input_ = &app.platform().input();
      world_ = &app.GetService<WorldProviderSystem>().GetWorld();

      entity_ = world_->GetOwner<EntitySystem>().Create();
      entity_.Add<TransformComponent>();

      CameraComponent camera = entity_.Add<CameraComponent>();
      camera.make_main_camera();

      // @note (Maarten) Yes, your eyes work perfectly fine. These numbers are in fact hard-code
      //   because I feel like it is a good starting position. The camera looks along the
      //   positive z-axis by default.
      TransformComponent transform = entity_.Get<TransformComponent>();
      transform.SetLocalPosition(glm::vec3(0.0, 15.0f, -50.0f));

      auto free_cam_update = [](FreeCamSystem& free_cam)
      {
        // @todo (Maarten) This should only be updated when focused
        free_cam.UpdateMovement();
        free_cam.UpdateLook();
      };

      foundation::Job free_cam_update_job = foundation::make_job("free_cam_update", "editor_update",
        free_cam_update,
        foundation::bind_write(*this));

      job_graph.Add(std::move(free_cam_update_job));
    }

    //-------------------------------------------------------------------------
    void FreeCamSystem::OnTerminate()
    {
      world_->GetOwner<EntitySystem>().Destroy(entity_);
    }

    //-------------------------------------------------------------------------
    void FreeCamSystem::set_move_sensitivity(const glm::vec3& sensitivity)
    {
      move_sensitivity_ = sensitivity;
    }

    //-------------------------------------------------------------------------
    void FreeCamSystem::set_look_sensitivity(const glm::vec2& sensitivity)
    {
      look_sensitivity_ = sensitivity;
    }

    //-------------------------------------------------------------------------
    void FreeCamSystem::set_mouse_speed(float speed)
    {
      mouse_speed_ = speed;
    }

    //-------------------------------------------------------------------------
    glm::vec3 FreeCamSystem::move_sensitivity()
    {
      return move_sensitivity_;
    }

    //-------------------------------------------------------------------------
    glm::vec2 FreeCamSystem::look_sensitivity()
    {
      return look_sensitivity_;
    }

    //-------------------------------------------------------------------------
    float FreeCamSystem::mouse_speed()
    {
      return mouse_speed_;
    }

    //-------------------------------------------------------------------------
    void FreeCamSystem::UpdateMovement()
    {
      // @todo (Maarten) Replace hard-coded input with customizable mappings

      glm::vec3 movement(0.0f);

      // Keyboard input
      if (input_->IsButtonDown(Button::kD) == true)
      {
        movement.x += 1.0f;
      }
      if (input_->IsButtonDown(Button::kA) == true)
      {
        movement.x -= 1.0f;
      }
      if (input_->IsButtonDown(Button::kW) == true)
      {
        movement.z += 1.0f;
      }
      if (input_->IsButtonDown(Button::kS) == true)
      {
        movement.z -= 1.0f;
      }
      if (input_->IsButtonDown(Button::kSpace) == true)
      {
        movement.y += 1.0f;
      }
      if (input_->IsButtonDown(Button::kLeftControl) == true)
      {
        movement.y -= 1.0f;
      }

      movement.x += input_->GetAxis(Axis::kLeftX);
      movement.z -= input_->GetAxis(Axis::kLeftY);
      if (input_->IsButtonDown(Button::kGamepadActionDown) == true)
      {
        movement.y += 1.0f;
      }
      if (input_->IsButtonDown(Button::kGamepadActionRight) == true)
      {
        movement.y -= 1.0f;
      }

      TransformComponent transform = entity_.Get<TransformComponent>();

      // @note (Maarten) It is safe to do everything in local-space here as the free cam is
      //   always parent-less
      glm::vec3 right = transform.GetLocalRight();
      glm::vec3 forward = transform.GetLocalForward();
      glm::vec3 up = transform.GetLocalUp();

      transform.TranslateLocal(
        right * movement.x * move_sensitivity_.x * foundation::Frame::delta_time() +
        forward * movement.z * move_sensitivity_.z * foundation::Frame::delta_time() +
        up * movement.y * move_sensitivity_.y * foundation::Frame::delta_time());
    }

    //-------------------------------------------------------------------------
    void FreeCamSystem::UpdateLook()
    {
      glm::vec2 rotation(0.f); // in euler-angles

      if (input_->IsButtonDown(Button::kRightMouse) == true)
      {
        rotation += input_->GetMouseOffset() * mouse_speed_;
      }

      rotation.x += input_->GetAxis(Axis::kRightX);
      rotation.y += input_->GetAxis(Axis::kRightY);
      if (input_->IsButtonDown(Button::kRight) == true)
      {
        rotation.x += 1.0f;
      }
      if (input_->IsButtonDown(Button::kLeft) == true)
      {
        rotation.x -= 1.0f;
      }
      if (input_->IsButtonDown(Button::kUp) == true)
      {
        rotation.y -= 1.0f;
      }
      if (input_->IsButtonDown(Button::kDown) == true)
      {
        rotation.y += 1.0f;
      }

      TransformComponent transform = entity_.Get<TransformComponent>();

      transform.RotateWorldY(
        rotation.x * look_sensitivity_.x * foundation::Frame::delta_time());
      transform.RotateLocalX(
        rotation.y * look_sensitivity_.y * foundation::Frame::delta_time());
    }
  }
}