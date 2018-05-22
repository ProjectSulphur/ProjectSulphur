#pragma once

#include "engine/systems/component_system.h"
#include "engine/graphics/irenderer.h"

#include "engine/assets/mesh.h"
#include "engine/assets/material.h"

#include <foundation/containers/vector.h>

#include <glm/glm.hpp>

namespace sulphur 
{
  namespace engine 
  {
    class Entity;
    class CameraSystem;
    class DebugRenderSystem;

    /**
    * @internal
    * @class DebugRendererComponent
    * @brief Hack for getting a unique system id
    * @author Not me
    */
    class DebugRendererComponent : public ComponentHandleBase
    {
    public:
      using System = DebugRenderSystem; //!< System type define

    };

    /**
    * @class sulphur::engine::DebugRenderSystem
    * @brief Manages drawing of debug objects like gizmo's and lines
    * @author Jelle de Haan
    * @see sulphur::engine::DebugRenderComponent
    */
    class DebugRenderSystem : public IComponentSystem<DebugRendererComponent>
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
      * @see sulphur::engine::IComponentSystem::OnUpdate
      */
      void OnUpdate(float delta_time) override;

      /**
      * @internal
      * @see sulphur::engine::IComponentSystem::OnTerminate
      */
      void OnTerminate() override;

      /**
      * @internal
      * @brief UNUSED
      */
      DebugRendererComponent Create(Entity entity) override;

      /**
      * @internal
      * @brief UNUSED
      */
      void Destroy(DebugRendererComponent) override;

      /**
      * @internal
      * @see sulphur::engine::IComponentSystem::OnPreRender
      */
      void OnPreRender() override;

      /**
      * @internal
      * @see sulphur::engine::IComponentSystem::OnRender
      */
      void OnRender() override;

      /**
      * @internal
      * @see sulphur::engine::IComponentSystem::OnPostRender
      */
      void OnPostRender() override;

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
      struct DebugRenderData
      {
        MeshHandle mesh;
        foundation::Color color = foundation::Color::kWhite;
        bool wire_frame = false;
        bool depth_test = true;
        glm::mat4 transform = glm::mat4(1);
        float life_time = 0.0f;
      };

      static size_t unique_id_;

      static MeshHandle mesh_cone_;
      static MeshHandle mesh_cube_;
      static MeshHandle mesh_wire_cube_;
      static MeshHandle mesh_cylinder_;
      static MeshHandle mesh_plane_;
      static MeshHandle mesh_pyramid_;
      static MeshHandle mesh_sphere_;

      static MeshHandle mesh_transform_;
      static MeshHandle mesh_scale_;
      static MeshHandle mesh_rotation_;


      static MaterialHandle debug_material_;

      static foundation::Vector<DebugRenderData> data_;

      CameraSystem* camera_system_;
      IRenderer* renderer_;


      static MaterialHandle default_material_, error_material_;
    };
  }
}