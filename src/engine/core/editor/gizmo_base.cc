#include "gizmo_base.h"
#include "engine\systems\components\camera_system.h"
#include "engine\systems\components\transform_system.h"

namespace sulphur
{
  namespace engine
  {
    //------------------------------------------------------------------------------------------------------
    GizmoBase::GizmoBase() :
      kRot90X(glm::quat(0.7071068f, 0.7071068f, 0.0f, 0.0f)),
      kRot90Y(glm::quat(0.7071068f, 0.0f, 0.7071068f, 0.0f)),
      kRot90Z(glm::quat(0.7071068f, 0.0f, 0.0f, 0.7071068f)),
      kDefaultSize(70.0f),
      size_(kDefaultSize)
    {
    }

    //------------------------------------------------------------------------------------------------------
    void GizmoBase::DrawGizmo(foundation::Vector<TransformComponent>& attached_entities,
      CameraComponent camera)
    {
      CameraComponent camera_comp = camera;
      if (camera_comp.IsValid() == false)
      {
        return;
      }

      scale_ = CalculateScale(camera_comp);
      Draw(attached_entities);
    }
    //------------------------------------------------------------------------------------------------------
    float GizmoBase::size() const
    {
      return size_;
    }

    //------------------------------------------------------------------------------------------------------
    void GizmoBase::set_size(const float new_size)
    {
      size_ = new_size;
    }

    //------------------------------------------------------------------------------------------------------
    bool GizmoBase::active()
    {
      return active_;
    }

    //------------------------------------------------------------------------------------------------------
    bool GizmoBase::is_manipulating()
    {
      return is_manipulating_;
    }

    //------------------------------------------------------------------------------------------------------
    float GizmoBase::scale()
    {
      return scale_;
    }

    //------------------------------------------------------------------------------------------------------
    void GizmoBase::set_is_manipulating(bool value)
    {
      is_manipulating_ = value;
    }

    //------------------------------------------------------------------------------------------------------
    glm::vec3 GizmoBase::position()
    {
      return position_;
    }

    //------------------------------------------------------------------------------------------------------
    void GizmoBase::set_position(const glm::vec3 position)
    {
      position_ = position;
    }

    //------------------------------------------------------------------------------------------------------
    float GizmoBase::CalculateScale(CameraComponent camera)
    {
      TransformComponent transform = camera.GetTransform();
      glm::quat rotation(1, 0, 0, 0);
      glm::vec3 cam_pos = transform.GetWorldPosition();

      float z = glm::dot(position_ - cam_pos, 
        transform.TransformDirection(glm::vec3(0.0f, 0.0f, 1.0f)));

      glm::vec3 a = camera.WorldToScreenPoint(cam_pos + 
        transform.TransformDirection(glm::vec3(0.0f, 0.0f, z)));

      glm::vec3 b = camera.WorldToScreenPoint(cam_pos + transform.
        TransformDirection(glm::vec3(1.0f, 0.0f, z)));

      float magnitude = glm::length(a - b);
      scale_ = size_ / glm::max(magnitude, 0.0001f);
      return scale_;
    }
  }
}