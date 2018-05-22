//#include <engine/component/camera_manager.h>
//#include <engine/component/system_manager.h>

#include "graphics/vulkan/vulkan_renderer.h"


namespace sulphur
{
    namespace engine
    {
    VulkanRenderer::VulkanRenderer(engine::World& world) :
      IRenderer(world)
      //camera_manager_(*systemManager.GetManager<CameraManager>())
    {
      // TODO: Initialize variables and device if possible
    }

    VulkanRenderer::~VulkanRenderer()
    {
      OnDestroy();
    }

    void VulkanRenderer::OnInitialize(engine::NativeWindowHandle hWnd, glm::ivec2 screen_size, bool vsync)
    {
      // Initialize everything else
      initialized_ = true;
    }

    void VulkanRenderer::OnDestroy()
    {
      // TODO: Cleanup if not done yet
    }

    void VulkanRenderer::OnUpdate()
    {
      // Probably no need for this one... But if you need it, it's there
    }

    void VulkanRenderer::OnResizeWindow(uint width, uint height)
    {
      // TODO: Resize stuff
    }

    void VulkanRenderer::StartFrame()
    {
      // TODO: Prepare renderer for a new frame
      // Reset pipeline state to default
      // Reset vertex counter
      // etc..
      // IMPORTANT: DO NOT CLEAR THE RENDER TARGETS, THIS IS DONE BY THE CAMERA'S
    }

    void VulkanRenderer::EndFrame(bool present)
    {
      // Present();
    }

    void VulkanRenderer::SetMaterial(const engine::MaterialPass& material)
    {
      // TODO:

      // SetShader(material->GetShader());

      // auto textures = material->GetTextures();
      // for(int i = 0; i < textures.size()){
      //     SetTexture(i, textures[i]);
      // }
    }

    void VulkanRenderer::SetCamera(const CameraHandle& camera)
    {
      // TODO:

      // SetViewport(camera->GetViewport());
      // SetScissorRect(camera->GetBounds());

      // DepthBufferHandle depth_buffer = camera->GetDepthBuffer();
      // if(depth_buffer)
      //   SetDepthBuffer(depth_buffer);
      // else
      //   SetDepthBuffer(nullptr);

      // RenderTargetHandle render_target = camera->GetRenderTarget();
      // if(render_target)
      //   SetRenderTarget(0, render_target);
      // else
      //   Use the back buffer or gbuffer in case of defered rendering
    }


    void VulkanRenderer::SetModelMatrix(const glm::mat4& model_matrix)
    {
      // TODO:
      // CameraHandle camera = camera_manager_.CurrentCamera();

      //if (!camera)
      //	return;

      // Setup constant buffer's mvp matrix
      // m = model_matrix;
      // v = camera->GetViewMatrix();
      // p = camera->GetProjectionMatrix();
    }

    void VulkanRenderer::SetScissorRect(const glm::vec4& rect)
    {
      // TODO: Set scissor rect if changed
    }

    void VulkanRenderer::SetViewport(const glm::vec4& rect)
    {
      // TODO: Set viewport rect if changed
    }

    void VulkanRenderer::Draw()
    {
      // Probably could do some kind of verification in debug mode

      // Execute draw command
    }

    void VulkanRenderer::Dispatch(ComputeQueueType type, int x, int y, int z)
    {
      // TODO: Process compute shader dispatch call
    }

    void VulkanRenderer::SetVsync(bool value)
    {
      // TODO: Update renderer to use vsync value
      vsync_ = value;
    }

    void VulkanRenderer::SetMesh(const MeshHandle& mesh)
    {
      // GPUHandle handle = mesh->GetGPUHandle();
      // if(!handle){
      //     LoadMesh(mesh);
      //     handle = mesh->GetGPUHandle();
      // }

      // Find mesh using 'handle'
      // Use mesh's vertex/index buffers
    }

    void VulkanRenderer::SetShader(const ShaderHandle& shader)
    {
      // TODO:

      // SetPipelineState(shader->GetPipelineState());

      // GPUHandle handle = shader->GetGPUHandle();
      // if(!handle){
      //     LoadShader(shader);
      //     handle = shader->GetGPUHandle();
      // }

      // Find shader using 'handle'
      // Use this shader
    }

    void VulkanRenderer::SetTexture(int register_id, const TextureHandle& texture)
    {
      // TODO:

      // GPUHandle handle = texture->GetGPUHandle();
      // if(!handle){
      //     LoadTexture(texture);
      //     handle = texture->GetGPUHandle();
      // }

      // Find texture using 'handle'
      // Use this texture at register 'registerId'
    }

    void VulkanRenderer::SetRenderTarget(int register_id, const RenderTargetHandle & render_target)
    {
      // TODO: Validate render target
      // Use this render target at register 'registerId'
    }

    void VulkanRenderer::SetPipelineState(const PipelineState & pipeline_state)
    {
      // TODO: Update pipeline state to match 'pipelineState'
    }

    void VulkanRenderer::LoadTexture(const TextureHandle& texture)
    {
      // GPUHandle handle = texture->GetGPUHandle();
      // if(!handle){
      //     Generate GPU resource
      //     texture->SetGPUHandle('pointer to the generated resource');
      // }
    }

    void VulkanRenderer::LoadMesh(const MeshHandle& mesh)
    {
      // GPUHandle handle = mesh->GetGPUHandle();
      // if(!handle){
      //     Generate GPU resource
      //     mesh->SetGPUHandle('pointer to the generated resource');
      // }
    }

    void VulkanRenderer::LoadShader(const ShaderHandle& shader)
    {
      // GPUHandle handle = shader->GetGPUHandle();
      // if(!handle){
      //     Generate GPU resource
      //     shader->SetGPUHandle('pointer to the generated resource');
      // }
    }
    }
}
