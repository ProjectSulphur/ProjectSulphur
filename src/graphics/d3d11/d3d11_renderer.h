#pragma once
#include "d3d11_device_wrapper.h"
#include "d3d11_asset_manager.h"

#include "graphics/platform/pipeline_state.h"
#include <engine/assets/material.h>
#include <engine/assets/post_process_material.h>
#include <engine/graphics/irenderer.h>

namespace sulphur
{
  namespace engine
  {
    class World;
  }

  namespace graphics
  {
    class D3D11Renderer : public engine::IRenderer
    {
    public:
      D3D11Renderer();
      ~D3D11Renderer();

      // Events
      bool IsInitialized() { return initialized_; };
      void OnInitialize(void* window_handle, const glm::ivec2& screen_size = glm::ivec2(1280, 720), bool vsync = false) override;
      void OnDestroy() override;
      void OnUpdate() override {};
      void OnResizeWindow(uint width, uint height) override;
      void StartFrame() override;
      void EndFrame(bool present = true) override;

      // Processing draws
      void SetMesh(const engine::MeshHandle& mesh) override;
      void SetBoneMatrices(const foundation::Vector<glm::mat4>& bone_matrices)override;
      void SetPipelineState(const PipelineState& pipeline_state) override;
      void SetMaterial(const engine::MaterialPass& pass) override;
      void SetComputePass(const engine::ComputePass& pass) override;
      void SetCamera(const glm::mat4& view, const glm::mat4& projection, const engine::DepthBuffer& depth_buffer, const engine::RenderTarget& render_target) override;
      void SetModelMatrix(const glm::mat4& model_matrix) override;
      void SetScissorRect(const glm::vec4& rect) override;
      void SetViewport(const glm::vec4& rect) override;

      // Execute
      void ClearRenderTarget(const engine::RenderTarget& render_target, const foundation::Color& clear_color) override;
      void ClearDepthBuffer(const engine::DepthBuffer& depth_buffer) override;
      void Draw(uint index_count = 0, uint index_offset = 0) override;
      void CopyToScreen(const engine::RenderTarget& render_target) override;
      void Dispatch(ComputeQueueType type, uint x, uint y, uint z) override;
      
      // Settings
      void SetVsync(bool value) override;
      void SetStencilRef(uint value) override;

    protected:
      void UpdateMaterial();
      void UpdateMesh();

    private:
      struct cbPerObject
      {
        glm::mat4 world;
        glm::mat4 view;
        glm::mat4 projection;
      } cb_per_obj_;

      struct DrawCall
      {
        engine::MeshHandle mesh;
        engine::MaterialPass material;
        PipelineState pipeline_state;
      } draw_call_data_;

      static const size_t kMaxSimultaneousRenderTargets = 8; //D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT == 8

      struct BoundAssets
      {
        D3D11Mesh* mesh = nullptr;
        engine::MaterialPass material;
        TopologyType primitive_topology = TopologyType::kTriangle;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil;
        uint bound_render_target_count = 0u;
        ID3D11RenderTargetView* render_targets[kMaxSimultaneousRenderTargets];
      } bound_asset_data_;

      bool initialized_, vsync_;

      D3D11Device device_;
      D3D11AssetManager asset_manager_;

      Microsoft::WRL::ComPtr<ID3D11Buffer> cb_per_object_buffer_;
    };

  }
}
