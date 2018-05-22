#include "camera_system.h"

#include "engine/systems/components/transform_system.h"
#include "engine/application/application.h"

#include <graphics/platform/pipeline_state.h>

#include <glm/gtc/matrix_transform.hpp>

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    CameraSystem::CameraSystem() :
      IComponentSystem<CameraComponent, CameraData>("CameraSystem")
    {
    }

    //-------------------------------------------------------------------------
    CameraComponent CameraSystem::Create(Entity entity)
    {
      if (!entity.Has<TransformComponent>())
      {
        entity.Add<TransformComponent>();
      }

      CameraComponent ret = CameraComponent(*this, component_data_.data.Add(
        ProjectionMode::kPerspective,
        ClearMode::kColor,
        foundation::Color::kCornFlower,
        RenderTarget(),
        DepthBuffer(),
        LayerMask(),
        0.3f,
        1000.0f,
        glm::vec2(5.0f),
        60.0f,
        window_->size(),
        true,
        glm::mat4(),
        true,
        glm::mat4(),
        glm::mat4(),
        entity));
      UpdateProjection(ret);

      if (!main_camera_.IsValid())
        ret.make_main_camera();

      return ret;
    }

    //-------------------------------------------------------------------------
    void CameraSystem::Destroy(CameraComponent handle)
    {
      //TODO: Validate handle
      component_data_.data.Remove(handle);
    }

    //-------------------------------------------------------------------------
    void CameraSystem::OnInitialize(Application& app, foundation::JobGraph&)
    {
      renderer_ = &app.platform_renderer();
      window_ = &app.platform().window();
    }

    //-------------------------------------------------------------------------
    void CameraSystem::OnUpdate(float)
    {}

    //-------------------------------------------------------------------------
    void CameraSystem::OnPreRender()
    {
      // TODO: Sort camera's based on sorting layer
    }

    void CameraSystem::OnRender()
    {
      //---------------------Clear the cameras------------------------------
      for (int i = 0; i < component_data_.data.size(); ++i)
      {
        if (component_data_.entity[i] == component_data_.data.Get<
          static_cast<size_t>(CameraComponentElements::kEntity)>(main_camera_))
        {
          if (static_cast<glm::ivec2>(component_data_.projection_size[i]) 
            != window_->size())
          {
            component_data_.projection_size[i] = window_->size();
            UpdateProjection(main_camera_);
          }
        }
        switch (component_data_.clear_mode[i])
        {
        case ClearMode::kColor:
          renderer_->ClearDepthBuffer(component_data_.depth_buffer[i]);
          renderer_->ClearRenderTarget(
            component_data_.render_target[i],
            component_data_.clear_color[i]);
          break;
        case ClearMode::kDepthOnly:
          renderer_->ClearDepthBuffer(component_data_.depth_buffer[i]);
          break;
        case ClearMode::kSky:
          // TODO: Draw sky
          break;
        default:
          break;
        }
      }
    }

    //-------------------------------------------------------------------------
    CameraComponent::CameraComponent() :
      system_(nullptr)
    {
    }
    CameraComponent::CameraComponent(System& system, size_t handle) :
      ComponentHandleBase(handle),
      system_(&system)
    {
    }

    //-------------------------------------------------------------------------
    void CameraComponent::SetNearZ(float nearZ)
    {
      system_->SetNearZ(*this, nearZ);
    }
    void CameraSystem::SetNearZ(CameraComponent handle, float nearZ)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (component_data_.near_z[data_index] == nearZ)
      {
        return;
      }
      component_data_.near_z[data_index] = nearZ;
      component_data_.invalid_projection[data_index] = true;
    }

    //-------------------------------------------------------------------------
    float CameraComponent::GetNearZ() const
    {
      return system_->GetNearZ(*this);
    }
    float CameraSystem::GetNearZ(CameraComponent handle) const
    {
      return component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kNearZ)>(handle);
    }

    //-------------------------------------------------------------------------
    void CameraComponent::SetFarZ(float farZ)
    {
      system_->SetFarZ(*this, farZ);
    }
    void CameraSystem::SetFarZ(CameraComponent handle, float farZ)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (component_data_.far_z[data_index] == farZ)
      {
        return;
      }
      component_data_.far_z[data_index] = farZ;
      component_data_.invalid_projection[data_index] = true;
    }

    //-------------------------------------------------------------------------
    float CameraComponent::GetFarZ() const
    {
      return system_->GetFarZ(*this);
    }
    float CameraSystem::GetFarZ(CameraComponent handle) const
    {
      return component_data_.data.Get<static_cast<size_t>(CameraComponentElements::kFarZ)>(handle);
    }

    //-------------------------------------------------------------------------
    void CameraComponent::SetFieldOfView(float fieldOfView)
    {
      system_->SetFieldOfView(*this, fieldOfView);
    }
    void CameraSystem::SetFieldOfView(CameraComponent handle, float fieldOfView)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (component_data_.fov[data_index] == fieldOfView)
      {
        return;
      }
      component_data_.fov[data_index] = fieldOfView;
      component_data_.invalid_projection[data_index] = true;
    }

    //-------------------------------------------------------------------------
    float CameraComponent::GetFieldOfView() const
    {
      return system_->GetFieldOfView(*this);
    }
    float CameraSystem::GetFieldOfView(CameraComponent handle) const
    {
      return component_data_.data.Get<static_cast<size_t>(CameraComponentElements::kFov)>(handle);
    }

    //-------------------------------------------------------------------------
    void CameraComponent::SetOrthographicSize(glm::vec2 orthographicSize)
    {
      system_->SetOrthographicSize(*this, orthographicSize);
    }
    void CameraSystem::SetOrthographicSize(CameraComponent handle, glm::vec2 orthographicSize)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (component_data_.orthographic_size[data_index] == orthographicSize)
      {
        return;
      }
      component_data_.orthographic_size[data_index] = orthographicSize;
      component_data_.invalid_projection[data_index] = true;
    }

    //-------------------------------------------------------------------------
    glm::vec2 CameraComponent::GetOrthographicSize() const
    {
      return system_->GetOrthographicSize(*this);
    }
    glm::vec2 CameraSystem::GetOrthographicSize(CameraComponent handle) const
    {
      return component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kOrthographicSize)>(handle);
    }

    //-------------------------------------------------------------------------
    void CameraComponent::SetClearMode(const ClearMode& clear_mode)
    {
      system_->SetClearMode(*this, clear_mode);
    }
    void CameraSystem::SetClearMode(CameraComponent handle, const ClearMode& clear_mode)
    {
      component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kClearMode)>(handle) = clear_mode;
    }

    //-------------------------------------------------------------------------
    ClearMode CameraComponent::GetClearMode() const
    {
      return system_->GetClearMode(*this);
    }
    ClearMode CameraSystem::GetClearMode(CameraComponent handle) const
    {
      return component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kClearMode)>(handle);
    }

    //-------------------------------------------------------------------------
    void CameraComponent::SetClearColor(const foundation::Color& clear_color)
    {
      system_->SetClearColor(*this, clear_color);
    }
    void CameraSystem::SetClearColor(CameraComponent handle, const foundation::Color& clear_color)
    {
      component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kClearColor)>(handle) = clear_color;
    }

    //-------------------------------------------------------------------------
    foundation::Color CameraComponent::GetClearColor() const
    {
      return system_->GetClearColor(*this);
    }
    foundation::Color CameraSystem::GetClearColor(CameraComponent handle) const
    {
      return component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kClearColor)>(handle);
    }

    //-------------------------------------------------------------------------
    void CameraComponent::SetProjectionMode(const ProjectionMode& projection)
    {
      system_->SetProjectionMode(*this, projection);
    }
    void CameraSystem::SetProjectionMode(CameraComponent handle, const ProjectionMode& projection)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (component_data_.projection_mode[data_index] == projection)
      {
        return;
      }
      component_data_.projection_mode[data_index] = projection;
      component_data_.invalid_projection[data_index] = true;
    }

    //-------------------------------------------------------------------------
    ProjectionMode CameraComponent::GetProjectionMode() const
    {
      return system_->GetProjectionMode(*this);
    }
    ProjectionMode CameraSystem::GetProjectionMode(CameraComponent handle) const
    {
      return component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kProjectMode)>(handle);
    }

    //-------------------------------------------------------------------------
    void CameraComponent::SetProjectionSize(const glm::vec2& projectionSize)
    {
      system_->SetProjectionSize(*this, projectionSize);
    }
    void CameraSystem::SetProjectionSize(CameraComponent handle, const glm::vec2& projectionSize)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (component_data_.projection_size[data_index] == projectionSize)
      {
        return;
      }
      component_data_.projection_size[data_index] = projectionSize;
      component_data_.invalid_projection[data_index] = true;
    }

    //-------------------------------------------------------------------------
    glm::vec2 CameraComponent::GetProjectionSize() const
    {
      return system_->GetProjectionSize(*this);
    }
    glm::vec2 CameraSystem::GetProjectionSize(CameraComponent handle) const
    {
      return component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kProjectionSize)>(handle);
    }

    //-------------------------------------------------------------------------
    glm::mat4 CameraComponent::GetViewMatrix() const
    {
      return system_->GetViewMatrix(*this);
    }
    glm::mat4 CameraSystem::GetViewMatrix(CameraComponent handle) const
    {
      return GetTransform(handle).GetWorldToLocal();
    }

    //-------------------------------------------------------------------------
    const glm::mat4& CameraComponent::GetProjectionMatrix()
    {
      return system_->GetProjectionMatrix(*this);
    }
    const glm::mat4& CameraSystem::GetProjectionMatrix(CameraComponent handle)
    {
      UpdateProjection(handle);
      return component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kProjectMatrix)>(handle);
    }

    //-------------------------------------------------------------------------
    const glm::mat4& CameraComponent::GetViewProjectionMatrix()
    {
      return system_->GetViewProjectionMatrix(*this);
    }
    const glm::mat4& CameraSystem::GetViewProjectionMatrix(CameraComponent handle)
    {
      UpdateViewProjection(handle);
      return component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kViewProjectionMatrix)>(handle);
    }

    //-------------------------------------------------------------------------
    const glm::mat4& CameraComponent::GetInverseViewProjectionMatrix()
    {
      return system_->GetInverseViewProjectionMatrix(*this);
    }
    const glm::mat4& CameraSystem::GetInverseViewProjectionMatrix(CameraComponent handle)
    {
      UpdateViewProjection(handle);
      return component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kViewProjectionMatrix)>(handle);
    }

    //-------------------------------------------------------------------------
    glm::vec2 CameraComponent::ScreenToViewportPoint(const glm::vec2& point) const
    {
      return system_->ScreenToViewportPoint(*this, point);
    }
    glm::vec2 CameraSystem::ScreenToViewportPoint(CameraComponent handle, const glm::vec2& point) const
    {
      PS_LOG_ONCE(Warning, "This function is unevaluated (also in orthographic mode)");

      glm::vec2& projection_size = component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kProjectionSize)>(handle);
      glm::vec2 viewPoint;
      viewPoint.x = 2.0f * point.x / projection_size.x - 1.0f;
      viewPoint.y = 1.0f - (2.0f * point.y) / projection_size.y;
      return viewPoint;
    }

    //-------------------------------------------------------------------------
    glm::vec3 CameraComponent::ScreenToWorldPoint(const glm::vec3& point)
    {
      return system_->ScreenToWorldPoint(*this, point);
    }
    glm::vec3 CameraSystem::ScreenToWorldPoint(CameraComponent handle, const glm::vec3& point)
    {
      PS_LOG_ONCE(Warning, "This function is unevaluated (also in orthographic mode)");

      glm::vec2 viewPoint = ScreenToViewportPoint(handle, glm::vec2(point));
      return ViewportToWorldPoint(handle, glm::vec3(viewPoint, point.z));
    }

    //-------------------------------------------------------------------------
    glm::vec3 CameraComponent::WorldToViewportPoint(const glm::vec3& point)
    {
      return system_->WorldToViewportPoint(*this, point);
    }
    glm::vec3 CameraSystem::WorldToViewportPoint(CameraComponent handle, const glm::vec3& point)
    {
      PS_LOG_ONCE(Warning, "This function is unevaluated (also in orthographic mode)");

      UpdateViewProjection(handle);
      glm::mat4& view_projection = component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kViewProjectionMatrix)>(handle);

      glm::vec4 ray(point.x, point.y, point.z, 1.0f);
      ray = view_projection * ray;

      glm::vec3 viewPoint;
      viewPoint.x = (ray.x / ray.w + 1.0f) * 0.5f;
      viewPoint.y = (1.0f - ray.y / ray.w) * 0.5f;
      viewPoint.z = glm::length(point - GetTransform(handle).GetWorldPosition());

      return viewPoint;
    }

    //-------------------------------------------------------------------------
    glm::vec3 CameraComponent::WorldToScreenPoint(const glm::vec3& point)
    {
      return system_->WorldToScreenPoint(*this, point);
    }
    glm::vec3 CameraSystem::WorldToScreenPoint(CameraComponent handle, const glm::vec3& point)
    {
      PS_LOG_ONCE(Warning, "This function is unevaluated (also in orthographic mode)");

      glm::vec3 viewPoint = WorldToViewportPoint(handle, point);
      return ViewportToScreenPoint(handle, viewPoint);
    }

    //-------------------------------------------------------------------------
    glm::vec3 CameraComponent::ViewportToScreenPoint(const glm::vec3& point) const
    {
      return system_->ViewportToScreenPoint(*this, point);
    }
    glm::vec3 CameraSystem::ViewportToScreenPoint(
      CameraComponent handle,
      const glm::vec3& point) const
    {
      PS_LOG_ONCE(Warning, "This function is unevaluated (also in orthographic mode)");

      return point * glm::vec3(component_data_.data.Get<static_cast<size_t>
        (CameraComponentElements::kProjectionSize)>(handle), 1.0f);
    }

    //-------------------------------------------------------------------------
    glm::vec3 CameraComponent::ViewportToWorldPoint(const glm::vec3& point)
    {
      return system_->ViewportToWorldPoint(*this, point);
    }
    glm::vec3 CameraSystem::ViewportToWorldPoint(CameraComponent handle, const glm::vec3& point)
    {
      PS_LOG_ONCE(Warning, "This function is unevaluated (also in orthographic mode)");

      UpdateViewProjection(handle);

      glm::vec4 tempOrigin = glm::vec4(point.x, point.y, point.z, 1.0f);
      glm::vec4 ray = tempOrigin * component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kInvViewProjectionMatrix)>(handle);

      return (glm::vec3(ray.x, ray.y, ray.z) / ray.w);
    }

    //-------------------------------------------------------------------------
    foundation::Array<glm::vec4, 6> CameraComponent::GetFrustumPlanes()
    {
      return system_->GetFrustumPlanes(*this);
    }
    foundation::Array<glm::vec4, 6> CameraSystem::GetFrustumPlanes(CameraComponent handle)
    {
      UpdateViewProjection(handle);
      glm::mat4& viewProj = component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kViewProjectionMatrix)>(handle);

      foundation::Array<glm::vec4, 6> tempFrustumPlane;
      tempFrustumPlane[0] = glm::normalize(glm::vec4(viewProj[0][3] + viewProj[0][0],
        viewProj[1][3] + viewProj[1][0],
        viewProj[2][3] + viewProj[2][0],
        viewProj[3][3] + viewProj[3][0]));
      tempFrustumPlane[1] = glm::normalize(glm::vec4(viewProj[0][3] - viewProj[0][0],
        viewProj[1][3] - viewProj[1][0],
        viewProj[2][3] - viewProj[2][0],
        viewProj[3][3] - viewProj[3][0]));
      tempFrustumPlane[2] = glm::normalize(glm::vec4(viewProj[0][3] - viewProj[0][1],
        viewProj[1][3] - viewProj[1][1],
        viewProj[2][3] - viewProj[2][1],
        viewProj[3][3] - viewProj[3][1]));
      tempFrustumPlane[3] = glm::normalize(glm::vec4(viewProj[0][3] + viewProj[0][1],
        viewProj[1][3] + viewProj[1][1],
        viewProj[2][3] + viewProj[2][1],
        viewProj[3][3] + viewProj[3][1]));
      tempFrustumPlane[4] = glm::normalize(glm::vec4(viewProj[0][2], 
        viewProj[1][2],
        viewProj[2][2],
        viewProj[3][2]));
      tempFrustumPlane[5] = glm::normalize(glm::vec4(viewProj[0][3] - viewProj[0][2],
        viewProj[1][3] - viewProj[1][2], 
        viewProj[2][3] - viewProj[2][2],
        viewProj[3][3] - viewProj[3][2]));

      return tempFrustumPlane;
    }

    //-------------------------------------------------------------------------
    void CameraSystem::UpdateProjection(CameraComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);
      if (!component_data_.invalid_projection[data_index])
      {
        return;
      }

      switch (component_data_.projection_mode[data_index])
      {
      case ProjectionMode::kPerspective:
        component_data_.projection_matrix[data_index] = glm::perspectiveFovLH_ZO(
          glm::radians(component_data_.fov[data_index]),
          component_data_.projection_size[data_index].x,
          component_data_.projection_size[data_index].y,
          component_data_.near_z[data_index],
          component_data_.far_z[data_index]);
        break;
      case ProjectionMode::kOrthographic:
        component_data_.projection_matrix[data_index] = glm::orthoLH_ZO(
          -component_data_.orthographic_size[data_index].x,
          component_data_.orthographic_size[data_index].x,
          -component_data_.orthographic_size[data_index].y,
          component_data_.orthographic_size[data_index].y,
          component_data_.near_z[data_index],
          component_data_.far_z[data_index]);
        component_data_.projection_size[data_index] = 
          component_data_.orthographic_size[data_index] * 2.0f;
        break;
      case ProjectionMode::kCanvas:
        component_data_.projection_matrix[data_index] = glm::orthoLH_ZO(
          .0f,
          component_data_.projection_size[data_index].x,
          component_data_.projection_size[data_index].y,
          .0f,
          component_data_.near_z[data_index],
          component_data_.far_z[data_index]);
        break;
      }

      component_data_.depth_buffer[data_index] = DepthBuffer(
        component_data_.projection_size[data_index],
        component_data_.depth_buffer[data_index].format());

      component_data_.invalid_projection[data_index] = false;
      component_data_.invalid_view_projection[data_index] = false;
    }

    //-------------------------------------------------------------------------
    void CameraSystem::UpdateViewProjection(CameraComponent handle)
    {
      size_t data_index = component_data_.data.GetDataIndex(handle);

      UpdateProjection(handle); // First check if it needs to update the projection matrix as this triggers the viewProj matrix to update as well
      if (!component_data_.invalid_view_projection[data_index])// && !transform.hasChanged)
      {
        return;
      }

      // TODO: add transform invalidation check
      glm::mat4 view = GetViewMatrix(handle);

      // TODO: verify view projection matrix
      glm::mat4 view_projection = view * component_data_.projection_matrix[data_index];

      component_data_.inv_view_projection_matrix[data_index] = glm::inverse(view_projection);
      component_data_.view_projection_matrix[data_index] = view_projection;
      component_data_.invalid_view_projection[data_index] = false;
    }

    //-------------------------------------------------------------------------
    TransformComponent CameraComponent::GetTransform() const
    {
      return system_->GetTransform(*this);
    }
    TransformComponent CameraSystem::GetTransform(CameraComponent handle) const
    {
      return component_data_.data.Get<static_cast<size_t>(CameraComponentElements::kEntity)>
        (handle).Get<TransformComponent>();
    }

    //-------------------------------------------------------------------------
    foundation::Vector<CameraComponent> CameraSystem::GetCameras()
    {
      foundation::Vector<CameraComponent> list;
      for (int i = 0; i < component_data_.data.size(); ++i)
      {
        list.push_back(component_data_.entity[i].Get<CameraComponent>());// There is a better way but need to look into that 
      }
      return list;
    }

    //-------------------------------------------------------------------------
    RenderTarget CameraComponent::GetRenderTarget() const
    {
      return system_->GetRenderTarget(*this);
    }
    RenderTarget CameraSystem::GetRenderTarget(CameraComponent handle) const
    {
      return component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kRenderTarget)>(handle);
    }

    //-------------------------------------------------------------------------
    void CameraComponent::SetRenderTarget(const RenderTarget& target)
    {
      system_->SetRenderTarget(*this, target);
    }
    void CameraSystem::SetRenderTarget(CameraComponent handle, const RenderTarget& target)
    {
      component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kRenderTarget)>(handle) = target;
    }

    //-------------------------------------------------------------------------
    DepthBuffer CameraComponent::GetDepthBuffer() const
    {
      return system_->GetDepthBuffer(*this);
    }
    DepthBuffer CameraSystem::GetDepthBuffer(CameraComponent handle) const
    {
      return component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kDepthBuffer)>(handle);
    }

    //-------------------------------------------------------------------------
    void CameraComponent::SetDepthBuffer(const DepthBuffer& buffer)
    {
      system_->SetDepthBuffer(*this, buffer);
    }
    void CameraSystem::SetDepthBuffer(CameraComponent handle, const DepthBuffer& buffer)
    {
      component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kDepthBuffer)>(handle) = buffer;
    }

    //-------------------------------------------------------------------------
    LayerMask CameraComponent::GetLayerMask() const
    {
      return system_->GetLayerMask(*this);
    }
    LayerMask CameraSystem::GetLayerMask(CameraComponent handle) const
    {
      return component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kLayerMask)>(handle);
    }

    //-------------------------------------------------------------------------
    void CameraComponent::SetLayerMask(const LayerMask& layer_mask)
    {
      system_->SetLayerMask(*this, layer_mask);
    }
    void CameraSystem::SetLayerMask(CameraComponent handle, const LayerMask& layer_mask)
    {
      component_data_.data.Get<
        static_cast<size_t>(CameraComponentElements::kLayerMask)>(handle) = layer_mask;
    }

    //-------------------------------------------------------------------------
    CameraComponent CameraComponent::main_camera()
    {
      return system_->main_camera();
    }

    //-------------------------------------------------------------------------
    CameraComponent CameraComponent::current_camera()
    {
      return system_->current_camera();
    }

    //-------------------------------------------------------------------------
    void CameraComponent::make_main_camera()
    {
      system_->set_main_camera(*this);
    }
  }
}