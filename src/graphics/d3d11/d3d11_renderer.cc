#include "d3d11_renderer.h"

#include <engine/assets/mesh.h>
#include <engine/assets/shader.h>
#include <engine/assets/material.h>
#include <engine/assets/render_target.h>
#include <engine/assets/depth_buffer.h>

#include <foundation/application/native_window_handle.h>

#include <d3d11.h>

#include <engine/graphics/imgui/imgui.h>
#include "imgui_impl.h"

namespace sulphur
{
  namespace graphics
  {
    //--------------------------------------------------------------------------
    D3D11Renderer::D3D11Renderer() :
      asset_manager_(device_)
    {
      device_.Create();
    }

    //--------------------------------------------------------------------------
    D3D11Renderer::~D3D11Renderer()
    {
      OnDestroy();
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::OnInitialize(void* window_handle, const glm::ivec2& screen_size, bool vsync)
    {
      vsync_ = vsync;

      device_.CreateSwapChain(window_handle, screen_size, vsync_, 60u);

      device_.CreateDepthStencilState();

      device_.CreateBackBuffer();
      device_.CreateObjectBuffer(cb_per_object_buffer_, sizeof(cbPerObject));

      device_.CreateDefaultShaders();
      
      device_.SetViewport(0.0f, 0.0f, (float)screen_size.x, (float)screen_size.y);

      device_.CreateBlendState();
      device_.CreateSamplers();
      device_.CreateRasterizerState();
      
      initialized_ = true;

      ImGui_ImplDX11_Init(window_handle, device_.GetDevice(), device_.GetDeviceContext());

      // TODO: Put this in a better place
      ImGui_Impl_NewFrame();
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::OnDestroy()
    {
      if (!initialized_)
      {
        return;
      }
      initialized_ = false;

      // Release assets
      draw_call_data_.material = engine::MaterialPass();
      draw_call_data_.mesh = engine::MeshHandle();

      bound_asset_data_.depth_stencil = nullptr;
      bound_asset_data_.material = engine::MaterialPass();


      ImGui_Impl_Shutdown();

      asset_manager_.OnDestroy();
    }
    
    //--------------------------------------------------------------------------
    void D3D11Renderer::OnResizeWindow(uint, uint)
    {
      // TODO: Resize stuff
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::StartFrame()
    {
      if (!initialized_)
      {
        return;
      }

      // TODO: Prepare renderer for a new frame
      // Reset pipeline state to default
      // Reset vertex counter
      // etc..
      // IMPORTANT: DO NOT CLEAR THE RENDER TARGETS, THIS IS DONE BY THE CAMERA'S
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::EndFrame(bool present)
    {
      // SetRenderTargets hack for ImGui
      device_.SetRenderTargets(1, bound_asset_data_.render_targets, bound_asset_data_.depth_stencil);
      ImGui::Render();

      if (present)
      {
        device_.Present(vsync_);
      }

      // TODO: Put this in a better place
      ImGui_Impl_NewFrame();
    }

    

    //--------------------------------------------------------------------------
    void D3D11Renderer::SetMesh(const engine::MeshHandle& mesh)
    {
      if (!mesh->Validate())
      {
        return;
      }

      if (!mesh.GetGPUHandle())
      {
        asset_manager_.CreateMesh(mesh);
      }

      // TODO: update dynamic mesh
      /*
      if (mesh->HasChanged())
      {
      UpdateDynamicMesh(mesh);
      }
      */

      draw_call_data_.mesh = mesh;
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::SetBoneMatrices(const foundation::Vector<glm::mat4>&)
    {
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::SetPipelineState(const PipelineState& pipeline_state)
    {
      draw_call_data_.pipeline_state = pipeline_state;
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::SetMaterial(const engine::MaterialPass& material)
    {
      draw_call_data_.material = material;
      draw_call_data_.pipeline_state = material.shader()->pipeline_state;

      if (!material.shader().GetGPUHandle())
      {
        asset_manager_.CreateShader(material.shader());
      }

      for (int i = 0; i < material.NumTextures(); ++i)
      {
        if (!material.GetTexture(i) || material.GetTexture(i).GetGPUHandle()) // TEMP TEST, UNTIL MATERIALS ARE SHADER BASED
          continue;

        asset_manager_.CreateTexture(material.GetTexture(i));
      }
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::SetComputePass(const engine::ComputePass& /*pass*/)
    {}

    //--------------------------------------------------------------------------
    void D3D11Renderer::SetCamera(const glm::mat4& view, const glm::mat4& projection, const engine::DepthBuffer& depth_buffer, const engine::RenderTarget& render_target)
    {
      // TODO:

      // SetViewport(camera->GetViewport());
      // SetScissorRect(camera->GetBounds());
      
      cb_per_obj_.view = view;
      cb_per_obj_.projection = projection;

      if (!depth_buffer.buffer().GetGPUHandle())
      {
        asset_manager_.CreateDepthBuffer(depth_buffer.buffer());
      }

      D3D11Texture* depth_texture = asset_manager_.GetTexture(depth_buffer.buffer());
      bound_asset_data_.depth_stencil = depth_texture->depth_stencil_view;

      if (render_target.render_target_type() == engine::RenderTargetType::kBackBuffer)
      {
        bound_asset_data_.render_targets[0] = device_.GetBackBuffer().Get();
        bound_asset_data_.bound_render_target_count = 1;
      }
      else
      {
        if (!render_target.GetTextureResource().GetGPUHandle())
        {
          asset_manager_.CreateRenderTarget(render_target.GetTextureResource());
        }

        D3D11Texture* render_texture = asset_manager_.GetTexture(render_target.GetTextureResource());

        bound_asset_data_.render_targets[0] = render_texture->render_target_view.Get();
        bound_asset_data_.bound_render_target_count = 1;
      }
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::SetModelMatrix(const glm::mat4& model_matrix)
    {
      cb_per_obj_.world = model_matrix;
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::SetScissorRect(const glm::vec4&)
    {
      // TODO: Set scissor rect if changed
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::SetViewport(const glm::vec4&)
    {
      // TODO: Set viewport rect if changed
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::ClearRenderTarget(const engine::RenderTarget& render_target, const foundation::Color& clear_color)
    {
      if (render_target.render_target_type() == engine::RenderTargetType::kBackBuffer)
      {
        device_.ClearBackBuffer(clear_color);
      }
      else
      {
        if (!render_target.GetTextureResource().GetGPUHandle())
        {
          asset_manager_.CreateRenderTarget(render_target.GetTextureResource());
        }

        D3D11Texture* render_texture = asset_manager_.GetTexture(render_target.GetTextureResource());
        device_.ClearRenderTarget(render_texture->render_target_view, clear_color);
      }
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::ClearDepthBuffer(const engine::DepthBuffer& depth_buffer)
    {
      if (!depth_buffer.buffer().GetGPUHandle())
      {
        asset_manager_.CreateDepthBuffer(depth_buffer.buffer());
      }

      D3D11Texture* depth_texture = asset_manager_.GetTexture(depth_buffer.buffer());

      device_.ClearDepthBuffer(depth_texture->depth_stencil_view);
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::Draw(uint index_count, uint index_offset)
    {
      if (!draw_call_data_.material.shader() ||
        draw_call_data_.mesh->GetIndexCount() == 0)
      {
        PS_LOG(Warning, "Unable to render without a shader or index count is zero, ignoring this draw call!");
        return;
      }

      if (index_count == 0)
      {
        index_count = draw_call_data_.mesh->GetIndexCount();
      }

      // Bind render target state
      device_.SetRenderTargets(1, bound_asset_data_.render_targets, bound_asset_data_.depth_stencil);

      UpdateMaterial();

      // Update uniform buffer data
      device_.UpdateObjectBuffer(cb_per_object_buffer_, &cb_per_obj_);

      UpdateMesh();

      //pso_manager_.SetPipelineState(draw_call_data_.pipeline_state, vert, pix);


      device_.Draw(index_count, index_offset);
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::CopyToScreen(const engine::RenderTarget& render_target)
    {
      if (!render_target.GetTextureResource().GetGPUHandle())
      {
        return;
      }

      D3D11Texture* texture = asset_manager_.GetTexture(render_target.GetTextureResource());
      
      Microsoft::WRL::ComPtr<ID3D11Resource> backbuffer_texture;
      device_.GetBackBuffer()->GetResource(backbuffer_texture.GetAddressOf());

      device_.CopyResource(backbuffer_texture, texture->texture);
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::Dispatch(ComputeQueueType, uint, uint, uint)
    {
      // TODO: Process compute shader dispatch call
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::SetVsync(bool value)
    {
      // TODO: Update renderer to use vsync value
      vsync_ = value;
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::SetStencilRef(uint value)
    {
      device_.SetStencilRef(value);
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::UpdateMaterial()
    {
      // Update shader
      const engine::MaterialPass& material = draw_call_data_.material;

      D3D11Shader* shader = asset_manager_.GetShader(material.shader());
      device_.SetShader(shader);

      for (int i = 0; i < material.NumTextures(); ++i)
      {
        D3D11Texture* texture = asset_manager_.GetTexture(material.GetTexture(i));
        device_.SetTexture(i, texture);
      }
    }

    //--------------------------------------------------------------------------
    void D3D11Renderer::UpdateMesh()
    {
      // Update mesh
      D3D11Mesh* mesh = asset_manager_.GetMesh(draw_call_data_.mesh);
      if (bound_asset_data_.mesh != mesh)
      {
        bound_asset_data_.mesh = mesh;
        device_.SetMesh(mesh);

        // Update topology type
        if (bound_asset_data_.primitive_topology != draw_call_data_.mesh->topology_type())
        {
          bound_asset_data_.primitive_topology = draw_call_data_.mesh->topology_type();
          device_.SetPrimitiveTopology(bound_asset_data_.primitive_topology);
        }
      }

      // Force pipeline state to meshes topology type
      draw_call_data_.pipeline_state.topology_type = bound_asset_data_.primitive_topology;
    }
  }
}
