#pragma once

#include "engine/systems/owner_system.h"
#include "engine/graphics/irenderer.h"

#include "engine/assets/mesh.h"
#include "engine/assets/material.h"

#include <foundation/containers/vector.h>

#include <glm/glm.hpp>

namespace sulphur 
{
  namespace engine 
  {
    class CameraSystem;

    /**
    * @class sulphur::engine::DebugRenderSystem
    * @brief Manages drawing of debug objects like gizmo's and lines
    * @author Jelle de Haan
    */
    class DebugRenderSystem : public IOwnerSystem<DebugRenderSystem>
    {
    public:
      /**
      * @brief Default constructor
      */
      DebugRenderSystem();

      static void SetupDebugAssets();

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
      * @brief Draw a debug line
      * @param[in] start (const glm::vec3&) The first point of the line
      * @param[in] end (const glm::vec3&) The second point of the line
      * @param[in] color (const foundation::Color&) The color of the line
      * @param[in] depth_test (bool) Should this line render on top of other objects
      * @param[in] lifetime (float) The amount of seconds this line should appear or zero if only once
      */
      static void DrawLine(const glm::vec3& start, 
        const glm::vec3& end,
        const foundation::Color& color = foundation::Color::kWhite,
        bool depth_test = true, 
        float lifetime = 0.0f);

      /**
      * @brief Draw a debug cone
      * @param[in] position (const glm::vec3&) The position the cone should appear at
      * @param[in] rotation (const glm::quat&) The rotation of the cone
      * @param[in] size (const glm::vec3&) The size of the cone
      * @param[in] color (const foundation::Color&) The color of the cone
      * @param[in] wire_frame (bool) Should this cone be rendered as a set of lines
      * @param[in] depth_test (bool) Should this cone render on top of other objects
      * @param[in] lifetime (float) The amount of seconds this cone should appear or zero if only once
      */
      static void DrawCone(const glm::vec3& position,
        const glm::quat& rotation = glm::quat(1, 0, 0, 0),
        const glm::vec3& size = glm::vec3(1, 1, 1),
        const foundation::Color& color = foundation::Color::kWhite,
        bool wire_frame = false,
        bool depth_test = true,
        float lifetime = 0.0f);

      /**
      * @brief Draw a debug cube
      * @param[in] position (const glm::vec3&) The position the cube should appear at
      * @param[in] rotation (const glm::quat&) The rotation of the cube
      * @param[in] size (const glm::vec3&) The size of the cube
      * @param[in] color (const foundation::Color&) The color of the cube
      * @param[in] wire_frame (bool) Should this cube be rendered as a set of lines
      * @param[in] depth_test (bool) Should this cube render on top of other objects
      * @param[in] lifetime (float) The amount of seconds this cube should appear or zero if only once
      */
      static void DrawCube(const glm::vec3& position,
        const glm::quat& rotation = glm::quat(1, 0, 0, 0),
        const glm::vec3& size = glm::vec3(1,1,1),
        const foundation::Color& color = foundation::Color::kWhite,
        bool wire_frame = false,
        bool depth_test = true,
        float lifetime = 0.0f);

      /**
      * @brief Draw a debug cylinder
      * @param[in] position (const glm::vec3&) The position the cylinder should appear at
      * @param[in] rotation (const glm::quat&) The rotation of the cylinder
      * @param[in] size (const glm::vec3&) The size of the cylinder
      * @param[in] color (const foundation::Color&) The color of the cylinder
      * @param[in] wire_frame (bool) Should this cylinder be rendered as a set of lines
      * @param[in] depth_test (bool) Should this cylinder render on top of other objects
      * @param[in] lifetime (float) The amount of seconds this cylinder should appear or zero if only once
      */
      static void DrawCylinder(const glm::vec3& position,
        const glm::quat& rotation = glm::quat(1, 0, 0, 0),
        const glm::vec3& size = glm::vec3(1, 1, 1),
        const foundation::Color& color = foundation::Color::kWhite,
        bool wire_frame = false,
        bool depth_test = true,
        float lifetime = 0.0f);

      /**
      * @brief Draw a debug plane
      * @param[in] position (const glm::vec3&) The position the plane should appear at
      * @param[in] rotation (const glm::quat&) The rotation of the plane
      * @param[in] size (const glm::vec3&) The size of the plane
      * @param[in] color (const foundation::Color&) The color of the plane
      * @param[in] wire_frame (bool) Should this plane be rendered as a set of lines
      * @param[in] depth_test (bool) Should this plane render on top of other objects
      * @param[in] lifetime (float) The amount of seconds this plane should appear or zero if only once
      */
      static void DrawPlane(const glm::vec3& position,
        const glm::quat& rotation = glm::quat(1, 0, 0, 0),
        const glm::vec3& size = glm::vec3(1, 1, 1),
        const foundation::Color& color = foundation::Color::kWhite,
        bool wire_frame = false,
        bool depth_test = true,
        float lifetime = 0.0f);

      /**
      * @brief Draw a debug pyramid
      * @param[in] position (const glm::vec3&) The position the pyramid should appear at
      * @param[in] rotation (const glm::quat&) The rotation of the pyramid
      * @param[in] size (const glm::vec3&) The size of the pyramid
      * @param[in] color (const foundation::Color&) The color of the pyramid
      * @param[in] wire_frame (bool) Should this pyramid be rendered as a set of lines
      * @param[in] depth_test (bool) Should this pyramid render on top of other objects
      * @param[in] lifetime (float) The amount of seconds this pyramid should appear or zero if only once
      */
      static void DrawPyramid(const glm::vec3& position,
        const glm::quat& rotation = glm::quat(1, 0, 0, 0),
        const glm::vec3& size = glm::vec3(1, 1, 1),
        const foundation::Color& color = foundation::Color::kWhite,
        bool wire_frame = false,
        bool depth_test = true,
        float lifetime = 0.0f);

      /**
      * @brief Draw a debug sphere
      * @param[in] position (const glm::vec3&) The position the sphere should appear at
      * @param[in] rotation (const glm::quat&) The rotation of the sphere
      * @param[in] size (const glm::vec3&) The size of the sphere
      * @param[in] color (const foundation::Color&) The color of the sphere
      * @param[in] wire_frame (bool) Should this sphere be rendered as a set of lines
      * @param[in] depth_test (bool) Should this sphere render on top of other objects
      * @param[in] lifetime (float) The amount of seconds this sphere should appear or zero if only once
      */
      static void DrawSphere(const glm::vec3& position,
        const glm::quat& rotation = glm::quat(1, 0, 0, 0),
        const glm::vec3& size = glm::vec3(1, 1, 1),
        const foundation::Color& color = foundation::Color::kWhite,
        bool wire_frame = true,
        bool depth_test = true,
        float lifetime = 0.0f);

      /**
      * @brief Draw a debug mesh
      * @param[in] mesh (const MeshHandle&) The mesh to draw
      * @param[in] position (const glm::vec3&) The position the mesh should appear at
      * @param[in] rotation (const glm::quat&) The rotation of the mesh
      * @param[in] size (const glm::vec3&) The size of the mesh
      * @param[in] color (const foundation::Color&) The color of the mesh
      * @param[in] wire_frame (bool) Should this mesh be rendered as a set of lines
      * @param[in] depth_test (bool) Should this mesh render on top of other objects
      * @param[in] lifetime (float) The amount of seconds this mesh should appear or zero if only once
      */
      static void DrawMesh(const MeshHandle& mesh,
        const glm::vec3& position,
        const glm::quat& rotation = glm::quat(1, 0, 0, 0),
        const glm::vec3& size = glm::vec3(1, 1, 1),
        const foundation::Color& color = foundation::Color::kWhite,
        bool wire_frame = false,
        bool depth_test = true,
        float lifetime = 0.0f);

      /**
      * @internal
      * @brief Check if any overrides are enabled
      */
      static bool HasOverrides()
      {
        return force_wireframe || force_default_material;
      };

      static bool force_wireframe; //<! Force everything to be rendered in wire frame mode
      static bool force_default_material; //<! Force everything to be rendered using the default material

    private:
      /**
      * @brief Performs the render logic
      */
      void Render();

      /**
      * @brief The data for rendering a single debug shape
      * @author Jelle de Haan
      */
      struct DebugRenderData
      {
        MeshHandle mesh; //!< The mesh to render
        foundation::Color color = foundation::Color::kWhite; //!< The color to use
        bool wire_frame = false; //!< True if the mesh should be rendered as a wireframe, solid otherwise
        bool depth_test = true; //!< True if the mesh should be rendered with depth, false otherwise
        glm::mat4 transform = glm::mat4(1); //!< The matrix to render this mesh with
        float life_time = 0.0f; //!< The number of seconds to render this shape for
      };

      static size_t unique_id_; //!< Unique-id used for registration of debug lines with the asset system

      static MeshHandle mesh_cone_; //!< The mesh instance for rendering cone shapes
      static MeshHandle mesh_cube_; //!< The mesh instance for rendering cube shapes
      static MeshHandle mesh_wire_cube_; //!< The mesh instance for rendering wire-cube shapes
      static MeshHandle mesh_cylinder_; //!< The mesh instance for rendering cylinder shapes
      static MeshHandle mesh_plane_; //!< The mesh instance for rendering plane shapes
      static MeshHandle mesh_pyramid_; //!< The mesh instance for rendering pyramid shapes
      static MeshHandle mesh_sphere_; //!< The mesh instance for rendering sphere shapes

      static MeshHandle mesh_transform_; //!< The mesh instance for rendering translation-gizmo shapes
      static MeshHandle mesh_scale_; //!< The mesh instance for rendering scale-gizmo shapes
      static MeshHandle mesh_rotation_; //!< The mesh instance for rendering rotation-gizmo shapes

      static foundation::Vector<DebugRenderData> data_; //!< The debug shapes that are currently being drawn

      CameraSystem* camera_system_; //!< A reference to the camera system of this world
      IRenderer* renderer_; //!< A pointer to the platform-renderer instance

      static MaterialHandle default_material_; //!< A material to use as a default option
      static MaterialHandle error_material_; //!< A material to indicate an erroneous reference 
    };
  }
}