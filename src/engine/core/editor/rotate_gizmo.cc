#include "rotate_gizmo.h"
#include "engine\systems\components\camera_system.h"
#include "engine\systems\components\transform_system.h"
#include "engine\graphics\debug_render_system.h"
#include "engine\assets\asset_system.h"
#include "foundation\math\ray.h"

#include "engine\networking\editor\editor_messaging_system.h"
#include "engine\networking\editor\editor_message_payloads.h"

namespace sulphur
{
  namespace engine
  {

    //------------------------------------------------------------------------------------------------------
    RotateGizmo::RotateGizmo() :
      accumulated_rotation_(glm::quat(1,0,0,0)),
      kRadius_(1.0f),
      kSelectionThreshold_(0.1f),
      rotation_(glm::quat(1, 0, 0, 0)),
      rotation_x_axis_(glm::vec3(1, 0, 0)),
      rotation_y_axis_(glm::vec3(0, 1, 0)),
      rotation_z_axis_(glm::vec3(0, 0, 1)),
      display_mode_(DisplayMode::kUnused)
    {
    }

    //------------------------------------------------------------------------------------------------------
    void RotateGizmo::Initialize()
    {
      Mesh* rotation_mesh = foundation::Memory::Construct<Mesh>();
      circle_mesh_.AttachMesh(
        Mesh::CreateLineCircle(),
        glm::vec3(),
        glm::quat(1, 0, 0, 0),
        glm::vec3(kRadius_, kRadius_, 1.0f));

      gizmo_mesh_ = AssetSystem::Instance().AddAsset(rotation_mesh, "rotate_gizmo");
      gizmo_mesh_->SetTopologyType(graphics::TopologyType::kLine);
      gizmo_mesh_->SetAlwaysOnTop(true);
    }

    //------------------------------------------------------------------------------------------------------
    void RotateGizmo::Release()
    {
      gizmo_mesh_ = MeshHandle();
    }

    //------------------------------------------------------------------------------------------------------
    void RotateGizmo::ManipulationStart(const foundation::Ray& camera_ray, foundation::Vector<TransformComponent>&)
    {
      float t_min;
      float t_max;
      if (camera_ray.IntersectSphere(position(), kRadius_ * scale(), t_min, t_max) == false)
      {
        return;
      }
      glm::vec3 intersect = camera_ray.origin + camera_ray.direction * t_min;
      glm::vec3 v = intersect - position();
      switch (rotate_mode_)
      {
      case sulphur::engine::RotateGizmo::RotateMode::kAxis:
      {
        float dot = glm::dot(rotation_axis_, v);
        glm::vec3 p = intersect - dot * rotation_axis_;
        previous_intersect_ = p - position();
        break;
      }
      case sulphur::engine::RotateGizmo::RotateMode::kFree:
        previous_intersect_ = glm::vec3(0, 0, 0);
        break;
      }
      set_is_manipulating(true);
    }

    //------------------------------------------------------------------------------------------------------
    void RotateGizmo::ManipulateTransforms(
      foundation::Vector<TransformComponent>&,
      const foundation::Ray& camera_ray,
      TransformComponent camera_transform,
      editor::EditorMessagingSystem&)
    {
      rotation_x_axis_ = RotateVector(glm::vec3(1, 0, 0), rotation_);
      rotation_y_axis_ = RotateVector(glm::vec3(0, 1, 0), rotation_);
      rotation_z_axis_ = RotateVector(glm::vec3(0, 0, 1), rotation_);
      glm::quat applied_rotation = glm::quat(1,0,0,0);

      switch (rotate_mode_)
      {
      case sulphur::engine::RotateGizmo::RotateMode::kAxis:
      {
        float t_min;
        float t_max;
        if (camera_ray.IntersectSphere(position(), kRadius_ * scale(), t_min, t_max) == false)
        {
          return;
        }
        glm::vec3 intersect = camera_ray.origin + camera_ray.direction * t_min;
        glm::vec3 normal = intersect - position();
        glm::vec3 current_intersect = previous_intersect_;
        float dot;

        dot = glm::dot(rotation_axis_, normal);
        glm::vec3 point = intersect - dot * rotation_axis_;
        current_intersect = point - position();
        dot = glm::dot(current_intersect, previous_intersect_);
        float sqrt = glm::sqrt(glm::length2(current_intersect) * glm::length2(previous_intersect_));
        float angle = glm::acos(dot / sqrt);
        glm::vec3 cross = glm::cross(current_intersect, previous_intersect_);
        dot = glm::dot(cross, rotation_axis_);

        if (glm::isnan(angle) == true)
        {
          return;
        }
        if (dot > 0.0f)
        {
          angle *= -1;
        }
        glm::quat rotation = glm::angleAxis(angle, rotation_axis_);
        glm::quat temp_rot = rotation_;
        applied_rotation = glm::inverse(temp_rot) * rotation * temp_rot;
        previous_intersect_ = current_intersect;
        break;
      }
      case sulphur::engine::RotateGizmo::RotateMode::kFree:
        glm::vec3 camera_y_axis = camera_transform.GetLocalUp();
        glm::vec3 camera_x_axis = camera_transform.GetLocalRight();
        float t;
        camera_ray.IntersectPlane(-camera_transform.GetLocalForward(), camera_ray.origin, t);
        glm::vec3 intersect = camera_ray.origin + camera_ray.direction + t;
        //positions on the near z plane
        float x = -(glm::dot(camera_x_axis, intersect - camera_ray.origin));
        float y = glm::dot(camera_y_axis, intersect - camera_ray.origin);
        if (glm::length(previous_intersect_) == 0)
        {
          previous_intersect_ = { x,y,0 };
          applied_rotation = glm::quat(1, 0, 0, 0);
          break;
        }

        float x_angle = y - previous_intersect_.y;
        float y_angle = x - previous_intersect_.x;

        glm::quat rotation = glm::angleAxis(x_angle, camera_x_axis);
        glm::quat temp_rot = rotation_;
        applied_rotation = glm::inverse(temp_rot) * rotation * temp_rot;

        rotation = glm::angleAxis(y_angle, camera_y_axis);
        temp_rot = rotation_;
        applied_rotation *= glm::inverse(temp_rot) * rotation * temp_rot;

        previous_intersect_ = { x,y, 0.0f };
      {
        break;
      }
      }
      accumulated_rotation_ = applied_rotation * accumulated_rotation_;

      // rotation_ *= applied_rotation;
      // attached[0].SetWorldRotation(rotation_);
    }

    void RotateGizmo::RequestChange(editor::EditorMessagingSystem& system, foundation::Vector<TransformComponent>& attached)
    {
      glm::vec3 vec{ accumulated_rotation_.x, accumulated_rotation_.y, accumulated_rotation_.z };
      if (glm::length2(vec) == 0.0f)
      {
        return;
      }

      editor::EntityRotatePayload payload;
      payload.entity_index = attached[0].GetHierarchyIndex();
      payload.w = accumulated_rotation_.w;
      payload.x = accumulated_rotation_.x;
      payload.y = accumulated_rotation_.y;
      payload.z = accumulated_rotation_.z;
      system.SendToEditor(editor::EditorMessageID::kEntityRotated, payload);
      accumulated_rotation_ = {1,0,0,0 };
    }

    //------------------------------------------------------------------------------------------------------
    void RotateGizmo::ManipulationEnd()
    {
      display_mode_ = DisplayMode::kUnused;
      set_is_manipulating(false);
    }

    //------------------------------------------------------------------------------------------------------
    void RotateGizmo::Draw(foundation::Vector<TransformComponent>& attached_entities)
    {
      if (attached_entities.size() == 0)
      {
        return;
      }

      ConstructMesh();
      set_position(attached_entities[0].GetWorldPosition());
      rotation_ = attached_entities[0].GetWorldRotation();
      DebugRenderSystem::DrawMesh(gizmo_mesh_, position(), rotation_, glm::vec3(scale()));
    }

    //------------------------------------------------------------------------------------------------------
    bool RotateGizmo::Select(const foundation::Ray& camera_ray)
    {
      float t_min;
      float t_max;

      if (camera_ray.IntersectSphere(position(), kRadius_ * scale(), t_min, t_max) == false)
      {
        return false;
      }

      rotate_mode_ = RotateMode::kAxis;
      glm::vec3 intersection = camera_ray.origin + camera_ray.direction * t_min;
      glm::vec3 normal = glm::normalize(intersection - position());
      rotation_axis_ = rotation_x_axis_;
      float dot = glm::dot(normal, rotation_axis_);
      if (dot > -kSelectionThreshold_ && dot < kSelectionThreshold_)
      {
        display_mode_ = DisplayMode::kXAxisSelected;
        return true;
      }

      rotation_axis_ = rotation_z_axis_;
      dot = glm::dot(normal, rotation_axis_);
      if (dot > -kSelectionThreshold_ && dot < kSelectionThreshold_)
      {
        display_mode_ = DisplayMode::kZAxisSelected;
        return true;
      }

      rotation_axis_ = rotation_y_axis_;
      dot = glm::dot(normal, rotation_axis_);
      if (dot > -kSelectionThreshold_ && dot < kSelectionThreshold_)
      {
        display_mode_ = DisplayMode::kYAxisSelected;
        return true;
      }

      rotate_mode_ = RotateMode::kFree;
      return true;
    }

    //------------------------------------------------------------------------------------------------------
    void RotateGizmo::ConstructMesh()
    {
      gizmo_mesh_->Clear(true);
      foundation::Color mesh_colors_[] = {
        foundation::Color::kBlue,
        foundation::Color::kGreen,
        foundation::Color::kRed
      };

      if (display_mode_ != DisplayMode::kUnused)
      {
        mesh_colors_[static_cast<size_t>(display_mode_)] = foundation::Color::kYellow;
      }

      circle_mesh_.SetColor(mesh_colors_[static_cast<size_t>(DisplayMode::kZAxisSelected)]);
      gizmo_mesh_->AttachMesh(circle_mesh_);

      circle_mesh_.SetColor(mesh_colors_[static_cast<size_t>(DisplayMode::kYAxisSelected)]);
      gizmo_mesh_->AttachMesh(circle_mesh_, glm::vec3(), glm::inverse(kRot90X));

      circle_mesh_.SetColor(mesh_colors_[static_cast<size_t>(DisplayMode::kXAxisSelected)]);
      gizmo_mesh_->AttachMesh(circle_mesh_, glm::vec3(),kRot90Y);
    }

    //------------------------------------------------------------------------------------------------------
    glm::vec3 RotateGizmo::RotateVector(const glm::vec3& v, const glm::quat& q)
    {
      glm::quat uint_q = glm::normalize(q);
      glm::quat pure_q(0.0f, v.x, v.y, v.z);
      glm::quat result = uint_q * pure_q * glm::conjugate(uint_q);
      return glm::vec3(result.x, result.y, result.z);
    }
  }
}