#include "scale_gizmo.h"
#include "engine\systems\components\camera_system.h"
#include "engine\systems\components\transform_system.h"
#include "engine\graphics\debug_render_system.h"
#include "engine\assets\asset_system.h"
#include "foundation\math\ray.h"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "engine\networking\editor\editor_messaging_system.h"
#include "engine\networking\editor\editor_message_payloads.h"
namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------------------------------
    ScaleGizmo::ScaleGizmo() :
      accumulated_scale_(glm::vec3(0,0,0)),
      axis_scale_(0.0f),
      scaling_axis_({ 0,0,0 }),
      kBlockSize_(0.125f),
      kStemLength_(1.0f),
      kStemRadius_(0.025f),
      kCenterSize_(0.15f),
      kAABBLength_(kStemLength_ + kBlockSize_),
      kAABBHalfSize_(kBlockSize_ * 0.5f),
      display_mode_(DisplayMode::kUnused)
    {
    }

    //------------------------------------------------------------------------------------------------------
    void ScaleGizmo::Initialize()
    {
      axis_mesh_.AttachMesh(
        Mesh::CreateCylinder(kStemLength_, kStemRadius_, kStemRadius_, 6),
        glm::vec3(0.0f, 0.5f * kStemLength_, 0.0f));
      
      axis_mesh_.AttachMesh(
        Mesh::CreateCube(),
        glm::vec3(0.0f, kStemLength_, 0.0f),
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec3(kBlockSize_, kBlockSize_, kBlockSize_));

      Mesh* scale_mesh = foundation::Memory::Construct<Mesh>();
      y_axis_ = AssetSystem::Instance().AddAsset(scale_mesh, "scale_gizmo_y_axis");

      scale_mesh = foundation::Memory::Construct<Mesh>();
      z_axis_ = AssetSystem::Instance().AddAsset(scale_mesh, "scale_gizmo_z_axis");

      scale_mesh = foundation::Memory::Construct<Mesh>();
      x_axis_ = AssetSystem::Instance().AddAsset(scale_mesh, "scale_gizmo_x_axis");

      scale_mesh = foundation::Memory::Construct<Mesh>();
      uniform_axis_ = AssetSystem::Instance().AddAsset(scale_mesh, "scale_gizmo_uniform_axis");


      x_axis_->SetAlwaysOnTop(true);
      y_axis_->SetAlwaysOnTop(true);
      z_axis_->SetAlwaysOnTop(true);
      uniform_axis_->SetAlwaysOnTop(true);
    }

    //------------------------------------------------------------------------------------------------------
    void ScaleGizmo::Release()
    {
      x_axis_ = MeshHandle();
      y_axis_ = MeshHandle();
      z_axis_ = MeshHandle();
      uniform_axis_ = MeshHandle();
    }

    //------------------------------------------------------------------------------------------------------
    void ScaleGizmo::ManipulationStart(const foundation::Ray& camera_ray, foundation::Vector<TransformComponent>& attached_entities)
    {
      start_scale_ = attached_entities[0].GetWorldScale();
      switch (scaling_mode_)
      {
      case ScaleMode::kAxis:
      {
        float axis_t;
        float camera_ray_t;
        start_pos_ = position();
        foundation::Ray axis{ start_pos_,  RotateVector(scaling_axis_, rotation_) };
        axis.ClosedDistanceToRay(camera_ray, axis_t, camera_ray_t);
        previous_intersect_ = axis.origin + axis.direction * axis_t;
        manipulation_start_ = previous_intersect_;
        break;
      }
      case ScaleMode::kUniform:
        previous_intersect_ = glm::vec3(0,0,0);
        start_pos_ = position();
        break;
      }
      set_is_manipulating(true);
    }

    //------------------------------------------------------------------------------------------------------
    void ScaleGizmo::ManipulateTransforms(
      foundation::Vector<TransformComponent>&, 
      const foundation::Ray& camera_ray,
      TransformComponent camera_transform,
      editor::EditorMessagingSystem&)
    {
      switch (scaling_mode_)
      {
      case ScaleMode::kAxis:
      {
        float axis_t;
        float camera_ray_t;
        foundation::Ray axis{ start_pos_, RotateVector(scaling_axis_, rotation_) };
        axis.ClosedDistanceToRay(camera_ray, axis_t, camera_ray_t);
        glm::vec3 current_intersect = axis.origin + axis.direction * axis_t;

        float dot = glm::dot(current_intersect - previous_intersect_, scaling_axis_);
        float applied_scale = 0.0f;

        dot < 0.0f ? 
          applied_scale += glm::length(current_intersect - previous_intersect_) :
          applied_scale -= glm::length(current_intersect - previous_intersect_);

        applied_scale /= kStemLength_ * scale();
        accumulated_scale_ += applied_scale * (start_scale_ * scaling_axis_);

        previous_intersect_ = current_intersect;
        axis_scale_ += applied_scale;
        break;
      }
      case ScaleMode::kUniform:
      {
        glm::vec3 camera_y_axis = camera_transform.GetLocalUp();
        glm::vec3 camera_x_axis = camera_transform.GetLocalRight();
        float t;
        camera_ray.IntersectPlane(-camera_transform.GetLocalForward(), camera_ray.origin, t);
        glm::vec3 intersect = camera_ray.origin + camera_ray.direction + t;
        float x = glm::dot(camera_x_axis, intersect - camera_ray.origin);
        float y = glm::dot(camera_y_axis, intersect - camera_ray.origin);
        float applied_scale = 0.0f;
        if (glm::length(previous_intersect_) == 0.0f)
        {
          previous_intersect_ = { x,y, 0.0f };
          break;
        }

        applied_scale += x - previous_intersect_.x;
        applied_scale += y - previous_intersect_.y;
        applied_scale *= 5.0f;
        accumulated_scale_ += applied_scale * start_scale_;
        previous_intersect_ = { x,y, 0.0f };
        axis_scale_ += applied_scale;
      }
      default:
        break;
      }
    }

    void ScaleGizmo::RequestChange(editor::EditorMessagingSystem& system, foundation::Vector<TransformComponent>& attached)
    {
      if (glm::length2(accumulated_scale_) == 0.0f)
      {
        return;
      }

      editor::EntityScalePayload payload;
      payload.entity_index = attached[0].GetHierarchyIndex();
      payload.x = accumulated_scale_.x;
      payload.y = accumulated_scale_.y;
      payload.z = accumulated_scale_.z;
      system.SendToEditor(editor::EditorMessageID::kEntityScaled, payload);
      accumulated_scale_ = { 0,0,0 };
    }

    //------------------------------------------------------------------------------------------------------
    void ScaleGizmo::ManipulationEnd()
    {
      previous_intersect_ = { 0,0,0 };
      scaling_axis_ = { 0,0,0 };
      axis_scale_ = 0.0f;
      display_mode_ = DisplayMode::kUnused;
      set_is_manipulating(false);
    }

    //------------------------------------------------------------------------------------------------------
    void ScaleGizmo::ConstructMesh()
    {
      x_axis_->Clear(true);
      y_axis_->Clear(true);
      z_axis_->Clear(true);
      uniform_axis_->Clear(true);

      foundation::Color mesh_colors[]{
        foundation::Color::kRed,
        foundation::Color::kGreen,
        foundation::Color::kBlue,
        foundation::Color::kGray
      };

      if (display_mode_ != DisplayMode::kUnused)
      {
        mesh_colors[static_cast<size_t>(display_mode_)] = foundation::Color::kYellow;
      }

      Mesh axis_mesh;
      glm::vec3 axis{ 0, 1, 0 };
      ConstructAxisMesh(axis, axis_mesh);
      axis_mesh.SetColor(mesh_colors[static_cast<size_t>(DisplayMode::kYAxisSelected)]);
      y_axis_->AttachMesh(axis_mesh);
      axis_mesh.Clear();

      axis = { 0, 0, 1 };
      ConstructAxisMesh(axis, axis_mesh);
      axis_mesh.SetColor(mesh_colors[static_cast<size_t>(DisplayMode::kZAxisSelected)]);
      z_axis_->AttachMesh(axis_mesh, glm::vec3(), glm::inverse(kRot90X));
      axis_mesh.Clear();

      axis = { 1, 0, 0 };
      ConstructAxisMesh(axis, axis_mesh);
      axis_mesh.SetColor(mesh_colors[static_cast<size_t>(DisplayMode::kXAxisSelected)]);
      x_axis_->AttachMesh(axis_mesh, glm::vec3(),  kRot90Z);
      axis_mesh.Clear();

      uniform_axis_->AttachMesh(
        Mesh::CreateCube().SetColor(mesh_colors[static_cast<size_t>(DisplayMode::kUniformSelected)]),
        glm::vec3(0.0f, 0.0f, 0.0f),glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(kCenterSize_, kCenterSize_, kCenterSize_));   
    }

    //------------------------------------------------------------------------------------------------------
    void ScaleGizmo::Draw(foundation::Vector<TransformComponent>& attached_entities)
    {
      if (attached_entities.size() == 0)
      {
        return;
      }

      ConstructMesh();
      set_position(attached_entities[0].GetWorldPosition());
      rotation_ = attached_entities[0].GetWorldRotation();
 
      DebugRenderSystem::DrawMesh(x_axis_, position(), rotation_);
      DebugRenderSystem::DrawMesh(y_axis_, position(), rotation_);
      DebugRenderSystem::DrawMesh(z_axis_, position(), rotation_);
      DebugRenderSystem::DrawMesh(uniform_axis_, position(), rotation_, glm::vec3(scale()));
    }

    //------------------------------------------------------------------------------------------------------
    bool ScaleGizmo::Select(const foundation::Ray& camera_ray)
    {
      scaling_mode_ = ScaleMode::kAxis;
      bool result = false;
      float closest_t = FLT_MAX;
      float t_min = FLT_MAX;
      float t_max = FLT_MAX;
      float AABB_half_size = kAABBHalfSize_ * scale();

      glm::mat4x4 world_matrix = glm::translate(position()) * glm::toMat4(rotation_);

      foundation::Ray ray;
      ray.origin = glm::inverse(world_matrix) * glm::vec4(camera_ray.origin, 1.0f);
      ray.direction = glm::inverse(world_matrix) * glm::vec4(camera_ray.direction, 0.0f);

      glm::vec3 min_point = glm::vec3(0, 0, 0);
      min_point.y -= AABB_half_size;
      min_point.z -= AABB_half_size;

      glm::vec3 max_point = glm::vec3(0, 0, 0);
      max_point.x += kAABBLength_ * scale();
      max_point.y += AABB_half_size;
      max_point.z += AABB_half_size;

      if (ray.IntersectAABB(min_point, max_point, t_min, t_max) == true)
      {
        if (t_min < closest_t)
        {
          closest_t = t_min;
          scaling_axis_ = { 1,0,0 };
          display_mode_ = DisplayMode::kXAxisSelected;
        }
        result = true;
      }

      min_point = glm::vec3(0, 0, 0);;
      min_point.x -= AABB_half_size;
      min_point.z -= AABB_half_size;

      max_point = glm::vec3(0, 0, 0);;
      max_point.x += AABB_half_size;
      max_point.y += kAABBLength_ * scale();
      max_point.z += AABB_half_size;

      if (ray.IntersectAABB(min_point, max_point, t_min, t_max) == true)
      {
        if (t_min < closest_t)
        {
          closest_t = t_min;
          scaling_axis_ = { 0,1,0 };
          display_mode_ = DisplayMode::kYAxisSelected;
        }
        result = true;
      }

      min_point = glm::vec3(0, 0, 0);
      min_point.x -= AABB_half_size;
      min_point.y -= AABB_half_size;

      max_point = glm::vec3(0, 0, 0);
      max_point.x += AABB_half_size;
      max_point.y += AABB_half_size;
      max_point.z += kAABBLength_ * scale();

      if (ray.IntersectAABB(min_point, max_point, t_min, t_max) == true)
      {
        if (t_min < closest_t)
        {
          closest_t = t_min;
          scaling_axis_ = { 0,0,1 };
          display_mode_ = DisplayMode::kZAxisSelected;
        }
        result = true;
      }

      min_point = glm::vec3(-0.5f * kCenterSize_ * scale());
      max_point = glm::vec3(0.5f * kCenterSize_ * scale());

      if (ray.IntersectAABB(min_point, max_point, t_min, t_max) == true)
      {
        if (t_min < closest_t)
        {
          scaling_mode_ = ScaleMode::kUniform;
          closest_t = t_min;
          scaling_axis_ = { 1,1,1 };
          display_mode_ = DisplayMode::kUniformSelected;
        }
        result = true;
      }

      return result;
    }

    //------------------------------------------------------------------------------------------------------
    glm::vec3 ScaleGizmo::RotateVector(const glm::vec3& v, const glm::quat& q)
    {
      glm::quat uint_q = glm::normalize(q);
      glm::quat pure_q(0.0f, v.x, v.y, v.z);
      glm::quat result = uint_q * pure_q * glm::conjugate(uint_q);
      return glm::vec3(result.x, result.y, result.z);
    }

    void ScaleGizmo::ConstructAxisMesh(glm::vec3& axis, Mesh& mesh)
    {
      Mesh tip = Mesh::CreateCube();
      Mesh stem = Mesh::CreateCylinder(kStemLength_, kStemRadius_, kStemRadius_);

      glm::vec3 pos = glm::vec3(0, kStemLength_ * scale(), 0); // default position without scaling
      if (is_manipulating() == true && scaling_axis_ == axis)
      {
        glm::vec3 dir = manipulation_start_ - previous_intersect_;
        float dot = glm::dot(dir, axis);
        float length = glm::length(dir);
        dot > 0.0f ? pos.y += length : pos.y -= length;
      }
      else if (scaling_axis_ == glm::vec3(1, 1, 1))
      {
        pos.y += axis_scale_;
      }

      float stem_length = pos.y;
      float stem_offset = stem_length * 0.5f;

      mesh.AttachMesh(tip, pos, glm::quat(1, 0, 0, 0), glm::vec3(kBlockSize_ * scale()));
      mesh.AttachMesh(stem, glm::vec3(0, stem_offset, 0), glm::quat(1, 0, 0, 0), glm::vec3(scale(), stem_length, scale()));
    }
  }
}