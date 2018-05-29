#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <foundation/containers/vector.h>

namespace sulphur
{
  namespace foundation
  {
    struct Ray;
  }
  namespace engine
  {
    namespace editor
    {
      class EditorMessagingSystem;
    }

    class CameraComponent;
    class TransformComponent;
    class Entity;

    /**
    *@class sulphur::engine::IGizmo
    *@brief base class for implementing gizmos
    *@author Stan Pepels
    */
    class GizmoBase
    {
    public:
      /**
      *@brief draws the actual gizmo by first calculating the scale according to the camera position
      *@param[in] attached_entities (const sulphur::foundation::Vector<TransformComponent&>&) entities this gizmo is shared with.
      * in most cases the gizmo belongs to only 1 entity with exception of the transform gizmos which can modifie multiple entities at once
      * this parameter is passed to the sulphur::engine::IGizmo::Draw function
      *@param[in] camera (sulphr::engine::Entity*) camera containing the current render target. this camera is used to calculate the scale required for the gizmo to maintain an independant size
      */
      void DrawGizmo(foundation::Vector<TransformComponent>& attached_entities,
        CameraComponent camera);
      
      /**
      *@brief Initializes the gizmo. this is where you should construct your meshes for rendering etc.
      */
      virtual void Initialize() = 0;

      /**
      *@brief checks if the gizmo is selected by the cursor
      *@param[in] screen_to_world_ray (const foundation::Ray&) ray from the camera into the world
      *return (bool) true if ray hits the gizmo else false is returned
      */
      virtual bool Select(const foundation::Ray& camera_ray) = 0;

      /**
      *@brief release resources allocated in the Initialize function
      */
      virtual void Release() = 0;

      /**
      *@brief gets the current size all gizmos are rendered at
      *@return (float) size of the gizmos
      */
      float size() const;

      /**
      *@brief sets the size all gizmos should be rendered at
      *@param[in] new_size (const float) size to render the gizmos at
      */
      void set_size(const float new_size);

      /**
      *@brief get bool indicating if the gizmo is currently active
      *@return (bool) true if active false if not active
      */
      bool active();

      /**
      *@brief get bool indicating if this gizmo is currently used to manipulate a transform
      *@return (bool) true if manipulation of a transform has started. false otherwise
      */
      bool is_manipulating();

      /**
      *@brief function that sets the gizmo up for manipulating a transform
      *@param[in] camera_ray (const foundation::Ray&) ray from the camera into the scene
      * @param[in] attached_entities (const sulphur::foundation::Vector<TransformComponent&>&) entities this gizmo is shared with.
      *@remark this function should set the is_manipulating_ bool member to true
      */
      virtual void ManipulationStart(const foundation::Ray& camera_ray, foundation::Vector<TransformComponent>& attached_entities) =0;

      /**
      *@brief function that manipulates the transforms of the entities attached to this gizmo according to speific logic
      *@param[in] attached_entities (sulphur::foundation::Vector<sulphur::engine::TransformComponent>&) entities attached to this gizmo. this parameter is passed in by the gizmo system.
      *@param[in] camera_ray (const foundation::Ray&) ray from the camera into the scene
      *@param[in] camera_transform (sulphur::engine::TransformComponent) transform of the camera used to create the camera_ray
      *@param[in] message_system (const sulphur::editor::MessagingSystem&) refrence to the messagging system used to send messages to the editor
      */
      virtual void ManipulateTransforms(foundation::Vector<TransformComponent>& attached_entities,
        const foundation::Ray& camera_ray,
        TransformComponent camera_transform,
        editor::EditorMessagingSystem& message_system) = 0;
      
      /**
      *@brief this function ends the transform manipulation
      *@remark this function should set the is_manipulating_ member to false
      */
      virtual void ManipulationEnd() =0;

      virtual void RequestChange(editor::EditorMessagingSystem&, foundation::Vector<TransformComponent>&) {};
    protected:
      /**
      *@brief constructor. defaults the size of the gizmo to 70.0f
      */
      GizmoBase();

      /**
      *@brief base function for drawing a gizmo. every gizmo has its own drawing logic. some might be meshes others might be billboards
      *@param[in] attached_entities (const sulphur::foundation::Vector<TransformComponent&>&) collection of entities that share this gizmo.
      * This parameter will in most cases have a size of 1 but in case of transform gizmos multiple entities can share a gizmo 
      */
      virtual void Draw(foundation::Vector<TransformComponent>& attached_entities) = 0;

      /**
      *@brief gets the scale that must be applied to keep a gizmo at a fixed size on screen
      *@return (float) scale value used to multiply with
      */
      float scale();

      /**
      *@brief indicate whether we are currently manipulating attached transforms
      *@param[in] value (bool) value to set to
      */
      void set_is_manipulating(bool value);

      /**
      *@brief get the current position of the gizmo
      *@return (glm::vec3) current position of the gizmo
      */
      glm::vec3 position();

      /**
      *@brief sets the position the gizmo should be drawn at
      *@param[in] position (const glm::vec3) new position of the gizmo
      */
      void set_position(const glm::vec3 position);

      const glm::quat kRot90X; //!< Quaterion representing a 90 degrees rotation along the X axis.
      const glm::quat kRot90Y; //!< Quaterion representing a 90 degrees rotation along the Y axis.
      const glm::quat kRot90Z; //!< Quaterion representing a 90 degrees rotation along the Z axis.
    private:

      /**
      *@brief claclulates the scaling rquired for the gizmo to maintain a constant size on the screen
      *@param[in] camera (sulphur::engine::CameraComponent&) camera component of the camera passed to the sulphur::engine::IGizmo::DrawGizmo function
      *@param[in] camera_transform (sulphur::engine::TransformComponent&) transform component of the camera passed to the sulphur::engine::IGizmo::DrawGizmo function
      *@return (float) scale value to apply to a gizmo
      */
      float CalculateScale(CameraComponent camera);
      const float kDefaultSize; //!< Default size of a gizmo when a project is created or the setting cannot be found.
      glm::vec3 position_; //!< position where the gizmo should be drawn at
      float size_; //!< size of the gizmo as it appears on the screen.
      float scale_; //!< scale the gizmo should be rendered at
      bool active_; //!< true if active. false otherwise. inactive gizos cannot manipulate transforms and are not drawn
      bool is_manipulating_; //!< true if interacted with. false otherwise.
    };
  }
}