#pragma once

#include "graphics/platform/renderer_loader.h"

#include "graphics/d3d12/d3d12_device_wrapper.h"
#include "graphics/d3d12/assets/d3d12_asset_manager.h"
#include "graphics/d3d12/d3d12_constant_buffer_heap.h"
#include "graphics/d3d12/d3d12_frame_descriptor_heap.h"
#include "graphics/d3d12/d3d12_pso_manager.h"
#include "graphics/d3d12/assets/d3d12_material_manager.h"

#include <engine/assets/material.h>
#include <engine/assets/post_process_material.h>
#include <engine/assets/mesh.h>

#include <foundation/utils/color.h>
#include <foundation/memory/memory.h>

#include <d3d12.h>
#include <dxgi1_4.h>

namespace sulphur 
{
  namespace graphics 
  {
    class D3D12PersistentDescriptorHeap;
    class D3D12MeshAssetManager;

    /**
    * @class sulphur::graphics::D3D12Renderer : public sulphur::engine::IRenderer
    * @brief This class controls the flow of the DirectX 12 rendering pipeline.
    * @author Yana Mateeva
    */
    class D3D12Renderer : public engine::IRenderer
    {

    public:
      /**
      * @brief Constructor.
      */
      D3D12Renderer();

      /**
      * @brief Destructor.
      */
      ~D3D12Renderer();

      // Events
      /**
      * @return (bool) Is this renderer initialized?
      */
      bool IsInitialized() const { return initialized_; };

      /**
      * @see sulphur::engine::IRenderer::OnInitialize
      */
      void OnInitialize(
        void* window_handle,
        const glm::ivec2& screen_size = glm::ivec2(1280, 720),
        bool vsync = false) override;

      /**
      * @see sulphur::engine::IRenderer::OnDestroy
      */
      void OnDestroy() override;

      /**
      * @see sulphur::engine::IRenderer::OnUpdate
      */
      void OnUpdate() override;

      /**
      * @see sulphur::engine::IRenderer::OnResizeWindow
      */
      void OnResizeWindow(uint width, uint height) override;
      
      /**
      * @see sulphur::engine::IRenderer::StartFrame
      */
      void StartFrame() override;

      /**
      * @see sulphur::engine::IRenderer::EndFrame
      */
      void EndFrame(bool present = true) override;

      // Processing draws
      /**
      * @see sulphur::engine::IRenderer::SetMesh
      */
      void SetMesh(const engine::MeshHandle& mesh) override;

      /**
      * @see sulphur::engine::IRenderer::SetBoneMatrices
      */
      void SetBoneMatrices(const foundation::Vector<glm::mat4>& bone_matrices) override;

      /**
      * @see sulphur::engine::IRenderer::SetPipelineState
      */
      void SetPipelineState(const PipelineState& pipeline_state) override;

      /**
      * @see sulphur::engine::IRenderer::SetMaterial
      */
      void SetMaterial(const engine::MaterialPass& material) override;

      /**
      * @see sulphur::engine::IRenderer::SetComputePass
      */
      void SetComputePass(const engine::ComputePass& pass) override;

      /**
      * @see sulphur::engine::IRenderer::SetCamera
      */
      void SetCamera(
        const glm::vec3& pos,
        const glm::mat4& view,
        const glm::mat4& projection,
        const engine::DepthBuffer& depth_buffer,
        const engine::RenderTarget& render_target) override;

      /**
      * @see sulphur::engine::IRenderer::SetModelMatrix
      */
      void SetModelMatrix(const glm::mat4& model_matrix) override;

      /**
      * @see sulphur::engine::IRenderer::SetScissorRect
      */
      void SetScissorRect(const glm::vec4& rect) override;

      /**
      * @see sulphur::engine::IRenderer::SetViewport
      */
      void SetViewport(const glm::vec4& rect) override;

      // Execute
      /**
      * @see sulphur::engine::IRenderer::ClearRenderTarget
      */
      void ClearRenderTarget(
        const engine::RenderTarget& render_target,
        const foundation::Color& clear_color) override;

      /**
      * @see sulphur::engine::IRenderer::ClearDepthBuffer
      */
      void ClearDepthBuffer(const engine::DepthBuffer& depth_buffer) override;

      /**
      * @see sulphur::engine::IRenderer::Draw
      */
      void Draw(uint index_count = 0, uint index_offset = 0) override;

      /**
      * @brief Copies the render target contents onto the back buffer, overwriting it
      * @see sulphur::engine::IRenderer::CopyToScreen
      */
      void CopyToScreen(const engine::RenderTarget& render_target) override;

      /**
      * @see sulphur::engine::IRenderer::Dispatch
      */
      void Dispatch(ComputeQueueType type, uint x, uint y, uint z) override;

      // Settings
      /**
      * @see sulphur::engine::IRenderer::SetVsync
      */
      void SetVsync(bool value) override;

      /**
      * @see sulphur::engine::IRenderer::SetStencilRef
      */
      void SetStencilRef(uint value) override;

    protected:
      // Setting GPU resources
      /**
      * @brief Sets the currently used texture at the specified register id.
      * @param[in] register_id (int) The register id to bind the texture to.
      * @param[in] texture (const sulphur::engine::TextureHandle&) A handle to the texture to set.
      * @param[in] is_compute_resource (bool) Is this texture being bound as a resource to a compute shader?
      * @param[in] use_ping_pong (bool) Is the current pass using this texture as a ping-pong buffer?
      */
      void SetTexture(
        int register_id,
        const engine::TextureHandle& texture,
        bool is_compute_resource = false,
        bool use_ping_pong = false);

      /**
      * @brief Sets the currently used Read/Write texture at the specified register id.
      * @param[in] register_id (int) The register id to bind the texture to.
      * @param[in] uav_texture (const sulphur::engine::TextureHandle&) A handle to the texture to set.
      * @param[in] use_ping_pong (bool) Is the current pass using this texture as a ping-pong buffer?
      */
      void SetUAV(
        int register_id,
        const engine::TextureHandle& uav_texture,
        bool use_ping_pong = false);

      /**
      * @brief Sets the currently used render target at the specified register id with an optimized clear value.
      * @param[in] register_id (int) The register id to bind the render target to.
      * @param[in] render_target (const sulphur::engine::RenderTarget&) The render target to set.
      * @param[in] clear_color (const sulphur::foundation::Color&) The optimized clear value to use for this render target.
      */
      void SetRenderTarget(
        int register_id,
        const engine::RenderTarget& render_target,
        const foundation::Color& clear_color);
      
      /**
      * @brief Sets the currently used depth buffer.
      * @param[in] depth_buffer (const sulphur::engine::DepthBuffer&) The depth buffer to set.
      */
      void SetDepthBuffer(const engine::DepthBuffer& depth_buffer);

      // Loading GPU resources
      /**
      * @brief Load a texture into GPU memory.
      * @param[in] texture (const sulphur::engine::TextureHandle&) The texture to load.
      * @param[in] type (const sulphur::graphics::D3D12TextureType) The type of the texture.
      * @param[in] clear_color (const sulphur::foundation::Color) An optimized clear value to use when creating the resource for the texture.
      */
      void LoadTexture(
        const engine::TextureHandle& texture,
        foundation::Color clear_color = foundation::Color::kBlackTransparent);

      /**
      * @brief Loads a mesh into GPU memory.
      * @param[in] mesh (const sulphur::engine::MeshHandle&) The mesh to load.
      */
      void LoadMesh(const engine::MeshHandle& mesh);

      /**
      * @brief Loads the specified shader.
      * @param[in] shader (const sulphur::engine::ShaderHandle&) The shader to load.
      */
      void LoadShader(const engine::ShaderHandle& shader);

      /**
      * @brief Updates the GPU resources for a dynamic mesh.
      * @param[in] mesh (const sulphur::engine::MeshHandle&) The dynamic mesh to update.
      */
      void UpdateDynamicMesh(const engine::MeshHandle& mesh);

      static const uint8_t BACK_BUFFER_COUNT = 2; //!< The amount of back buffers to use with this renderer.

    private:
      /**
      * @struct sulphur::graphics::D3D12Renderer::DrawCallData
      * @brief A collection of the current data, used for a draw call.
      * @author Yana Mateeva
      */
      struct DrawCallData
      {
        engine::MeshHandle current_mesh; //!< The currently used mesh.
        engine::MaterialPass current_material; //!< The currently used material.
        PipelineState current_pipeline_state; //!< The currently used pipeline state
        bool use_default_pipeline_state = true; //!< Should a default pipeline state be used?
        TopologyType current_primitive_topology = TopologyType::kTriangle; //!< The currently set primitive topology.
      } current_draw_call_; //!< Current draw call information.

      bool initialized_; //!< Is the renderer initialized?
      bool vsync_; //!< Is vsync enabled?

      uint32_t current_frame_index_; //!< The current back buffer index.

      float total_time_; //!< Total time passed (TODO: Remove this, currently used as hack to test materials).

      D3D12ConstantBufferHeap constant_buffer_heap_; //!< The constant buffer heap, used by this renderer.

      D3D12FrameDescriptorHeap frame_descriptor_heap_; //!< The sorted frame descriptor heap, used by this renderer.

      PsoManager pso_manager_; //!< The pipeline state object manager, used by this renderer.

      D3D12MaterialManager material_manager_; //!< The material manager, used by this renderer.

      // TODO: Temporary command queues/lists/allocators - replace with some kind of wrapper (needs research)
      ID3D12CommandQueue* direct_command_queue_; //!< The direct command queue, used to execute the direct command list.
      ID3D12GraphicsCommandList* direct_command_list_; //!< The direct command list, used to record draw commands for this renderer.
      ID3D12CommandAllocator* direct_command_allocators_[BACK_BUFFER_COUNT]; //!< Collection of command allocators to use for the direct command list.

      IDXGISwapChain3* swap_chain_; //!< The DirectX swap chain object.

      // TODO: Replace with proper resource wrappers?
      D3D12Texture2D* OM_render_targets_[BACK_BUFFER_COUNT]; //!< DirectX 12 resources for the back buffers
      ID3D12Resource* depth_buffer_[BACK_BUFFER_COUNT]; //!< DirectX 12 resource for the main depth buffers.
      uint32_t persistent_depth_buffer_handles_[BACK_BUFFER_COUNT]; //!< Handles to the main depth buffer descriptors in the persistent descriptor heap.
      
      // TODO: Temporary root signature
      ID3D12RootSignature* root_signature_; //!< The default root signature.

      // TODO: Temporary fence(sync) objects
      ID3D12Fence* fence_; //!< DirectX 12 fence object. Used to sync command queues between frames.
      uint64_t fence_values_[BACK_BUFFER_COUNT]; //!< Current fence values for each frame.
      HANDLE fence_event_; //!< Event, used to wait for synchronization.

      // TODO: Temporary viewport/scissor rect
      D3D12_VIEWPORT viewport_; //!< Default viewport, if none else is set.
      D3D12_RECT scissor_rect_; //!< Default scissor rect, if none else is set.

      D3D12Device device_; //!< The DirectX 12 device.
    };

  }
}