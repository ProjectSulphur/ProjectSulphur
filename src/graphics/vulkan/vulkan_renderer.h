#pragma once
#include <engine/fwd.h>
#include <engine/graphics/irenderer.h>

#include "graphics/vulkan/wulkan_fwd.h"

namespace sulphur
{
  namespace graphics
  {

    class VulkanRenderer : public engine::IRenderer
    {
    public:
      VulkanRenderer(engine::World& world);
      ~VulkanRenderer();

      // Events
      bool IsInitialized() { return initialized_; };
      void OnInitialize(engine::NativeWindowHandle hWnd, glm::ivec2 screen_size = glm::ivec2(1280, 720), bool vsync = false) override;
      void OnDestroy() override;
      void OnUpdate() override;
      void OnResizeWindow(uint width, uint height) override;
      void StartFrame() override;
      void EndFrame(bool present = true) override;

      // Processing draws
      void SetMesh(const engine::MeshHandle& mesh) override;
      void SetMaterial(const engine::MaterialPass& material) override;
      void SetCamera(const engine::CameraHandle& camera) override;
      void SetModelMatrix(const glm::mat4& model_matrix) override;
      void SetScissorRect(const glm::vec4& rect) override;
      void SetViewport(const glm::vec4& rect) override;

      // Execute
      void Draw() override;
      void Dispatch(engine::ComputeQueueType type, int x, int y, int z) override;

      // Settings
      void SetVsync(bool value) override;

    protected:
      // Setting GPU resources
      void SetShader(const engine::ShaderHandle& shader);
      void SetTexture(int register_id, const engine::TextureHandle& texture);
      void SetRenderTarget(int register_id, const engine::RenderTargetHandle& render_target);
      void SetPipelineState(const engine::PipelineState& pipeline_state);

      // Loading GPU resources
      void LoadTexture(const engine::TextureHandle& texture);
      void LoadMesh(const engine::MeshHandle& mesh);
      void LoadShader(const engine::ShaderHandle& shader);


    private:
      bool initialized_, vsync_;

      //engine::CameraManager& camera_manager_;
    };

  }
}
