#include "debug_render_system.h"

#include "engine/core/entity_system.h"
#include "engine/systems/components/camera_system.h"
#include "engine/systems/components/transform_system.h"
#include "engine/application/application.h"
#include "engine/assets/asset_system.h"

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

    MeshHandle DebugRenderSystem::mesh_transform_;
    MeshHandle DebugRenderSystem::mesh_scale_;
    MeshHandle DebugRenderSystem::mesh_rotation_;

    MaterialHandle DebugRenderSystem::debug_material_;

    bool DebugRenderSystem::force_wireframe = false;
    bool DebugRenderSystem::force_default_material = false;

    foundation::Vector<DebugRenderSystem::DebugRenderData> DebugRenderSystem::data_;
    
    //-------------------------------------------------------------------------
    DebugRenderSystem::DebugRenderSystem() :
      IComponentSystem<DebugRendererComponent>("DebugRenderSystem")
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
        foundation::Memory::Construct<Material>(),
        "Default_Material");

      MaterialPass white_pass(default_shader);
      white_pass.SetTexture(0, white_pixel);
      default_material_->AddMaterialPass(white_pass);


      TextureHandle magenta_pixel = asset_system.AddAsset(
        foundation::Memory::Construct<Texture>(foundation::Color::kMagenta.AsBytes(), 1u, 1u),
        "__pixel_magenta");

      error_material_ = asset_system.AddAsset(
        foundation::Memory::Construct<Material>(),
        "Error_Material");

      MaterialPass magenta_pass(default_shader);
      magenta_pass.SetTexture(0, magenta_pixel);
      error_material_->AddMaterialPass(magenta_pass);

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


      Mesh arrow_mesh;
      arrow_mesh.AttachMesh(
        Mesh::CreateCylinder(1.0f, 0.5f, 0.5f, 6),
        glm::vec3(0.0f, 0.5f, 0.0f),
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec3(0.05f, 1.0f, 0.05f));

      arrow_mesh.AttachMesh(
        Mesh::CreateCone(1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec3(.125f, .25f, .125f));

      Mesh* transform_mesh = foundation::Memory::Construct<Mesh>();


      mesh_transform_ = asset_system.AddAsset(
        transform_mesh,
        "__Transform_Gizmo");

      mesh_scale_ = asset_system.AddAsset(
        foundation::Memory::Construct<Mesh>(Mesh::CreateSphere()),
        "__Scale_Gizmo");

      mesh_rotation_ = asset_system.AddAsset(
        foundation::Memory::Construct<Mesh>(Mesh::CreateSphere()),
        "__Rotation_Gizmo");
    }

    //-------------------------------------------------------------------------
    void DebugRenderSystem::OnInitialize(Application& app, foundation::JobGraph&)
    {
      World& world = app.GetService<WorldProviderSystem>().GetWorld();
      camera_system_ = &world.GetSystem<CameraSystem>();
      renderer_ = &app.platform_renderer();
    }

    //-------------------------------------------------------------------------
    void DebugRenderSystem::OnUpdate(float delta_time)
    {
      // Destroy old frame's data
      for (DebugRenderData& component : data_)
      {
        // Decrease lifetime of an object
        component.life_time -= delta_time;
      }
    }

    //-------------------------------------------------------------------------
    void DebugRenderSystem::OnTerminate()
    {
      data_.clear();

      // Make sure the assets get destroyed before the asset manager is destroyed
      default_material_ = MaterialHandle();
      error_material_ = MaterialHandle();
      debug_material_ = MaterialHandle();

      mesh_cone_ = MeshHandle();
      mesh_cube_ = MeshHandle();
      mesh_wire_cube_ = MeshHandle();
      mesh_cylinder_ = MeshHandle();
      mesh_plane_ = MeshHandle();
      mesh_pyramid_ = MeshHandle();
      mesh_sphere_ = MeshHandle();

      mesh_transform_ = MeshHandle();
      mesh_scale_ = MeshHandle();
      mesh_rotation_ = MeshHandle();
    }

    //-------------------------------------------------------------------------
    DebugRendererComponent DebugRenderSystem::Create(Entity)
    {
      return DebugRendererComponent();
    }
    void DebugRenderSystem::Destroy(DebugRendererComponent)
    {
    }

    //-------------------------------------------------------------------------
    void DebugRenderSystem::OnPreRender()
    {}

    //-------------------------------------------------------------------------
    void DebugRenderSystem::OnRender()
    {
      CameraComponent camera = camera_system_->main_camera();
      if (!camera.IsValid())
      {
        PS_LOG(Warning, "Unable to render without a main camera!");
        return;
      }
      
      camera_system_->set_current_camera(camera);
      renderer_->SetCamera(
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
          ps.rasterizer_state.cull_mode = graphics::CullMode::kNone;
          ps.depth_stencil_state.depth_enable = component.depth_test;
          ps.depth_stencil_state.stencil_enable = false;
          renderer_->SetPipelineState(ps);

          renderer_->SetModelMatrix(component.transform);

          renderer_->SetMesh(component.mesh);
          renderer_->Draw();
        }
      }
      // END FIXME
    }

    //-------------------------------------------------------------------------
    void DebugRenderSystem::OnPostRender()
    {
      // Destroy old frame's data
      for (eastl::reverse_iterator<DebugRenderData*> it = data_.rbegin(); it != data_.rend(); ++it)
      {
        // If the lifetime of this object has expired
        if (it->life_time <= 0)
        {
          // Erase it from existence
          data_.erase(it);
        }
      }
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


