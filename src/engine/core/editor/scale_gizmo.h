#pragma once

#include "engine/core/editor/gizmo_base.h"
#include "engine/assets/mesh.h"

namespace sulphur
{
  namespace engine
  {
    /**
    *@class sulphur::engine::ScaleGizmo : sulphur::engine::IGizmo
    *@brief Gizmo used for scaling objects.
    *@author Stan Pepels
    */
    class ScaleGizmo : public GizmoBase
    {
    public:
      /**
      *@brief constructor
      */
      ScaleGizmo();

      /**
      *@see sulphur::engine::IGizmo::~ScaleGizmo
      */
      virtual ~ScaleGizmo() final {};

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
      *@brief Enum for changing the way the gizmo is displayed.
      */
      enum struct DisplayMode
      {
        kXAxisSelected, //!< Construct mesh with x axis highlighted.
        kYAxisSelected, //!< Construct mesh with y axis highlighted.
        kZAxisSelected, //!< Construct mesh with z axis highlighted.
        kUniformSelected, //!< Construct mesh with center highlighted.
        kUnused //!< Display the gizmo regularly.
      };

      /**
      *@brief Enum for changeing the way attached objects are scaled.
      */
      enum struct ScaleMode
      {
        kAxis, //!< Scale along an axis.
        kUniform //!< Scale uniformly.
      };

      void ConstructMesh();

      /**
      * @see sulphur::engine::IGizmo::Draw
      */
      virtual void Draw(foundation::Vector<TransformComponent>& attached_entities) final;

      /**
      *@see sulphur::engine::IGizmo::Select
      */
      virtual bool Select(const foundation::Ray& camera_ray) final;

      glm::vec3 RotateVector(const glm::vec3& v, const glm::quat& q);

      
    private:
      void ConstructAxisMesh(glm::vec3& axis, Mesh& mesh);

      glm::quat rotation_;
      MeshHandle x_axis_; //!< Mesh used for rendering the x axis.
      MeshHandle y_axis_; //!< Mesh used for rendering the y axis.
      MeshHandle z_axis_; //!< Mesh used for rendering the z axis.
      MeshHandle uniform_axis_; //!< Mesh used for rendering the center.
      Mesh axis_mesh_; //!< Mesh representing an axis of the gizmo.
      ScaleMode scaling_mode_; //!< Current scaling mode used.
      glm::vec3 accumulated_scale_;
      float axis_scale_;       //!< Current scaling of the axis to be applied when rendering.
      glm::vec3 manipulation_start_; 
      glm::vec3 scaling_axis_; //!< Axis to scale the object on.
      glm::vec3 previous_intersect_; //!< Previous closest point on the current scaling axis.
      glm::vec3 start_pos_; //!< Position of the gizmo when the manipulation starts.
      glm::vec3 start_scale_; //!< Scale of the attahced object when the manipulation starts
      const float kBlockSize_; //!< Size of the block at the end of an axis mesh.
      const float kStemLength_; //!< Size of the stem of an axis mesh.
      const float kStemRadius_; //!< Radius of the stem of an axis mesh.
      const float kCenterSize_; //!< Size of the center block of the gizmo mesh.
      const float kAABBLength_; //!< Lenght of the AABB.
      const float kAABBHalfSize_; //!< Width and height of the AABB.
      DisplayMode display_mode_; //!< Indicates how the gizmo should be displayed. i.e. what part should be highlighted.
    };
  }
}