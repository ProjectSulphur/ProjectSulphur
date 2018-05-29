#pragma once

#include "engine/core/editor/gizmo_base.h"
#include "engine/assets/mesh.h"

namespace sulphur
{
  namespace engine
  {
    /**
    *@class sulphur::engine::TranslateGizmo : sulphur::engine::IGizmo
    *@brief Gizmo responsible for modifying the position of the entities it's attached to.
    *@author Stan Pepels
    */
    class TranslateGizmo : public GizmoBase
    {
    public:
      /**
      *@brief Initializes the sizes of the different parts of the gizmos render mesh.
      */
      TranslateGizmo();

      /**
      *@see sulphur::engine::IGizmo::~TranslateGizmo
      */
      virtual ~TranslateGizmo() final {};

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
      *@see sulphur::engine::IGizmo::Draw
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
        kXAxisSelected, //!< Construct mesh with x axis highlighted.
        kYAxisSelected, //!< Construct mesh with y axis highlighted.
        kZAxisSelected, //!< Construct mesh with z axis highlighted.
        kXYPlaneSelected, //!< Construct mesh with xy plane highlighted.
        kXZPlaneSelected, //!< Construct mesh with xz plane highlighted.
        kZYPlaneSelected,  //!< Construct mesh with zy plane highlighted.
        kUnused //!< Render the gizmo without anything highlighted.
      };

      /**
      *@brief Constructs the mesh in accordance with the current display mode.
      */
      void ConstructMesh();

      /**
      *@brief Checks against the bounding boxes of the axis and sets the translate axis.
      *@param[in] camera_ray (const foundation::Ray&) Ray from the camera onto the world.
      *@return (bool) True if any axis got hit, false otherwise.
      */
      bool SelectAxis(const foundation::Ray& camera_ray);

      /**
      *@brief Checks against the translation planes and sets the translation plane normal.
      *@param[in] camera_ray (const foundation::Ray&) Ray from the camera onto the world.
      *@return (bool) True if any plane got hit, false otherwise.
      */
      bool SelectPlane(const foundation::Ray& camera_ray);

      /**
      *@brief Enum for changeing the way attached objects are translated.
      */
      enum struct TranslationMode 
      {
        kAxis, //!< Move along an axis.
        kPlane //!< Move on a plane.
      };

      MeshHandle gizmo_mesh_; //!< Mesh used for rendering the gizmo.
      Mesh arrow_mesh_; //!< Mesh used for representing a single axis.
      Mesh plane_mesh_; //!< Mesh used to render the translation planes of the gizmos.

      glm::vec3 accumulated_translation_;

      DisplayMode display_mode_; //!< Indicates how the gizmo should be displayed. i.e. what part should be highlighted.
      TranslationMode translation_mode_; //!< Translate along axis or plane
      glm::vec3 translate_axis_; //!< Axis on which to move the object.
      glm::vec3 translation_plane_normal_; //!< Normal of the plane we are translating the gizmo on.
      glm::vec3 start_pos_; //!< Position the manipulation started.
      glm::vec3 previous_intersect_; //!< Depending on the moving mode. This stores the intersection with the plane or axis from the previous frame.
      const float kArrowLength_; //!< Length of the arrow minus the arrow tip.
      const float kArrowRadius_; //!< Radius of the arrow.
      const float kArrowTipRadius_; //!< Radius of the arrow tip.
      const float kArrowTipLength_; //!< Lenght of the arrow tip.
      const float kPlaneSize_; //!< Size of the planes.
      const float kAABBLength_; //!< Lenght of the AABB.
      const float kAABBHalfSize_; //!< Width and height of the AABB.
    };
  }
}