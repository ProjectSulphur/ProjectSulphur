#pragma once

#include "engine/core/entity_system.h"
#include "engine/application/window.h"
#include "engine/systems/component_system.h"

#include <foundation/utils/color.h>
#include "engine/utilities/layer.h"

#include "engine/assets/render_target.h"
#include "engine/assets/depth_buffer.h"
#include "engine/assets/post_process_material.h"
#include "engine/graphics/irenderer.h"
#include "engine/graphics/g_buffer.h"
#include "engine/scripting/scriptable_object.h"

#include <foundation/containers/array.h>
#include <foundation/containers/vector.h>

namespace sulphur
{
  namespace engine
  {
    class Entity;
    class CameraSystem;
    class TransformComponent;

    SCRIPT_CLASS() class CameraEnums : public ScriptableObject
    {

    public:

      SCRIPT_NAME(CameraEnums);

      /**
      * @enum sulphur::engine::ProjectionMode
      * @brief Describes the how the camera is projected
      */
      SCRIPT_ENUM() enum ProjectionMode
      {
        kPerspective,   //!< The camera is projected in perspective mode
        kOrthographic,  //!< The camera is projected in orthographic mode
        kCanvas         //!< The camera is projected in orthographic mode, but with the top left set to zero and the bottom right set to the screen size
      };

      /**
      * @enum sulphur::engine::ClearMode
      * @brief Describes the how the camera is cleared
      */
      SCRIPT_ENUM() enum ClearMode
      {
        kNone,          //!< The camera will not be cleared
        kDepthOnly,     //!< The camera will only clear it's depth buffer
        kColor,         //!< The camera will clear it's render targets to a defined background color and the depth buffer
        kSky            //!< The camera will render a sky in the background
      };
    };

    /**
    * @brief An enumerator that is an index of the element. Should be in sync with @see sulphur::engine::CameraData::ComponentSystemData
    */
    enum struct CameraComponentElements
    {
      kProjectMode, //!< The projection mode index
      kClearMode, //!< The clear mode index
      kClearColor, //!< The clear color index
      kRenderTarget,  //!< The render target index
      kDepthBuffer,  //!< The depth buffer index
      kPostProcessMaterial, //!< The post process material index
      kLayerMask, //!< The layer mask index
      kNearZ, //!< The near Z index
      kFarZ, //!< The far Z index
      kOrthographicSize, //!< The orthographic size index
      kFov, //!< The field of view index
      kProjectionSize, //!< The projection size index
      kInvalidProjection, //!< The invalid projection index
      kProjectMatrix, //!< The project matrix index
      kInvalidViewProjection, //!< The invalid view projection index
      kViewProjectionMatrix, //!< The view projection matrix index
      kInvViewProjectionMatrix, //!< The inverse view projection matrix index
      kFrustum, //!< The frustum, index
      kEntity //!< The entity index
    };
    
    /**
    * @brief The data of the camera component.
    * @author Raymi Klingers
    */
    class CameraData
    {
    public:
      /**
      * @brief Alias of the system data. The order of the member variables should be in sync with @see sulphur::engine::CameraComponentElements and the pointers in this
      */
      using ComponentSystemData = SystemData<
        CameraEnums::ProjectionMode,
        CameraEnums::ClearMode,
        foundation::Color,
        RenderTarget,
        DepthBuffer,
        PostProcessMaterialHandle,
        LayerMask,
        float,
        float,
        glm::vec2,
        float,
        glm::vec2,
        bool,
        glm::mat4,
        bool,
        glm::mat4,
        glm::mat4,
        foundation::Frustum,
        Entity>;

      /**
      * @brief Constructor of the camera that initializes the system data by passing it the list of pointers. It uses the first element for this and assumes that the others follow
      */
      CameraData()
        :
        data((void**)&projection_mode)
      {
      }

      CameraEnums::ProjectionMode* projection_mode;//!< Simple direct access to the projection_mode array data.
      CameraEnums::ClearMode* clear_mode;//!< Simple direct access to the intensity clear_mode data.
      foundation::Color* clear_color;//!< Simple direct access to the clear_color array data.
      RenderTarget* render_target;//!< Simple direct access to the render_target array data.
      DepthBuffer* depth_buffer;//!< Simple direct access to the depth_buffer array data.
      PostProcessMaterialHandle* post_process_material;//!< Simple direct access to the render_target array data.
      LayerMask* layer_mask;//!< Simple direct access to the layer_mask array data.
      float* near_z;//!< Simple direct access to the near_z array data.
      float* far_z;//!< Simple direct access to the far_z array data.
      glm::vec2* orthographic_size;//!< Simple direct access to the orthographic_size array data.
      float* fov;//!< Simple direct access to the fov array data.
      glm::vec2* projection_size;//!< Simple direct access to the projection_size array data.
      bool* invalid_projection;//!< Simple direct access to the invalid_projection array data.
      glm::mat4* projection_matrix;//!< Simple direct access to the projection_matrix array data.
      bool* invalid_view_projection;//!< Simple direct access to the invalid_view_projection array data.
      glm::mat4* view_projection_matrix;//!< Simple direct access to the view_projection_matrix array data.
      glm::mat4* inv_view_projection_matrix;//!< Simple direct access to the inv_view_projection_matrix array data.
      foundation::Frustum* frustum;//!< Simple direct access to the frustum array data.
      Entity* entity;//!< Simple direct access to the entity array data.
      ComponentSystemData data;//!< System data of the component.
    };

    /**
    * @class sulphur::engine::CameraComponent
    * @brief The component type used by the CameraSystem to modify a camera
    * @author Jelle de Haan
    */
    SCRIPT_CLASS() class CameraComponent : public ComponentHandleBase
    {
    public:

      SCRIPT_NAME(CameraComponent);
      SCRIPT_COMPONENT();

      using System = CameraSystem; //!< System type define

      /**
      * @brief Default constructor (creates an empty/invalid handle)
      */
      CameraComponent();
      /**
      * @brief Constructor for creating a handle from an integral value
      * @param[in] system (sulphur::engine::CameraComponent::System&) A reference to the relevant system that owns this component
      * @param[in] handle (size_t) The integral value to use for constructing the handle
      */
      CameraComponent(System& system, size_t handle);

      /**
      * @brief Sets the value that represents the near plane of this camera. Everything in front of the near plane will be clipped.
      * @param[in] near_z (float) The value to set the near plane to
      */
      SCRIPT_FUNC() void SetNearZ(float near_z);

      /**
      * @brief Returns the value that represents the near plane of this camera.
      * @return (float) The value to near plane is set to
      */
      SCRIPT_FUNC() float GetNearZ() const;

      /**
      * @brief Sets the value that represents the far plane of this camera. Everything behind the far plane will be clipped.
      * @param[in] far_z (float) The value to set the far plane to
      */
      SCRIPT_FUNC() void SetFarZ(float far_z);

      /**
      * @brief Returns the value that represents the far plane of this camera.
      * @return (float) The value to far plane is set to
      */
      SCRIPT_FUNC() float GetFarZ() const;

      /**
      * @brief Set the field of view of the camera.
      * @param[in] fov (float) The value to set the field of view to
      * @remarks Value in degrees
      * @remarks This is ignored when the camera is not perspective.
      */
      SCRIPT_FUNC() void SetFieldOfView(float fov);

      /**
      * @brief Get the field of view of the camera.
      * @return (float) The value that represents the field of view of the camera
      * @remarks Value in degrees
      * @remarks This is ignored when the camera is not perspective.
      */
      SCRIPT_FUNC() float GetFieldOfView() const;

      /**
      * @brief Set the orthographic half size of the camera.
      * @param[in] size (glm::vec2) The value to set the orthographic half size to
      * @remarks This is ignored when the camera is not orthographic.
      */
      SCRIPT_FUNC() void SetOrthographicSize(glm::vec2 size);

      /**
      * @brief Get the orthographic half size of the camera.
      * @return (glm::vec2) The value to that represents the orthographic half size of the camera
      * @remarks This is ignored when the camera is not orthographic.
      */
      SCRIPT_FUNC() glm::vec2 GetOrthographicSize() const;

      /**
      * @brief Set the clear mode of the camera.
      * @param[in] clear_mode (sulphur::engine::ClearMode) The new clear mode of the camera
      */
      SCRIPT_FUNC() void SetClearMode(const CameraEnums::ClearMode& clear_mode);

      /**
      * @brief Returns the clear mode that the camera currently uses
      * @return (sulphur::engine::ClearMode) The clear mode of the camera
      */
      SCRIPT_FUNC() CameraEnums::ClearMode GetClearMode() const;

      /**
      * @brief Set the clear color of the camera.
      * @param[in] clear_color (sulphur::foundation::Color) The new clear color of the camera
      */
      void SetClearColor(const foundation::Color& clear_color);

      /**
      * @brief Returns the clear color that the camera currently uses
      * @return (Color) The clear color of the camera
      */
      foundation::Color GetClearColor() const;

      /**
      * @brief Set the projection mode of the camera.
      * @param[in] projection (sulphur::engine::ProjectionMode) The new projection mode of the camera
      */
      SCRIPT_FUNC() void SetProjectionMode(const CameraEnums::ProjectionMode& projection);

      /**
      * @brief Returns projection the mode that the camera currently uses
      * @return (sulphur::engine::ProjectionMode) The projection mode of the camera
      */
      SCRIPT_FUNC() CameraEnums::ProjectionMode GetProjectionMode() const;

      /**
      * @brief Set the 'screen' size of the camera projection
      * @param[in] size (const glm::vec2&) The new projection size of the camera
      * @remarks This is ignored when the camera is not perspective.
      */
      SCRIPT_FUNC() void SetProjectionSize(const glm::vec2& size);

      /**
      * @brief Get the 'screen' size of the camera projection
      * @return (glm::vec2) The projection size of the camera
      * @remarks This is ignored when the camera is not perspective.
      */
      SCRIPT_FUNC() glm::vec2 GetProjectionSize() const;

      /**
      * @brief Get the view matrix of the camera
      * @return (glm::mat4) The view matrix of the camera
      */
      glm::mat4 GetViewMatrix() const;

      /**
      * @brief Get the projection matrix of the camera
      * @return (const glm::mat4&) The projection matrix of the camera
      */
      const glm::mat4& GetProjectionMatrix();

      /**
      * @brief Get the combined view and projection matrix of the camera
      * @return (const glm::mat4&) The view-projection matrix of the camera
      */
      const glm::mat4& GetViewProjectionMatrix();

      /**
      * @brief Get the inverse of the combined view and projection matrix of the camera
      * @return (const glm::mat4&) The inverse view-projection matrix of the camera
      */
      const glm::mat4& GetInverseViewProjectionMatrix();

      /**
      * @brief Converts a point in screen space to a point in view space
      * @param[in] point (const glm::vec2&) The point in screen space
      * @return (glm::vec2) The point in view space
      */
      SCRIPT_FUNC() glm::vec2 ScreenToViewportPoint(const glm::vec2& point) const;

      /**
      * @brief Converts a point in screen space to a point in world space
      * @param[in] point (const glm::vec3&) The point in screen space
      * @return (glm::vec3) The point in view space
      * @remarks The z component is in world units from the camera. For best results, set to the value of the near plane.
      */
      SCRIPT_FUNC() glm::vec3 ScreenToWorldPoint(const glm::vec3& point);

      /**
      * @brief Converts a point in world space to a point in view space
      * @param[in] point (const glm::vec3&) The point in world space
      * @return (glm::vec3) The point in view space
      */
      SCRIPT_FUNC() glm::vec3 WorldToViewportPoint(const glm::vec3& point);

      /**
      * @brief Converts a point in world space to a point in screen space
      * @param[in] point (const glm::vec3&) The point in world space
      * @return (glm::vec3) The point in screen space
      * @remarks The z component is in world units from the camera.
      */
      SCRIPT_FUNC() glm::vec3 WorldToScreenPoint(const glm::vec3& point);

      /**
      * @brief Converts a point in view space to a point in screen space
      * @param[in] point (const glm::vec3&) The point in view space
      * @return (glm::vec3) The point in screen space
      * @remarks The z component is in world units from the camera. For best results, set to the value of the near plane.
      */
      SCRIPT_FUNC() glm::vec3 ViewportToScreenPoint(const glm::vec3& point) const;

      /**
      * @brief Converts a point in view space to a point in world space
      * @param[in] point (const glm::vec3&) The point in view space
      * @return (glm::vec3) The point in world space
      * @remarks The z component is in world units from the camera. For best results, set to the value of the near plane.
      */
      SCRIPT_FUNC() glm::vec3 ViewportToWorldPoint(const glm::vec3& point);

      /**
      * @brief Get the frustum of the camera
      * @return (const sulphur::foundation::Frustum&) The frustum of this camera
      */
      const foundation::Frustum& GetFrustum();

      /*
      * @brief Get the post process material attached to this camera
      * @return (sulphur::engine::PostProcessMaterialHandle) The post process material
      * @remarks This can be used to apply image effects on this camera's render target
      */
      PostProcessMaterialHandle GetPostProcessMaterial() const;

      /*
      * @brief Set the post process material attached to this camera
      * @param[in] material (sulphur::engine::PostProcessMaterialHandle) The post process material
      * @remarks This can be used to apply image effects on this camera's render target
      */
      void SetPostProcessMaterial(PostProcessMaterialHandle material);

      /*
      * @brief Get the render target attached to this camera
      * @return (sulphur::engine::RenderTarget) The render target
      */
      RenderTarget GetRenderTarget() const;

      /*
      * @brief Set the render target attached to this camera
      * @param[in] target (const sulphur::engine::RenderTarget&) The render target
      */
      void SetRenderTarget(const RenderTarget& target);

      /*
      * @brief Get the depth buffer attached to this camera
      * @return (sulphur::engine::DepthBuffer) The depth buffer
      */
      DepthBuffer GetDepthBuffer() const;

      /*
      * @brief Set the depth buffer attached to this camera
      * @param[in] buffer (const sulphur::engine::DepthBuffer&) The depth buffer
      */
      void SetDepthBuffer(const DepthBuffer& buffer);

      /*
      * @brief Get the layer mask attached to this camera
      * @return (sulphur::engine::LayerMask) The layer mask
      */
      LayerMask GetLayerMask() const;

      /*
      * @brief Set the layer mask this camera should use
      * @param[in] layer_mask (const sulphur::engine::LayerMask&) The layer mask
      */
      void SetLayerMask(const LayerMask& layer_mask);

      /**
      * @brief Get the transform component attached to this camera
      * @return (sulphur::engine::TransformComponent) The transform component
      */
      TransformComponent GetTransform() const;

      /**
      * @brief Get camera component that is currently set as the main camera
      * @return (sulphur::engine::CameraComponent) The main camera
      */
      SCRIPT_FUNC() CameraComponent main_camera();

      /**
      * @brief Get camera component that is set as the current camera
      * @return (sulphur::engine::CameraComponent) The current camera
      */
      SCRIPT_FUNC() CameraComponent current_camera();

      /**
      * @brief Make this camera the main camera
      */
      void make_main_camera();

    private:
      CameraSystem* system_;

    };

    class CameraSystem : public IComponentSystem
    {
    public:
      /**
      * @brief Default constructor
      */
      CameraSystem();

      /**
      * @see sulphur::engine::IComponentSystem::Create
      */
      template<typename ComponentT>
      ComponentT Create(Entity& entity) { return Create(entity); };

      /**
      * @brief Destroys the camera component
      * @param[in] handle (sulphur::engine::CameraComponent) The camera to destroy
      * @see sulphur::engine::IComponentSystem::Destroy
      */
      void Destroy(ComponentHandleBase handle) override;

      /**
      * @brief Create a new camera component for this entity and also creates a TransformComponent if it wasn't attached yet
      * @param[in] entity (sulphur::engine::Entity) The entity to create this camera for
      * @see sulphur::engine::IComponentSystem::Create
      */
      CameraComponent Create(Entity& entity);
      
      /**
      * @internal
      * @see sulphur::engine::IComponentSystem::OnInitialize
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;

      /**
      * @internal
      * @see sulphur::engine::IComponentSystem::OnTerminate
      */
      void OnTerminate() override;

      /**
      * @brief Sets the value that represents the near plane of this camera. Everything in front of the near plane will be clipped.
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to set the value on
      * @param[in] near_z (float) The value to set the near plane to
      */
      void SetNearZ(CameraComponent handle, float nearZ);

      /**
      * @brief Returns the value that represents the near plane of this camera.
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @return (float) The value to near plane is set to
      */
      float GetNearZ(CameraComponent handle) const;

      /**
      * @brief Sets the value that represents the far plane of this camera. Everything behind the far plane will be clipped.
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to set the value on
      * @param[in] far_z (float) The value to set the far plane to
      */
      void SetFarZ(CameraComponent handle, float farZ);

      /**
      * @brief Returns the value that represents the far plane of this camera.
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @return (float) The value to far plane is set to
      */
      float GetFarZ(CameraComponent handle) const;

      /**
      * @brief Set the field of view of the camera.
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to set the value on
      * @param[in] fov (float) The value to set the field of view to
      * @remarks Value in degrees
      * @remarks This is ignored when the camera is not perspective.
      */
      void SetFieldOfView(CameraComponent handle, float fov);

      /**
      * @brief Get the field of view of the camera.
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @return (float) The value that represents the field of view of the camera
      * @remarks Value in degrees
      * @remarks This is ignored when the camera is not perspective.
      */
      float GetFieldOfView(CameraComponent handle) const;

      /**
      * @brief Set the orthographic half size of the camera.
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to set the value on
      * @param[in] size (glm::vec2) The value to set the orthographic half size to
      * @remarks This is ignored when the camera is not orthographic.
      */
      void SetOrthographicSize(CameraComponent handle, glm::vec2 orthographicSize);

      /**
      * @brief Get the orthographic half size of the camera.
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @return (glm::vec2) The value to that represents the orthographic half size of the camera
      * @remarks This is ignored when the camera is not orthographic.
      */
      glm::vec2 GetOrthographicSize(CameraComponent handle) const;

      /**
      * @brief Set the clear mode of the camera.
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to set the value on
      * @param[in] clear_mode (sulphur::engine::ClearMode) The new clear mode of the camera
      */
      void SetClearMode(CameraComponent handle, const CameraEnums::ClearMode& clear_mode);

      /**
      * @brief Returns the clear mode that the camera currently uses
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @return (sulphur::engine::ClearMode) The clear mode of the camera
      */
      CameraEnums::ClearMode GetClearMode(CameraComponent handle) const;

      /**
      * @brief Set the clear color of the camera.
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to set the value on
      * @param[in] clear_color (sulphur::foundation::Color) The new clear color of the camera
      */
      void SetClearColor(CameraComponent handle, const foundation::Color& clear_color);

      /**
      * @brief Returns the clear color that the camera currently uses
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @return (sulphur::foundation::Color) The clear mode of the camera
      */
      foundation::Color GetClearColor(CameraComponent handle) const;

      /**
      * @brief Set the projection mode of the camera.
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to set the value on
      * @param[in] projection (sulphur::engine::ProjectionMode) The new projection mode of the camera
      */
      void SetProjectionMode(CameraComponent handle, const CameraEnums::ProjectionMode& projection);

      /**
      * @brief Returns the projection mode that the camera currently uses
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @return (sulphur::engine::ProjectionMode) The projection mode of the camera
      */
      CameraEnums::ProjectionMode GetProjectionMode(CameraComponent handle) const;

      /**
      * @brief Set the 'screen' size of the camera projection
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to set the value on
      * @param[in] size (const glm::vec2&) The new projection size of the camera
      * @remarks This is ignored when the camera is not perspective.
      */
      void SetProjectionSize(CameraComponent handle, const glm::vec2& projectionSize);

      /**
      * @brief Get the 'screen' size of the camera projection
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @return (glm::vec2) The projection size of the camera
      * @remarks This is ignored when the camera is not perspective.
      */
      glm::vec2 GetProjectionSize(CameraComponent handle) const;

      /**
      * @brief Get the view matrix of the camera
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @return (glm::mat4) The view matrix of the camera
      */
      glm::mat4 GetViewMatrix(CameraComponent handle) const;

      /**
      * @brief Get the projection matrix of the camera
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @return (const glm::mat4&) The projection matrix of the camera
      */
      const glm::mat4& GetProjectionMatrix(CameraComponent handle);

      /**
      * @brief Get the combined view and projection matrix of the camera
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @return (const glm::mat4&) The view-projection matrix of the camera
      */
      const glm::mat4& GetViewProjectionMatrix(CameraComponent handle);

      /**
      * @brief Get the inverse of the combined view and projection matrix of the camera
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @return (const glm::mat4&) The inverse view-projection matrix of the camera
      */
      const glm::mat4& GetInverseViewProjectionMatrix(CameraComponent handle);

      /**
      * @brief Converts a point in screen space to a point in view space
      * @param[in] handle (Csulphur::engine::ameraComponent) The camera handle to get the value from
      * @param[in] point (const glm::vec2&) The point in screen space
      * @return (glm::vec2) The point in view space
      */
      glm::vec2 ScreenToViewportPoint(CameraComponent handle, const glm::vec2& point) const;

      /**
      * @brief Converts a point in screen space to a point in world space
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @param[in] point (const glm::vec3&) The point in screen space
      * @return (glm::vec3) The point in view space
      * @remarks The z component is in world units from the camera. For best results, set to the value of the near plane.
      */
      glm::vec3 ScreenToWorldPoint(CameraComponent handle, const glm::vec3& point);

      /**
      * @brief Converts a point in world space to a point in view space
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @param[in] point (const glm::vec3&) The point in world space
      * @return (glm::vec3) The point in view space
      */
      glm::vec3 WorldToViewportPoint(CameraComponent handle, const glm::vec3& point);

      /**
      * @brief Converts a point in world space to a point in screen space
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @param[in] point (const glm::vec3&) The point in world space
      * @return (glm::vec3) The point in screen space
      * @remarks The z component is in world units from the camera.
      */
      glm::vec3 WorldToScreenPoint(CameraComponent handle, const glm::vec3& point);

      /**
      * @brief Converts a point in view space to a point in screen space
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @param[in] point (const glm::vec3&) The point in view space
      * @return (glm::vec3) The point in screen space
      * @remarks The z component is in world units from the camera. For best results, set to the value of the near plane.
      */
      glm::vec3 ViewportToScreenPoint(CameraComponent handle, const glm::vec3& point) const;

      /**
      * @brief Converts a point in view space to a point in world space
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @param[in] point (const glm::vec3&) The point in view space
      * @return (glm::vec3) The point in world space
      * @remarks The z component is in world units from the camera. For best results, set to the value of the near plane.
      */
      glm::vec3 ViewportToWorldPoint(CameraComponent handle, const glm::vec3& point);

      /**
      * @brief Get the frustum of the camera
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @return (const sulphur::foundation::Frustum&) The frustum of this camera
      */
      const foundation::Frustum& GetFrustum(CameraComponent handle);

      /*
      * @brief Get the post process material attached to this camera
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @return (sulphur::engine::RenderTarget) The post process material
      * @remarks This can be used to apply image effects on this camera's render target
      */
      PostProcessMaterialHandle GetPostProcessMaterial(CameraComponent handle) const;

      /*
      * @brief Set the post process material for this camera
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @param[in] material (sulphur::engine::PostProcessMaterialHandle) The new post process material
      * @remarks This can be used to apply image effects on this camera's render target
      */
      void SetPostProcessMaterial(CameraComponent handle, PostProcessMaterialHandle material);

      /*
      * @brief Get the render target attached to this camera
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @return (sulphur::engine::RenderTarget) The render target
      */
      RenderTarget GetRenderTarget(CameraComponent handle) const;

      /*
      * @brief Set the render target attached to this camera
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to set the value for
      * @param[in] target (const sulphur::engine::RenderTarget&) The render target that this camera should use.
      */
      void SetRenderTarget(CameraComponent handle, const RenderTarget& target);

      /*
      * @brief Get the depth buffer attached to this camera
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @return (sulphur::engine::DepthBuffer) The depth buffer
      */
      DepthBuffer GetDepthBuffer(CameraComponent handle) const;

      /*
      * @brief Set the depth buffer attached to this camera
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to set the value on
      * @param[in] buffer (const sulphur::engine::DepthBuffer&) The depth buffer
      */
      void SetDepthBuffer(CameraComponent handle, const DepthBuffer& buffer);

      /*
      * @brief Get the layer mask attached to this camera
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @return (sulphur::engine::LayerMask) The layer mask
      * @see sulphur::engine::LayerMask
      */
      LayerMask GetLayerMask(CameraComponent handle) const;

      /*
      * @brief Set the layer mask this camera should use
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to set the value on
      * @param[in] layer_mask (const sulphur::engine::LayerMask&) The layer mask
      * @see sulphur::engine::LayerMask
      */
      void SetLayerMask(CameraComponent handle, const LayerMask& layer_mask);

      /**
      * @brief Get the transform component attached to this camera
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to get the value from
      * @return (sulphur::engine::TransformComponent) The transform component
      * @see sulphur::engine::TransformComponent
      */
      TransformComponent GetTransform(CameraComponent handle) const;

      /**
      * @brief Creates and returns a list of all cameras managed by this systems
      * @return (sulphur::foundation::Vector<CameraComponent>) A list of all cameras
      */
      foundation::Vector<CameraComponent> GetCameras();

      /**
      * @brief Get camera component that is currently set as the main camera
      * @return (sulphur::engine::CameraComponent) The main camera
      */
      CameraComponent main_camera() { return main_camera_; };

      /**
      * @brief Get camera component that is set as the current camera
      * @return (sulphur::engine::CameraComponent) The current camera
      */
      CameraComponent current_camera() { return current_camera_; };
      
      /**
      * @brief Makes 'camera' the main camera. The main camera is rendered to the screen
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to set as the main camera
      */
      void set_main_camera(const CameraComponent& camera) { main_camera_ = camera; };

      /**
      * @internal
      * @brief Makes 'camera' the current camera. The current camera is the camera currently used by the systems for rendering objects
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to set as the current camera
      * @remarks This should only be set by a systems that renders trough a camera and only in a loop
      */
      void set_current_camera(const CameraComponent& camera) { current_camera_ = camera; };

    private:
      /**
      * @internal
      * @brief Updates the projection matrix of the camera
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to update
      */
      void UpdateProjection(CameraComponent handle);

      /**
      * @internal
      * @brief Updates the view-projection matrix of the camera
      * @param[in] handle (sulphur::engine::CameraComponent) The camera handle to update
      */
      void UpdateViewProjection(CameraComponent handle);

      CameraComponent main_camera_, current_camera_;

      IRenderer* renderer_; //!< A pointer to the platform-renderer instance
      Window* window_; //!< A pointer to the platform-window instance

      GBuffer g_buffer_; //!< The render target used as a back buffer
      DepthBuffer g_depth_buffer_; //!< The depth buffer used as a back buffer

      CameraData component_data_; //!< An instance of the container that stores per-component data
    };
  }
}