#pragma once

#include "engine/core/editor/gizmo_base.h"
#include "engine/assets/mesh.h"

namespace sulphur
{
  namespace engine
  {
    /**
    *@class sulphur::engine::RotateGizmo : sulphur::engine::IGizmo
    *@brief Gizmo used for manipulating rotations of attached objects.
    *@author Stan Pepels
    */
    class RotateGizmo : public GizmoBase
    {
    public:
      /**
      *@brief constructor
      */
      RotateGizmo();

      /**
      *@see sulphur::engine::IGizmo::~ScaleGizmo
      */
      virtual ~RotateGizmo() final {};

      /**
      *@see sulphur::engine::IGizmo::Initialize
      */
      virtual void Initialize() final;

      /**
      *@see sulphur::engine::IGizmo::Release
      */
      virtual void Release() final;

      /**
      *@see sulphur::engine::IGizmo::ManipulationStart
      */
      virtual void ManipulationStart(const foundation::Ray& camera_ray, 
                                     foundation::Vector<TransformComponent>& attached_entities) final;

      /**
      *@see sulphur::engine::IGizmo::ManipulateTransforms
      */
      virtual void ManipulateTransforms(foundation::Vector<TransformComponent>& attached_entities,
        const foundation::Ray& camera_ray,
        TransformComponent camera_transform,
        editor::EditorMessagingSystem& message_system) final;

      void RequestChange(editor::EditorMessagingSystem& system, foundation::Vector<TransformComponent>& attached) override;
      /**
      *@see sulphur::engine::IGizmo::ManipulationEnd
      */
      virtual void ManipulationEnd() final;
    protected:
      /**
      * @see see sulphur::engine::IGizmo::Draw
      */
      virtual void Draw(foundation::Vector<TransformComponent>& attached_entities) final;

      /**
      *@see sulphur::engine::IGizmo::Select
      */
      virtual bool Select(const foundation::Ray& camera_ray) final;

    private:
      /**
      *@brief Enum for changing the way the gizmo is displayed.
      */
      enum struct DisplayMode
      {
        kXAxisSelected, //!< Highlight the X rotation axis yellow.
        kYAxisSelected, //!< Highlight the Y rotation axis yellow.
        kZAxisSelected, //!< Highlight the Z rotation axis yellow.
        kFreeRotateSelected, //!< Display the gizmo regularly.
        kUnused //!< Display the gizmo regularly.
      };

      /**
      *@brief Enum for swapping between free rotate and axis rotate.
      */
      enum struct RotateMode
      {
        kAxis, //!< Rotate around the x,y or z axis.
        kFree //!< Rotate free along any axis.
      };

      void ConstructMesh();
      /**
      *@brief Rotate a vector with a quaternion.
      *@param[in] v (const glm::vec3&) Vector to rotate.
      *@param[in] q (const glm::quat&) Rotation to apply to the vector.
      *return (glm::vec3) Result of rotating vector v with quaternion q.
      */
      glm::vec3 RotateVector(const glm::vec3& v, const glm::quat& q);
      
      MeshHandle gizmo_mesh_; //!< Mesh used for rendering the gizmo.
      Mesh circle_mesh_; //!< Mesh of a single circle representing a rotation axis.

      glm::quat accumulated_rotation_;

      const float kRadius_; //!< Radius of the circles representing the different rotation axis.
      const float kSelectionThreshold_; //!< Threshold used for checking if an axis got selected. The bigger the threshold the less inacurate you have to click the gizmo.
      glm::vec3 previous_intersect_; //!< Previous closest intersection point with the gizmo.
      glm::vec3 rotation_axis_; //!< Axis to rotate around.
      glm::quat rotation_; //!< Rotation of the gizmo as is currently applied.

      glm::vec3 rotation_x_axis_; //!< Current x axis rotation.
      glm::vec3 rotation_y_axis_; //!< Current y axis rotation.
      glm::vec3 rotation_z_axis_; //!< Current z axis rotation.
      RotateMode rotate_mode_; //!< the way the gizmo manipulates the attached objects

      DisplayMode display_mode_; //!< Indicates how the gizmo should be displayed. i.e. what part should be highlighted.
    };
  }
}