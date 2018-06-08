#include "debug_render_system.h"

#include "engine/core/entity_system.h"
#include "engine/systems/components/camera_system.h"
#include "engine/systems/components/transform_system.h"
#include "engine/application/application.h"
#include "engine/assets/asset_system.h"

#include <foundation/job/job.h>
#include <foundation/job/data_policy.h>
#include <foundation/job/job_graph.h>
#include <foundation/utils/frame.h>

#include <glm/gtx/transform.hpp>

namespace sulphur
{
  namespace engine
  {
    size_t DebugRenderSystem::unique_id_;
    MaterialHandle DebugRenderSystem::default_material_;
    MaterialHandle DebugRenderSystem::error_material_;

    MeshHandle DebugRenderSystem::mesh_cone_;
    MeshHandle DebugRenderSystem::mesh_cube_;
    MeshHandle DebugRenderSystem::mesh_wire_cube_;
    MeshHandle DebugRenderSystem::mesh_cylinder_;
    MeshHandle DebugRenderSystem::mesh_plane_;
    MeshHandle DebugRenderSystem::mesh_pyramid_;
    MeshHandle DebugRenderSystem::mesh_sphere_;
    
    bool DebugRenderSystem::force_wireframe = false;
    bool DebugRenderSystem::force_default_material = false;

    foundation::Vector<DebugRenderSystem::DebugRenderData> DebugRenderSystem::data_;
    
    //-------------------------------------------------------------------------
    DebugRenderSystem::DebugRenderSystem() :
      IOwnerSystem<DebugRenderSystem>("DebugRenderSystem")
    {
    }

    //-------------------------------------------------------------------------
    void DebugRenderSystem::SetupDebugAssets()
    {
      if (default_material_) // Check if the debug assets have been setup already
      {
        return;
      }

      AssetSystem& asset_system = AssetSystem::Instance();

      // Setup debug materials
      ShaderHandle default_shader = asset_system.AddAsset(
        foundation::Memory::Construct<Shader>(),
        "Default_Shader");

      TextureHandle white_pixel = asset_system.AddAsset(
        foundation::Memory::Construct<Texture>(foundation::Color::kWhite.AsBytes(), 1u, 1u),
        "__pixel_white");

      default_material_ = asset_system.AddAsset(
        foundation::Memory::Construct<Material>(default_shader),
        "Default_Material");

      TextureHandle magenta_pixel = asset_system.AddAsset(
        foundation::Memory::Construct<Texture>(foundation::Color::kMagenta.AsBytes(), 1u, 1u),
        "__pixel_magenta");

      error_material_ = asset_system.AddAsset(
        foundation::Memory::Construct<Material>(default_shader),
        "Error_Material");
      error_material_->SetTexture(0, magenta_pixel);

      // Setup debug meshes
      mesh_cone_ = asset_system.AddAsset(
        foundation::Memory::Construct<Mesh>(Mesh::CreateCone()),
        "__Cone");
      mesh_cube_ = asset_system.AddAsset(
        foundation::Memory::Construct<Mesh>(Mesh::CreateCube()),
        "__Cube");
      mesh_wire_cube_ = asset_system.AddAsset(
        foundation::Memory::Construct<Mesh>(Mesh::CreateCube()),
        "__Wire_Cube");
      mesh_cylinder_ = asset_system.AddAsset(
        foundation::Memory::Construct<Mesh>(Mesh::CreateCylinder()),
        "__Cylinder");
      mesh_plane_ = asset_system.AddAsset(
        foundation::Memory::Construct<Mesh>(Mesh::CreatePlane()),
        "__Plane");
      mesh_pyramid_ = asset_system.AddAsset(
        foundation::Memory::Construct<Mesh>(Mesh::CreatePyramid()),
        "__Pyramid");
      mesh_sphere_ = asset_system.AddAsset(
        foundation::Memory::Construct<Mesh>(Mesh::CreateSphere()),
        "__Sphere");
    }

    //-------------------------------------------------------------------------
    void DebugRenderSystem::OnInitialize(Application& app, foundation::JobGraph& job_graph)
    {
      World& world = app.GetService<WorldProviderSystem>().GetWorld();
      camera_system_ = &world.GetComponent<CameraSystem>();
      renderer_ = &app.platform_renderer();

      const auto clear_frame_data = [](DebugRenderSystem&)
      {
        // Destroy old frame's data
        for (DebugRenderData& component : DebugRenderSystem::data_)
        {
          // Decrease lifetime of an object
          component.life_time -= foundation::Frame::delta_time();
        }
      };

      // NOTE: Should be moved to update onces all globals are handled correctly
      foundation::Job clear_frame_data_job = foundation::make_job("debugrendersystem_clear_old_frame_data", 
                                                      "render",
                                                      clear_frame_data,
                                                      bind_write(*this));
      clear_frame_data_job.set_blocker("renderer_endframe");
      job_graph.Add(std::move(clear_frame_data_job));

      const auto render = [](DebugRenderSystem& debug_render_system)
      {
        debug_render_system.Render();
      };

      foundation::Job render_job = foundation::make_job("debugrendersystem_render", "render",
                                            render, foundation::bind_write(*this));
      render_job.set_blocker("camerasystem_clearcameras");
      job_graph.Add(std::move(render_job));

      const auto post_render = [](DebugRenderSystem&)
      {
        // Destroy old frame's data
        for (eastl::reverse_iterator<DebugRenderData*> it = DebugRenderSystem::data_.rbegin();
             it != DebugRenderSystem::data_.rend();
             ++it)
        {
          // If the lifetime of this object has expired
          if (it->life_time <= 0)
          {
            // Erase it from existence
            DebugRenderSystem::data_.erase(it);
          }
        }
      };

      auto post_render_job = foundation::make_job("debugrendersystem_postrender", "render",
                                            post_render, foundation::bind_write(*this));
      post_render_job.set_blocker("renderer_endframe");
      job_graph.Add(std::move(post_render_job));
    }

    //-------------------------------------------------------------------------
    void DebugRenderSystem::OnTerminate()
    {
      data_.clear();

      // Make sure the assets get destroyed before the asset manager is destroyed
      default_material_ = MaterialHandle();
      error_material_ = MaterialHandle();

      mesh_cone_ = MeshHandle();
      mesh_cube_ = MeshHandle();
      mesh_wire_cube_ = MeshHandle();
      mesh_cylinder_ = MeshHandle();
      mesh_plane_ = MeshHandle();
      mesh_pyramid_ = MeshHandle();
      mesh_sphere_ = MeshHandle();
    }

    //-------------------------------------------------------------------------
    void DebugRenderSystem::Render()
    {
      CameraComponent camera = camera_system_->main_camera();
      if (!camera.IsValid())
      {
        PS_LOG(Warning, "Unable to render without a main camera!");
        return;
      }
      
      camera_system_->set_current_camera(camera);
      renderer_->SetCamera(
        camera.GetTransform().GetWorldPosition(),
        camera.GetViewMatrix(),
        camera.GetProjectionMatrix(),
        camera.GetDepthBuffer(),
        camera.GetRenderTarget());

      // FIXME: For now just do for every component
      for (const DebugRenderData& component : data_)
      {
        for (int i = 0; i < default_material_->NumMaterialPasses(); ++i)
        {
          const MaterialPass& pass = default_material_->GetMaterialPass(i);

          if (!component.mesh)
            continue;

          renderer_->SetMaterial(pass);

          graphics::PipelineState ps = pass.shader()->pipeline_state; // Copy pipeline state
          ps.rasterizer_state.fill_mode = component.wire_frame ? graphics::FillMode::kWireFrame : graphics::FillMode::kSolid;
          ps.depth_stencil_state.depth_enable = component.depth_test;

          renderer_->SetPipelineState(ps);

          renderer_->SetModelMatrix(component.transform);

          renderer_->SetMesh(component.mesh);
          if (component.mesh->IsAlwaysOnTop())
          {
            renderer_->SetStencilRef(2);
            renderer_->Draw();
            renderer_->SetStencilRef(0);
          }
          else
          {
            renderer_->Draw();
          }
          
        }
      }
      // END FIXME
    }

    //-------------------------------------------------------------------------
    void DebugRenderSystem::DrawLine(const glm::vec3& start,
      const glm::vec3& end,
      const foundation::Color& color,
      bool depth_test,
      float lifetime)
    {
      DebugRenderData data{};
      data.color = color;
      data.depth_test = depth_test;
      data.wire_frame = true;
      data.life_time = lifetime;


      Mesh* mesh = foundation::Memory::Construct<Mesh>();
      mesh->SetTopologyType(graphics::TopologyType::kLine);

      foundation::Vector<glm::vec3> vertices;
      foundation::Vector<glm::vec2> uvs;
      foundation::Vector<glm::vec3> normals;
      foundation::Vector<glm::vec3> tangents;
      foundation::Vector<foundation::Color> colors;
      foundation::Vector<uint32_t> indices;

      // Start
      vertices.emplace_back(start); uvs.emplace_back(glm::vec2(0.0f, 0.0f));
      normals.emplace_back(glm::vec3(0.0f, 0.0f, -1.0f));
      tangents.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
      indices.emplace_back(0);
      colors.emplace_back(color);

      // End
      vertices.emplace_back(end); uvs.emplace_back(glm::vec2(1.0f, 1.0f));
      normals.emplace_back(glm::vec3(0.0f, 0.0f, -1.0f));
      tangents.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
      indices.emplace_back(1);
      colors.emplace_back(color);

      mesh->SetVertices(eastl::move(vertices));
      mesh->SetIndices(eastl::move(indices));
      mesh->SetUVs(eastl::move(uvs));
      mesh->SetNormals(eastl::move(normals));
      mesh->SetTangents(eastl::move(tangents));
      mesh->SetColors(eastl::move(colors));

      const foundation::String name("__DebugRenderSystem_line_" + foundation::to_string(unique_id_++));
      data.mesh = AssetSystem::Instance().AddAsset(mesh, name);

      // Add debug mesh
      data_.emplace_back(data);
    }

    //-------------------------------------------------------------------------
    void DebugRenderSystem::DrawCone(const glm::vec3 & position, const glm::quat& rotation,
      const glm::vec3& size, const foundation::Color& color, bool wire_frame, bool depth_test, float lifetime)
    {
      DrawMesh(mesh_cone_, position, rotation, size, color,
        wire_frame, depth_test, lifetime);
    }

    //-------------------------------------------------------------------------
    void DebugRenderSystem::DrawCube(const glm::vec3& position, const glm::quat& rotation,
      const glm::vec3& size, const foundation::Color& color, bool wire_frame, bool depth_test, float lifetime)
    {
      DrawMesh(mesh_cube_, position, rotation, size, color,
        wire_frame, depth_test, lifetime);
    }

    //-------------------------------------------------------------------------
    void DebugRenderSystem::DrawCylinder(const glm::vec3& position, const glm::quat& rotation,
      const glm::vec3& size, const foundation::Color& color, bool wire_frame, bool depth_test, float lifetime)
    {
      DrawMesh(mesh_cylinder_, position, rotation, size, color,
        wire_frame, depth_test, lifetime);
    }

    //-------------------------------------------------------------------------
    void DebugRenderSystem::DrawPlane(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, const foundation::Color& color, bool wire_frame, bool depth_test, float lifetime)
    {
      DrawMesh(mesh_plane_, position, rotation, size, color,
        wire_frame, depth_test, lifetime);
    }

    //-------------------------------------------------------------------------
    void DebugRenderSystem::DrawPyramid(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, const foundation::Color& color, bool wire_frame, bool depth_test, float lifetime)
    {
      DrawMesh(mesh_pyramid_, position, rotation, size, color,
        wire_frame, depth_test, lifetime);
    }

    //-------------------------------------------------------------------------
    void DebugRenderSystem::DrawSphere(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, const foundation::Color& color, bool wire_frame, bool depth_test, float lifetime)
    {
      DrawMesh(mesh_sphere_, position, rotation, size, color,
        wire_frame, depth_test, lifetime);
    }

    //-------------------------------------------------------------------------
    void DebugRenderSystem::DrawMesh(const MeshHandle& mesh, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& size, const foundation::Color& color, bool wire_frame, bool depth_test, float lifetime)
    {
      DebugRenderData data{};
      data.color = color;
      data.wire_frame = wire_frame;
      data.depth_test = depth_test;
      data.life_time = lifetime;
      data.mesh = mesh;

      glm::mat4 t = glm::translate(position);
      glm::mat4 r = glm::mat4_cast(rotation);
      glm::mat4 s = glm::scale(size);
      data.transform = t * r * s;

      // Add debug mesh
      data_.push_back(data);
    }
  }
}


