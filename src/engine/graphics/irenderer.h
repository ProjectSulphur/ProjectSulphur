#pragma once

#include "engine/assets/mesh.h"
#include "engine/application/window.h"

#include <graphics/platform/pipeline_state.h>

namespace sulphur
{
  namespace foundation
  {
    struct Color;
    struct NativeWindowHandle;
  }
}

namespace sulphur 
{
  namespace engine 
  {
    class MaterialPass;
    class ComputePass;
    class RenderTarget;
    class DepthBuffer;

    /**
    * @class sulphur::engine::IRenderer
    * @brief An interface that all renderers implement for engine to low level graphics API communication
    * @author Jelle de Haan
    */
    class IRenderer
    {
    public:
      /**
      * @brief Constructor.
      */
      IRenderer() {};

      /**
      * @brief Virtual destructor.
      */
      virtual ~IRenderer() {};

      /**
      * @brief Initializes the renderer.
      * @param[in] window_handle (void*) The window to initialize the renderer with
      * @param[in] screen_size (const glm::ivec2&) The size of the renderable portion of the window.
      * @param[in] vsync (bool) Should vsync be enabled?
      */
      virtual void OnInitialize(void* window_handle, const glm::ivec2& screen_size = glm::ivec2(1280, 720), bool vsync = false) = 0;
      
      /**
      * @brief Shuts down the renderer. Releases all GPU resources.
      */
      virtual void OnDestroy() = 0;

      /**
      * @brief This function is called on every update tick of the application. (TODO: Remove this).
      */
      virtual void OnUpdate() = 0;

      /**
      * @brief Recreates the swap chain with a new size for the back buffers.
      * @param[in] width (uint) The new width of the window.
      * @param[in] height (uint) The new height of the window.
      */
      virtual void OnResizeWindow(uint width, uint height) = 0;
      
      /**
      * @brief Prepares the pipeline for a new frame.
      */
      virtual void StartFrame() = 0;

      /**
      * @brief Ends the frame for the rendering pipeline and presents the frame to the screen.
      * @param[in] present (bool) Should the frame be rendered on the screen?
      */
      virtual void EndFrame(bool present = true) = 0;

      /**
      * @see sulphur::engine::IRenderer::Draw
      * @brief Sets a mesh to render in the next Draw() call.
      * @param[in] mesh (const sulphur::engine::MeshHandle&) A handle to the mesh to render.
      */
      virtual void SetMesh(const MeshHandle& mesh) = 0;

      /**
      * @brief Sets the bone matrices to be bound to the pipeline
      * @param[in] bone_matrices (const foundation::Vector<glm::mat4>&) The bone matrices to be bound.
      */
      virtual void SetBoneMatrices(const foundation::Vector<glm::mat4>& bone_matrices) = 0;

      /**
      * @see sulphur::engine::IRenderer::Draw
      * @brief Sets the material to be used for the next Draw() call.
      * @param[in] material (const sulphur::engine::MaterialPass&) The material to set.
      */
      virtual void SetMaterial(const MaterialPass& material) = 0;
      
      /**
      * @see sulphur::engine::IRenderer::Dispatch
      * @brief Sets the compute pass to be used for the next Dispatch() call.
      * @param[in] material (const sulphur::engine::ComputePass&) The compute pass to set.
      */
      virtual void SetComputePass(const ComputePass& pass) = 0;

      /**
      * @brief Sets the current pipeline state.
      * @param[in] pipeline_state (const sulphur::graphics::PipelineState&) The pipeline state to set.
      */
      virtual void SetPipelineState(const graphics::PipelineState& pipeline_state) = 0;

      /**
      * @brief Sets the current camera to use for rendering.
      * @param[in] view (const glm::mat4& view) The view-matrix to use.
      * @param[in] projection (const glm::mat4&) The projection-matrix to use.
      * @param[in] depth_buffer (const sulphur::engine::DepthBuffer&) The depth buffer to use for depth-testing.
      * @param[in] render_target (const sulphur::engine::RenderTarget&) The render target to use for rendering.
      */
      virtual void SetCamera(const glm::mat4& view, const glm::mat4& projection, const engine::DepthBuffer& depth_buffer, const engine::RenderTarget& render_target) = 0;

      /**
      * @see sulphur::engine::IRenderer::Draw
      * @brief Sets the model matrix to use for the next Draw() call.
      * @param[in] model_matrix (const glm::mat4&) The model matrix to set.
      */
      virtual void SetModelMatrix(const glm::mat4& model_matrix) = 0;

      /**
      * @brief Sets the currently used scissor rect in the rasterizer state.
      * @param[in] rect (const glm::vec4&) The rectangle to use as the new scissor rect
      */
      virtual void SetScissorRect(const glm::vec4& rect) = 0;

      /**
      * @brief Sets the currently used viewport in the rasterizer state.
      * @param[in] rect (const glm::vec4&) The rectangle to use as the new viewport.
      */
      virtual void SetViewport(const glm::vec4& rect) = 0;

      // Execute
      /**
      * @brief Clears the specified render target with the provided color.
      * @param[in] render_target (const sulphur::engine::RenderTarget&) The render target to clear.
      * @param[in] clear_color (const sulphur::foundation::Color&) The color to clear the render target to.
      */
      virtual void ClearRenderTarget(const RenderTarget& render_target, const foundation::Color& clear_color) = 0;

      /**
      * @brief Clears the specified depth buffer to default values.
      * @param[in] depth_buffer (const sulphur::engine::DepthBuffer&) The depth buffer to clear.
      */
      virtual void ClearDepthBuffer(const DepthBuffer& depth_buffer) = 0;

      /**
      * @brief Records a draw command.
      */
      virtual void Draw(uint index_count = 0, uint index_offset = 0) = 0;

      /**
      * @brief Copies the render target contents onto the back buffer, overwriting it
      */
      virtual void CopyToScreen(const RenderTarget& render_target) = 0;

      /**
      * @brief Records a dispatch command on the specified compute queue.
      * @param[in] type (sulphur::graphics::ComputeQueueType) The type of compute dispatch
      * @param[in] x (uint) The number of groups to dispatch in the x direction.
      * @param[in] y (uint) The number of groups to dispatch in the y direction.
      * @param[in] z (uint) The number of groups to dispatch in the z direction.
      */
      virtual void Dispatch(graphics::ComputeQueueType type, uint x, uint y, uint z) = 0;

      // Settings
      /**
      * @brief Sets the vsync setting on the renderer.
      * @param[in] value (bool) Should vsync be used?
      */
      virtual void SetVsync(bool value) = 0;

      /**
      * @brief Sets the stencil ref setting on the renderer.
      * @param[in] value (uint) What value should be written to the stencil buffer.
      */
      virtual void SetStencilRef(uint value) = 0;
    };

  }
}