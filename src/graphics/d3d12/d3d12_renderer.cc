#include "d3d12_renderer.h"

#include "graphics/d3d12/d3d12_vertex.h"
#include "graphics/d3d12/d3d12_constant_buffer_heap.h"
#include "graphics/d3d12/d3d12_persistent_descriptor_heap.h"
#include "graphics/d3d12/d3d12_root_signature.h"
#include "graphics/d3d12/assets/d3d12_asset_manager.h"
#include "graphics/d3d12/assets/d3d12_asset.h"
#include "graphics/d3d12/d3d12_imgui.h"
#include "graphics/d3d12/d3dx12.h"

// TODO: Shaders - remove when asset pipeline is functional
#include "d3d12_default_pixel.h"
#include "d3d12_default_vertex.h"

#include <engine/assets/depth_buffer.h>
#include <engine/assets/render_target.h>
#include <engine/assets/mesh.h>
#include <engine/assets/material.h>
#include <engine/assets/shader.h>
#include <engine/graphics/imgui/custom_config.h>

#include <foundation/application/native_window_handle.h>
#include <foundation/utils/timer.h>
#include <foundation/containers/vector.h>
#include <foundation/logging/logger.h>

#include <experimental/filesystem>
#include <d3dcompiler.h>

#include <sstream>

#include "DXGIDebug.h"

namespace sulphur
{
  namespace graphics
  {
    // TODO: Remove this hacky thingie <3
    /**
    * @struct sulphur::graphics::SceneCB
    * @brief Temporary constant buffer struct
    * @author Yana Mateeva
    */
    struct SceneCB
    {
      glm::mat4 model; //!< Model matrix
      glm::mat4 view; //!< View matrix
      glm::mat4 projection; //!< Projection matrix
      float time; //!< Total time passed
      float padding[15]; //!< Padding to align size to 256 bytes
    } g_scene_buffer; //!< The per scene constant buffer 

    static_assert(
      sizeof(SceneCB) % 256 == 0,
      "Size of constant buffer structure should be multiple of 256 bytes."
      );

    //------------------------------------------------------------------------------------------------------
    /**
    * @brief Generates an error message string from an HRESULT and prints it
    * @param[in] hr (long) The error code
    */
    void GenerateErrorMessage(long hr)
    {
      LPSTR messageBuffer = nullptr;
      size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&messageBuffer),
        0,
        NULL
      );

      std::string message(messageBuffer, size);

      //Free the buffer.
      LocalFree(messageBuffer);

      std::ostringstream oss;
      oss << "Unable to create Direct3D environment.\nError code: 0x"
        << std::hex
        << hr
        << std::endl
        << message
        << std::endl;

      PS_LOG(Error, oss.str().c_str());
    }

    //------------------------------------------------------------------------------------------------------
    D3D12Renderer::D3D12Renderer() :
      total_time_(0.0f),
      initialized_(false),
      vsync_(false),
      fence_values_{ 0 },
      frame_descriptor_heap_(device_, device_.persistent_descriptor_heap()),
      pso_manager_(device_),
      material_manager_(device_)
    {
      // TODO: Initialize variables and device if possible
      // Enable debug layer
#if defined (_DEBUG)
      {
        ID3D12Debug* debug_controller;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller))))
        {
          debug_controller->EnableDebugLayer();
          debug_controller->Release();
        }
      }
#endif

      device_.Create();
    }

    //------------------------------------------------------------------------------------------------------
    D3D12Renderer::~D3D12Renderer()
    {
      OnDestroy();
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::OnInitialize(
      foundation::NativeWindowHandle hWnd,
      const glm::ivec2& screen_size,
      bool vsync)
    {
      // Create constant buffer heap
      constant_buffer_heap_.Create(device_);

      // Create frame descriptor heap
      frame_descriptor_heap_.Initialize(BACK_BUFFER_COUNT, 1024, 512, 512, 512);


      
      // TODO: Remove temporary viewport / scissor rect
      viewport_ = 
        CD3DX12_VIEWPORT(0.0f, 0.0f, (float)screen_size.x, (float)screen_size.y, 0.0f, 1.0f);
      scissor_rect_ = 
        CD3DX12_RECT(0, 0, (LONG)screen_size.x, (LONG)screen_size.y);

      // Create direct command queue
      if (device_.CreateCommandQueue(direct_command_queue_) == false)
      {
        PS_LOG(Error, "Failed to create d3d12 command queue.\n");
      }

      // Create swap chain
      {
        IDXGIFactory4* factory;
        CreateDXGIFactory1(IID_PPV_ARGS(&factory));

        DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
        swap_chain_desc.BufferCount = BACK_BUFFER_COUNT;
        swap_chain_desc.Width = screen_size.x;
        swap_chain_desc.Height = screen_size.y;
        swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swap_chain_desc.SampleDesc.Count = 1;
        swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

        if (hWnd.type != foundation::NativeWindowHandle::Type::kWin32)
        {
          PS_LOG(Error, "Wrong native handle passed to DirectX 12 renderer.\n");
          return;
        }

        HRESULT hresult;

        hresult = factory->CreateSwapChainForHwnd(
          direct_command_queue_,
          (HWND)hWnd.win32_window,
          &swap_chain_desc,
          nullptr,
          nullptr,
          (IDXGISwapChain1**)&swap_chain_
        );

        if (FAILED(hresult))
        {
          // TODO(Yana): Replace with foundation::Logger
          printf("Failed to create a swap chain.\n");
          return;
        }

        factory->Release();
      }
      // Create render targets and command allocators
      {
        D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        rtv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

        for (uint8_t i = 0; i < BACK_BUFFER_COUNT; ++i)
        {
          if (FAILED(swap_chain_->GetBuffer(i, IID_PPV_ARGS(&OM_render_targets_[i]))))
          {
            PS_LOG(Error, "Failed to get back buffers from the swap chain.\n");
            return;
          }

          device_.CreateRenderTargetView(
            OM_render_targets_[i],
            rtv_desc,
            persistent_render_target_handles_[i]
          );

          if (device_.CreateCommandAllocator(direct_command_allocators_[i]) == false)
          {
            PS_LOG(Error, "Failed to create command allocator.\n");
            return;
          }
        }
      }

      // Create depth buffer
      D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
      depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
      depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
      depthOptimizedClearValue.DepthStencil.Stencil = 0;

      for (uint32_t i = 0; i < BACK_BUFFER_COUNT; ++i)
      {
        device_.persistent_descriptor_heap().AllocateDSVDescriptor(
          persistent_depth_buffer_handles_[i]
        );

        if (device_.CreateCommittedResource(
          depth_buffer_[i],
          CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
          D3D12_HEAP_FLAG_NONE,
          CD3DX12_RESOURCE_DESC::Tex2D(
            DXGI_FORMAT_D32_FLOAT,
            screen_size.x,
            screen_size.y,
            1,
            0, 
            1,
            0,
            D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
          ),
          D3D12_RESOURCE_STATE_DEPTH_WRITE,
          &depthOptimizedClearValue) == false)
        {
          PS_LOG(Error, "Failed to create depth buffer.\n");
          return;
        }

        D3D12_CPU_DESCRIPTOR_HANDLE dsv_cpu_handle;
        device_.persistent_descriptor_heap().GetCPUHandleForDSVDescriptor(
          persistent_depth_buffer_handles_[i],
          dsv_cpu_handle
        );

        device_.CreateDepthStencilView(
          depth_buffer_[i],
          DXGI_FORMAT_D32_FLOAT,
          D3D12_DSV_DIMENSION_TEXTURE2D,
          D3D12_DSV_FLAG_NONE,
          dsv_cpu_handle
        );
      }
      // Create root signature
      {
        D3D12_DESCRIPTOR_RANGE1 desc_ranges[] =
        {
          { D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, 0 }
        };

        CD3DX12_ROOT_PARAMETER1 root_parameters[2];
        root_parameters[0].InitAsConstantBufferView(
          0,
          0,
          D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
          D3D12_SHADER_VISIBILITY_ALL
        );
        root_parameters[1].InitAsDescriptorTable(
          _countof(desc_ranges),
          desc_ranges,
          D3D12_SHADER_VISIBILITY_PIXEL
        );

        D3D12_STATIC_SAMPLER_DESC sampler_desc = {};
        sampler_desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler_desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler_desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler_desc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        sampler_desc.MaxAnisotropy = 0;
        sampler_desc.MipLODBias = 0;
        sampler_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        sampler_desc.MinLOD = 0;
        sampler_desc.MaxLOD = D3D12_FLOAT32_MAX;
        sampler_desc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        sampler_desc.RegisterSpace = 0;
        sampler_desc.ShaderRegister = 0;
        sampler_desc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc = {};
        desc.Init_1_1(
          _countof(root_parameters),
          root_parameters,
          1,
          &sampler_desc,
          D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
        );

        if (device_.CreateVersionedRootSignature(root_signature_, desc) == false)
        {
          PS_LOG(Error, "Failed to create versioned root signature.\n");
          return;
        }

      }

      // Create a default pipeline state and set it
      PipelineState pipeline_state;
      pso_manager_.SetPipelineState(
        pipeline_state,
        root_signature_,
        CD3DX12_SHADER_BYTECODE(default_vs, sizeof(default_vs)),
        CD3DX12_SHADER_BYTECODE(default_ps, sizeof(default_ps))
      );

      // Create command list
      if (device_.CreateGraphicsCommandList(
        direct_command_list_,
        direct_command_allocators_[0],
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        nullptr) == false)
      {
        PS_LOG(Error, "Failed to create d3d12 command list.\n");
        return;
      }

      // Close command list
      direct_command_list_->Close();

      // Create synchronization objects
      if (device_.CreateFence(fence_, 0) == false)
      {
        PS_LOG(Error, "Failed to create d3d12 fence object.\n");
        return;
      }

      fence_event_ = CreateEventA(nullptr, false, false, nullptr);

      if (fence_event_ == NULL)
      {
        PS_LOG(Error, "Failed to create d3d12 fence event.\n");
        return;
      }

      current_frame_index_ = swap_chain_->GetCurrentBackBufferIndex();

      ImguiCustomConfig();

      if (ImGui_ImplDX12_Init(hWnd.win32_window, device_, direct_command_list_) == false)
      {
        PS_LOG(Error, "Failed to initialize imgui.\n");
        return;
      }

      //SetWindowLongPtr((HWND)hWnd.win32_window, GWLP_WNDPROC, (LONG_PTR)&ImGui_Impl_WndProcHandler);

      ImGui_Impl_NewFrame();

      vsync_ = vsync;
      initialized_ = true;
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::OnDestroy()
    {
      if (initialized_ == false)
        return;
      initialized_ = false;

      // Release assets
      current_draw_call_.current_mesh = engine::MeshHandle();
      current_draw_call_.current_material = engine::MaterialPass();

      ImGui_Impl_Shutdown();

      SafeRelease(fence_);
      SafeRelease(root_signature_);

      for (uint8_t i = 0; i < BACK_BUFFER_COUNT; ++i)
      {
        SafeRelease(depth_buffer_[i]);
        SafeRelease(OM_render_targets_[i]);
      }

      SafeRelease(swap_chain_);

      for (uint8_t i = 0; i < BACK_BUFFER_COUNT; ++i)
      {
        SafeRelease(direct_command_allocators_[i]);
      }

      SafeRelease(direct_command_list_);
      SafeRelease(direct_command_queue_);

      pso_manager_.OnDestroy();
      material_manager_.OnDestroy();
      frame_descriptor_heap_.OnDestroy();
      constant_buffer_heap_.OnDestroy();

      device_.OnDestroy();

      // Libs can be removed if `dxgi_debug` is disabled
      /*#pragma comment(lib, "dxguid.lib")
      #pragma comment(lib, "dxgi.lib")

      IDXGIDebug* dxgi_debug;
      if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgi_debug))))
      {
        dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
      }
      SafeRelease(dxgi_debug);
      */
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::OnUpdate()
    {
      // Probably no need for this one... But if you need it, it's there
      total_time_ += 1.0f / 300.0f;
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::OnResizeWindow(uint, uint)
    {
      // TODO: Resize stuff
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::StartFrame()
    {
      // TODO: Prepare renderer for a new frame
      // Reset pipeline state to default
      // Reset vertex counter
      // etc..
      // IMPORTANT: DO NOT CLEAR THE RENDER TARGETS, THIS IS DONE BY THE CAMERA'S

      //current_frame_index_ = swap_chain_->GetCurrentBackBufferIndex();
      frame_descriptor_heap_.StartFrame(current_frame_index_);
      material_manager_.StartFrame();


      direct_command_allocators_[current_frame_index_]->Reset();
      direct_command_list_->Reset(direct_command_allocators_[current_frame_index_], nullptr);

      D3D12_RESOURCE_BARRIER render_target_barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        OM_render_targets_[current_frame_index_],
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);

      direct_command_list_->ResourceBarrier(1, &render_target_barrier);
      direct_command_list_->SetPipelineState(pso_manager_.pipeline_state_object());
      direct_command_list_->SetGraphicsRootSignature(root_signature_);

      direct_command_list_->RSSetViewports(1, &viewport_);
      direct_command_list_->RSSetScissorRects(1, &scissor_rect_);

      D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle;
      device_.persistent_descriptor_heap().GetCPUHandleForRTVDescriptor(
        persistent_render_target_handles_[current_frame_index_],
        rtv_handle
      );

      D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle;
      device_.persistent_descriptor_heap().GetCPUHandleForDSVDescriptor(
        persistent_depth_buffer_handles_[current_frame_index_],
        dsv_handle
      );

      direct_command_list_->ClearDepthStencilView(
        dsv_handle,
        D3D12_CLEAR_FLAG_DEPTH,
        1.0f,
        0,
        0,
        nullptr
      );

      //direct_command_list_->OMSetRenderTargets(1, &rtv_handle, false, &dsv_handle);

      current_draw_call_.current_primitive_topology = TopologyType::kTriangle;
      direct_command_list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      // Set descriptor heaps
      ID3D12DescriptorHeap* pp_desc_heaps[]
      {
        frame_descriptor_heap_.srv_uav_heap(current_frame_index_)
      };
      direct_command_list_->SetDescriptorHeaps(_countof(pp_desc_heaps), pp_desc_heaps);

    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::EndFrame(bool present)
    {
      ImGui::Render();

      D3D12_RESOURCE_BARRIER present_barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        OM_render_targets_[current_frame_index_],
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);

      direct_command_list_->ResourceBarrier(1, &present_barrier);

      direct_command_list_->Close();

      ID3D12CommandList* pp_command_lists[] = { direct_command_list_ };

      direct_command_queue_->ExecuteCommandLists(_countof(pp_command_lists), pp_command_lists);

      if (present == false)
        return;

      // Present();
      if (vsync_ == true)
      {
        swap_chain_->Present(1, 0);
      }
      else
      {
        swap_chain_->Present(0, 0);
      }

      // TODO: Put this in a better place
      ImGui_Impl_NewFrame();

      //ImGui::ShowTestWindow();

      // sync -----------------------------------------------------------------------------------

      //Schedule signal
      const uint64_t current_fence_value = fence_values_[current_frame_index_];

      if (FAILED(direct_command_queue_->Signal(fence_, current_fence_value)))
      {
      }

      //Update frame index
      current_frame_index_ = swap_chain_->GetCurrentBackBufferIndex();

      //Wait
      if (fence_->GetCompletedValue() < fence_values_[current_frame_index_])
      {
        fence_->SetEventOnCompletion(fence_values_[current_frame_index_], fence_event_);
        WaitForSingleObjectEx(fence_event_, INFINITE, FALSE);
      }

      fence_values_[current_frame_index_] = current_fence_value + 1;

    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::SetMaterial(const engine::MaterialPass& material)
    {
      LoadShader(material.shader());
      const foundation::Vector<engine::TextureHandle>& textures = material.textures();

      for (size_t i = 0; i < textures.size(); ++i)
      {
        SetTexture(static_cast<int>(i), textures[i]);
      }

      material_manager_.SetMaterial(material);
      current_draw_call_.use_default_pipeline_state = true;
      current_draw_call_.current_material = material;
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::SetCamera(
      const glm::mat4& view,
      const glm::mat4& projection,
      const engine::DepthBuffer& depth_buffer,
      const engine::RenderTarget& render_target)
    {
      // Update constant buffer data
      {
        g_scene_buffer.view = view;
        g_scene_buffer.projection = projection;

        size_t offset;
        constant_buffer_heap_.Write(&g_scene_buffer, sizeof(g_scene_buffer), offset);
        direct_command_list_->SetGraphicsRootConstantBufferView(
          0,
          constant_buffer_heap_.GetGPUVirtualAddress() + offset
        );
      }

      D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle;
      D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle;

      // Set depth buffer, get a cpu handle
      SetDepthBuffer(depth_buffer);
      D3D12Texture2D* ds_tex = device_.texture_asset_manager().GetTexture(
        depth_buffer.buffer().GetGPUHandle()
      );
      device_.persistent_descriptor_heap().GetCPUHandleForDSVDescriptor(
        ds_tex->dsv_persistent_index_,
        dsv_handle);

      // Set render target, get a cpu handle. Use the back buffer
      // from the swap chain, if the render target type is kBackBuffer (main camera)
      if (render_target.render_target_type() == sulphur::engine::RenderTargetType::kBackBuffer)
      {
        device_.persistent_descriptor_heap().GetCPUHandleForRTVDescriptor(
          persistent_render_target_handles_[current_frame_index_],
          rtv_handle);
      }
      else
      {
        SetRenderTarget(0, render_target, foundation::Color::kWhite); // Is this really necessary in this function?

        D3D12Texture2D* rt_tex = device_.texture_asset_manager().GetTexture(
          render_target.GetTarget().GetGPUHandle()
        );
        device_.persistent_descriptor_heap().GetCPUHandleForRTVDescriptor(
          rt_tex->rtv_persistent_index_,
          rtv_handle);
      }

      direct_command_list_->OMSetRenderTargets(1, &rtv_handle, false, &dsv_handle);
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::SetModelMatrix(const glm::mat4& model_matrix)
    {
      // Setup constant buffer's MVP matrix
      g_scene_buffer.model = model_matrix;
      g_scene_buffer.time = total_time_;

      size_t offset;
      constant_buffer_heap_.Write(&g_scene_buffer, sizeof(g_scene_buffer), offset);
      direct_command_list_->SetGraphicsRootConstantBufferView(
        0,
        constant_buffer_heap_.GetGPUVirtualAddress() + offset
      );
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::SetScissorRect(const glm::vec4&)
    {
      // TODO: Set scissor rect if changed
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::SetViewport(const glm::vec4&)
    {
      // TODO: Set viewport rect if changed
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::ClearRenderTarget(
      const engine::RenderTarget& render_target,
      const foundation::Color& clear_color)
    {
      D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle;
      if (render_target.render_target_type() == engine::RenderTargetType::kBackBuffer)
      {
        device_.persistent_descriptor_heap().GetCPUHandleForRTVDescriptor(
          persistent_render_target_handles_[current_frame_index_],
          rtv_handle
        );
        direct_command_list_->ClearRenderTargetView(rtv_handle, clear_color.rgba, 0, nullptr);
      }
      else
      {
        SetRenderTarget(0, render_target, clear_color);
        D3D12Texture2D* rt_tex = device_.texture_asset_manager().GetTexture(
          render_target.GetTarget().GetGPUHandle()
        );
        device_.persistent_descriptor_heap().GetCPUHandleForRTVDescriptor(
          rt_tex->rtv_persistent_index_,
          rtv_handle
        );
      }

      direct_command_list_->ClearRenderTargetView(rtv_handle, clear_color.rgba, 0, nullptr);
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::ClearDepthBuffer(const engine::DepthBuffer& depth_buffer)
    {
      SetDepthBuffer(depth_buffer);
      D3D12Texture2D* depth_tex = device_.texture_asset_manager().GetTexture(
        depth_buffer.buffer().GetGPUHandle()
      );
      D3D12_CPU_DESCRIPTOR_HANDLE depth_handle;
      device_.persistent_descriptor_heap().GetCPUHandleForDSVDescriptor(
        depth_tex->dsv_persistent_index_,
        depth_handle
      );

      D3D12_CLEAR_FLAGS flags = D3D12_CLEAR_FLAG_DEPTH;

      if (depth_buffer.format() == engine::TextureFormat::kR24G8_TYPELESS)
      {
        flags |= D3D12_CLEAR_FLAG_STENCIL;
      }

      direct_command_list_->ClearDepthStencilView(
        depth_handle,
        flags,
        1.0f,
        0,
        0,
        nullptr
      );
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::Draw()
    {
      // Probably could do some kind of verification in debug mode

      // TODO: NEEDS WORK (JELLE check this out)
      if (!current_draw_call_.current_material.shader())
      {
        return;
      }

      engine::Shader* raw_shader =
        current_draw_call_.current_material.shader().GetRaw();
      engine::ShaderProgram* vertex_shader =
        raw_shader->vertex_shader().GetRaw();
      engine::ShaderProgram* pixel_shader =
        raw_shader->pixel_shader().GetRaw();

      CD3DX12_SHADER_BYTECODE vert(
        vertex_shader->byte_code_data(),
        vertex_shader->byte_code_size()
      );

      CD3DX12_SHADER_BYTECODE pix(
        pixel_shader->byte_code_data(),
        pixel_shader->byte_code_size()
      );

      D3D12RootSignature* root_sig = device_.shader_asset_manager().GetRootSignatureForShader(
        current_draw_call_.current_material.shader().GetGPUHandle()
      );

      // Update topology type
      if (current_draw_call_.current_primitive_topology != 
        current_draw_call_.current_mesh->topology_type())
      {
        current_draw_call_.current_primitive_topology = 
          current_draw_call_.current_mesh->topology_type();

        switch (current_draw_call_.current_primitive_topology)
        {
        case TopologyType::kPoint:
          direct_command_list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
          break;
        case TopologyType::kLine:
          direct_command_list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
          break;
        case TopologyType::kTriangle:
        default:
          direct_command_list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
          break;
        }
      }

      if (current_draw_call_.use_default_pipeline_state == true)
      {
        raw_shader->pipeline_state.topology_type =
          current_draw_call_.current_primitive_topology;

        pso_manager_.SetPipelineState(
          raw_shader->pipeline_state,
          current_draw_call_.current_material.shader()
        );
      }
      else
      {
        current_draw_call_.current_pipeline_state.topology_type =
          current_draw_call_.current_primitive_topology;

        pso_manager_.SetPipelineState(
          current_draw_call_.current_pipeline_state,
          current_draw_call_.current_material.shader()
        );
      }

      direct_command_list_->SetGraphicsRootSignature(root_sig->root_signature());
      direct_command_list_->SetPipelineState(pso_manager_.pipeline_state_object());

      // Set material
      material_manager_.current_material()->CopyDescriptorsToFrameDescriptorHeap(
        frame_descriptor_heap_
      );

      direct_command_list_->SetGraphicsRootDescriptorTable(
        1,
        material_manager_.current_material()->srv_descriptor_table_handle()
      );


      // Draw mesh
      D3D12Mesh* mesh_data = device_.mesh_asset_manager().GetMesh(
        current_draw_call_.current_mesh.GetGPUHandle()
      );

      direct_command_list_->IASetVertexBuffers(0, 1, &mesh_data->vb_view_);
      direct_command_list_->IASetIndexBuffer(&mesh_data->ib_view_);

      // Execute draw command
      direct_command_list_->DrawIndexedInstanced(
        static_cast<UINT>(mesh_data->index_count_),
        1,
        0,
        0,
        0
      );
      // TODO: Remove these commands, they're a test
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::Dispatch(ComputeQueueType, uint, uint, uint)
    {
      // TODO: Process compute shader dispatch call
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::SetVsync(bool value)
    {
      // TODO: Update renderer to use vsync value
      vsync_ = value;
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::SetMesh(const engine::MeshHandle& mesh)
    {
      engine::GPUAssetHandle handle = mesh.GetGPUHandle();
      if (!handle)
      {
        LoadMesh(mesh);
        handle = mesh.GetGPUHandle();
      }

      if (mesh->HasChanged())
      {
        UpdateDynamicMesh(mesh);
      }

      current_draw_call_.current_mesh = mesh;

    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::SetPipelineState(const PipelineState& pipeline_state)
    {
      current_draw_call_.current_pipeline_state = pipeline_state;
      current_draw_call_.use_default_pipeline_state = false;
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::SetTexture(int /*register_id*/, const engine::TextureHandle& texture)
    {
      engine::GPUAssetHandle handle = texture.GetGPUHandle();
      if (!handle)
      {
        LoadTexture(texture, D3D12TextureType::kTexture);
        handle = texture.GetGPUHandle();
      }

      // Find texture using 'handle'
      // Use this texture at register 'registerId'
      D3D12Texture2D* texture_data = device_.texture_asset_manager().GetTexture(handle);

      if (texture_data->has_srv_ == false)
      {
        device_.CreateShaderResourceView(texture_data);
      }

      D3D12_RESOURCE_BARRIER psr_barrier;
      if (texture_data->Transition(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, psr_barrier))
      {
        direct_command_list_->ResourceBarrier(1, &psr_barrier);
      }

    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::SetRenderTarget(
      int,
      const engine::RenderTarget& render_target,
      const foundation::Color& clear_color)
    {
      // TODO: Validate render target
      engine::TextureHandle tex_handle = render_target.GetTarget();
      engine::GPUAssetHandle gpu_handle = tex_handle.GetGPUHandle();

      if (!gpu_handle)
      {
        LoadTexture(tex_handle, D3D12TextureType::kRenderTarget, clear_color);
        gpu_handle = tex_handle.GetGPUHandle();
      }

      D3D12Texture2D* texture_data = device_.texture_asset_manager().GetTexture(gpu_handle);

      if (texture_data->has_rtv_ == false)
      {
        device_.CreateRenderTargetView(texture_data);
      }

      D3D12_RESOURCE_BARRIER render_target_barrier;
      if (texture_data->Transition(D3D12_RESOURCE_STATE_RENDER_TARGET, render_target_barrier))
      {
        direct_command_list_->ResourceBarrier(1, &render_target_barrier);
      }

      // Use this render target at register 'registerId'
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::SetDepthBuffer(const engine::DepthBuffer& depth_buffer)
    {
      engine::GPUAssetHandle gpu_handle = depth_buffer.buffer().GetGPUHandle();

      if (!gpu_handle)
      {
        LoadTexture(depth_buffer.buffer(), D3D12TextureType::kDepthStencil);
        gpu_handle = depth_buffer.buffer().GetGPUHandle();
      }

      D3D12Texture2D* texture_data = device_.texture_asset_manager().GetTexture(gpu_handle);

      if (texture_data->has_dsv_ == false)
      {
        device_.CreateDepthStencilView(texture_data);
      }

      D3D12_RESOURCE_BARRIER depth_barrier;
      if (texture_data->Transition(D3D12_RESOURCE_STATE_DEPTH_WRITE, depth_barrier))
      {
        direct_command_list_->ResourceBarrier(1, &depth_barrier);
      }
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::LoadTexture(
      const engine::TextureHandle& texture,
      const D3D12TextureType type,
      foundation::Color clear_color)
    {
      engine::GPUAssetHandle& handle = texture.GetGPUHandle();
      if (!handle)
      {
        device_.texture_asset_manager().Create(texture, type, clear_color);
      }
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::LoadMesh(const engine::MeshHandle& mesh)
    {
      engine::GPUAssetHandle& handle = mesh.GetGPUHandle();
      if (!handle)
      {
        device_.mesh_asset_manager().Create(mesh);
      }
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::LoadShader(const engine::ShaderHandle& shader)
    {
      engine::GPUAssetHandle handle = shader.GetGPUHandle();
      if(!handle)
      {
        device_.shader_asset_manager().Create(shader);
      }
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Renderer::UpdateDynamicMesh(const engine::MeshHandle&)
    {

    }
  }
}