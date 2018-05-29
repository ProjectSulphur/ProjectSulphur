#include "translate_gizmo.h"
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
    TranslateGizmo::TranslateGizmo() :
      accumulated_translation_(glm::vec3(0,0,0)),
      display_mode_(DisplayMode::kUnused),
      kArrowLength_(1.0f),
      kArrowRadius_(0.025f),
      kArrowTipRadius_(0.075f),
      kArrowTipLength_(0.25f),
      kPlaneSize_(0.25f),
      kAABBLength_(kArrowLength_ + kArrowTipLength_),
      kAABBHalfSize_(kArrowTipRadius_)
    {
    }
    

    //------------------------------------------------------------------------------------------------------
    void TranslateGizmo::Initialize()
    {
      arrow_mesh_.AttachMesh(
        Mesh::CreateCylinder(kArrowLength_, kArrowRadius_, kArrowRadius_, 6),
        glm::vec3(0.0f, 0.5f * kArrowLength_, 0.0f));

      arrow_mesh_.AttachMesh(
        Mesh::CreateCone(kArrowTipLength_, kArrowTipRadius_),
        glm::vec3(0.0f, kArrowLength_, 0.0f));

      plane_mesh_.AttachMesh(
        Mesh::CreatePlane(),
        glm::vec3(0.5f * kPlaneSize_, 0.0f, 0.5f * kPlaneSize_),
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec3(kPlaneSize_, 1.0f, kPlaneSize_)
      );

      Mesh* translation_mesh = foundation::Memory::Construct<Mesh>();
      gizmo_mesh_ = AssetSystem::Instance().AddAsset(translation_mesh, "translate_gizmo");
      gizmo_mesh_->SetAlwaysOnTop(true);
    }

    //------------------------------------------------------------------------------------------------------
    void TranslateGizmo::Draw(foundation::Vector<TransformComponent>& attached_entities)
    {
      if (attached_entities.size() == 0)
      {
        return;
      }

      ConstructMesh();
      set_position(attached_entities[0].GetWorldPosition());
      DebugRenderSystem::DrawMesh(gizmo_mesh_,
        position(),
        glm::quat(1, 0, 0, 0),
        glm::vec3(scale()));
    }

    //------------------------------------------------------------------------------------------------------
    bool TranslateGizmo::Select(const foundation::Ray& camera_ray)
    {
      return SelectAxis(camera_ray) == true ? true : SelectPlane(camera_ray);
    }

    //------------------------------------------------------------------------------------------------------
    void TranslateGizmo::ConstructMesh()
    {
      gizmo_mesh_->Clear(true);

      foundation::Color mesh_colors[]{
        foundation::Color::kRed,
        foundation::Color::kGreen,
        foundation::Color::kBlue,
        foundation::Color::kRed,
        foundation::Color::kGreen,
        foundation::Color::kBlue };

      if (display_mode_ != DisplayMode::kUnused)
      {
        mesh_colors[static_cast<size_t> (display_mode_)] = foundation::Color::kYellow;
      }

      arrow_mesh_.SetColor(mesh_colors[static_cast<size_t> (DisplayMode::kYAxisSelected)]);
      gizmo_mesh_->AttachMesh(arrow_mesh_);

      arrow_mesh_.SetColor(mesh_colors[static_cast<size_t> (DisplayMode::kZAxisSelected)]);
      gizmo_mesh_->
        AttachMesh(arrow_mesh_, glm::vec3(), glm::inverse(kRot90X));

      arrow_mesh_.SetColor(mesh_colors[static_cast<size_t> (DisplayMode::kXAxisSelected)]);
      gizmo_mesh_->
        AttachMesh(arrow_mesh_, glm::vec3(), kRot90Z);

      plane_mesh_.SetColor(mesh_colors[static_cast<size_t> (DisplayMode::kXYPlaneSelected)]);
      gizmo_mesh_->AttachMesh(plane_mesh_);

      plane_mesh_.SetColor(mesh_colors[static_cast<size_t> (DisplayMode::kXZPlaneSelected)]);
      gizmo_mesh_->
        AttachMesh(plane_mesh_, glm::vec3(), kRot90X);

      plane_mesh_.SetColor(mesh_colors[static_cast<size_t> (DisplayMode::kZYPlaneSelected)]);
      gizmo_mesh_->
        AttachMesh(plane_mesh_, glm::vec3(), glm::inverse(kRot90Z));
    }

    //------------------------------------------------------------------------------------------------------
    bool TranslateGizmo::SelectAxis(const foundation::Ray& camera_ray)
    {
      bool result = false;
      translation_mode_ = TranslationMode::kAxis;
      float closest_t = FLT_MAX;
      float t_min = FLT_MAX;
      float t_max = FLT_MAX;
      float AABB_half_size = kAABBHalfSize_ * scale();

      glm::vec3 min_point = position();
      min_point.y -= AABB_half_size;
      min_point.z -= AABB_half_size;

      glm::vec3 max_point = position();
      max_point.x += kAABBLength_ * scale();
      max_point.y += AABB_half_size;
      max_point.z += AABB_half_size;

      if (camera_ray.IntersectAABB(min_point, max_point, t_min, t_max) == true)
      {
        if (t_min < closest_t)
        {
          closest_t = t_min;
          translate_axis_ = { 1,0,0 };
          display_mode_ = DisplayMode::kXAxisSelected;
        }
        result = true;
      }

      min_point = position();
      min_point.x -= AABB_half_size;
      min_point.z -= AABB_half_size;

      max_point = position();
      max_point.x += AABB_half_size;
      max_point.y += kAABBLength_ * scale();
      max_point.z += AABB_half_size;

      if (camera_ray.IntersectAABB(min_point, max_point, t_min, t_max) == true)
      {
        if (t_min < closest_t)
        {
          closest_t = t_min;
          translate_axis_ = { 0,1,0 };
          display_mode_ = DisplayMode::kYAxisSelected;
        }
        result = true;
      }

      min_point = position();
      min_point.x -= AABB_half_size;
      min_point.y -= AABB_half_size;

      max_point = position();
      max_point.x += AABB_half_size;
      max_point.y += AABB_half_size;
      max_point.z += kAABBLength_ * scale();

      if (camera_ray.IntersectAABB(min_point, max_point, t_min, t_max) == true)
      {
        if (t_min < closest_t)
        {
          closest_t = t_min;
          translate_axis_ = { 0,0,1 };
          display_mode_ = DisplayMode::kZAxisSelected;
        }
        result = true;
      }
      return result;
    }

    //------------------------------------------------------------------------------------------------------
    bool TranslateGizmo::SelectPlane(const foundation::Ray& camera_ray)
    {
      translation_mode_ = TranslationMode::kPlane;
      bool result = false;
      glm::vec3 pos = position();
      float plane_offset = kPlaneSize_ * 0.5f * scale();


      auto point_in_plane = [](glm::vec3 up,
        glm::vec3 right,
        glm::vec3 point,
        glm::vec3 origin,
        glm::vec2 size)
      {
        size *= 0.5f;
        float x = glm::dot(right, point - origin);
        float y = glm::dot(up, point - origin);
        return (x > -size.x && x < size.x) && (y < size.y && y > -size.y);
      };

      // x, y
      float closest_t = FLT_MAX;
      float t = FLT_MAX;

      if (camera_ray.IntersectPlane(glm::vec3(0, 0, 1), glm::vec3(0, 0, pos.z), t) == true)
      {
        bool test = point_in_plane(glm::vec3(0, 1, 0),
          glm::vec3(1, 0, 0),
          camera_ray.origin + camera_ray.direction * t,
          glm::vec3(pos.x + plane_offset, pos.y + plane_offset, pos.z),
          glm::vec2(kPlaneSize_ * scale(), kPlaneSize_ *scale()));
        if (test == true && t < closest_t)
        {
          result = true;
          display_mode_ = DisplayMode::kXYPlaneSelected;
          translation_plane_normal_ = glm::vec3(0, 0, 1);
          closest_t = t;
        }
      }

      // z, y
      if (camera_ray.IntersectPlane(glm::vec3(1, 0, 0), glm::vec3(pos.x, 0, 0), t) == true)
      {
        bool test = point_in_plane(glm::vec3(0, 1, 0),
          glm::vec3(0, 0, 1),
          camera_ray.origin + camera_ray.direction * t,
          glm::vec3(pos.x, pos.y + plane_offset, pos.z + plane_offset),
          glm::vec2(kPlaneSize_ * scale(), kPlaneSize_ *scale()));
        if (test == true && t < closest_t)
        {
          result = true;
          display_mode_ = DisplayMode::kZYPlaneSelected;
          translation_plane_normal_ = glm::vec3(1, 0, 0);
          closest_t = t;
        }
      }

      // y, z
      if (camera_ray.IntersectPlane(glm::vec3(0, 1, 0), glm::vec3(0, pos.y, 0), t) == true)
      {
        bool test = point_in_plane(glm::vec3(0, 0, 1),
          glm::vec3(1, 0, 0),
          camera_ray.origin + camera_ray.direction * t,
          glm::vec3(pos.x + plane_offset, pos.y, pos.z + plane_offset),
          glm::vec2(kPlaneSize_ * scale(), kPlaneSize_ *scale()));
        if (test == true && t < closest_t)
        {
          result = true;
          display_mode_ = DisplayMode::kZYPlaneSelected;
          translation_plane_normal_ = glm::vec3(0, 1, 0);
          closest_t = t;
        }
      }
      return result;
    }

    //------------------------------------------------------------------------------------------------------
    void TranslateGizmo::Release()
    {
      gizmo_mesh_ = MeshHandle();
    }

    //------------------------------------------------------------------------------------------------------
    void TranslateGizmo::ManipulationStart(const foundation::Ray& camera_ray,
                                           foundation::Vector<TransformComponent>&)
    {
      accumulated_translation_ = { 0,0,0 };
      switch (translation_mode_)
      {
      case TranslationMode::kAxis:
      {
        float axis_t;
        float camera_ray_t;
        start_pos_ = position();
        foundation::Ray axis{ start_pos_, translate_axis_ };
        axis.ClosedDistanceToRay(camera_ray, axis_t, camera_ray_t);
        previous_intersect_ = axis.origin + axis.direction * axis_t;
        break;
      }
      case TranslationMode::kPlane:
        float t;
        camera_ray.IntersectPlane(translation_plane_normal_,
          position() * translation_plane_normal_, t);
        previous_intersect_ = camera_ray.origin + camera_ray.direction *t;
        break;
      default:
        break;
      }
      set_is_manipulating(true);
    }

    //------------------------------------------------------------------------------------------------------
    void TranslateGizmo::ManipulateTransforms(
      foundation::Vector<TransformComponent>&,
      const foundation::Ray& camera_ray,
      TransformComponent,
      editor::EditorMessagingSystem&)
    {
      switch (translation_mode_)
      {
      case TranslationMode::kAxis:
      {
        float axis_t;
        float camera_ray_t;
        foundation::Ray axis{ start_pos_,  translate_axis_ };
        axis.ClosedDistanceToRay(camera_ray, axis_t, camera_ray_t);
        glm::vec3 current_intersect = axis.origin + axis.direction * axis_t;
         
        accumulated_translation_ += previous_intersect_ - current_intersect;
        previous_intersect_ = current_intersect;
        break;
      }
      case TranslationMode::kPlane:
      {
        float t;
        if (camera_ray.IntersectPlane(translation_plane_normal_,
          position() * translation_plane_normal_, t) == true)
        {
          glm::vec3 current_intersect = camera_ray.origin + camera_ray.direction * t;
          accumulated_translation_ += current_intersect - previous_intersect_;
          previous_intersect_ = current_intersect;
        }
      }
      default:
        break;
      }
    }

    void TranslateGizmo::RequestChange(editor::EditorMessagingSystem& system, foundation::Vector<TransformComponent>& attached)
    {
      if (glm::length2(accumulated_translation_) == 0.0f)
      {
        return;
      }

      editor::EntityMovedPayload payload;
      payload.entity_index = attached[0].GetHierarchyIndex();
      payload.x = accumulated_translation_.x;
      payload.y = accumulated_translation_.y;
      payload.z = accumulated_translation_.z;
      system.SendToEditor(editor::EditorMessageID::kEntityMoved, payload);
      accumulated_translation_ = { 0,0,0 };
    }
    //------------------------------------------------------------------------------------------------------
    void TranslateGizmo::ManipulationEnd()
    {
      display_mode_ = DisplayMode::kUnused;
      set_is_manipulating(false);
    }
  }
}