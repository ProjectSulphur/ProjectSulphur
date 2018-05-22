#include "d3d12_imgui.h"

#include "d3d12_device_wrapper.h"
#include "d3dx12.h"
#include "graphics/d3d12/assets/d3d12_asset.h"

#include <foundation/logging/logger.h>

#include <d3dcompiler.h>
#include <cstdint>

namespace sulphur
{
  namespace graphics
  {
    static HWND g_hwnd;
    static D3D12Device* g_device;
    static ID3D12PipelineState* g_pso;
    static ID3D12RootSignature* g_root_signature;
    static ID3D12GraphicsCommandList* g_command_list;
    static ID3D12Resource* g_upload_buffer;
    static D3D12Texture2D* g_font_texture;
    static ID3D12DescriptorHeap* g_srv_desc_heap;
    static uint8_t* g_mapped_buffer;

    static INT64 g_time;
    static INT64 g_ticks_per_second;

    constexpr uint64_t IMGUI_GPU_BUFFER_SIZE = 1024u * 1024u; //!< Size of the imgui GPU buffer.

    static bool g_initialized = false;

    /**
    * @brief Creates an imgui fonts texture.
    * @return (bool) Was it successful?
    */
    bool ImGui_Impl_CreateFontsTexture();

    //------------------------------------------------------------------------------------------------------
    /**
    * @brief Records draw commands for imgui.
    * @param[in] draw_data (ImDrawData*) The draw data for this frame.
    */
    void ImGui_ImplDX12_RenderDrawLists(ImDrawData* draw_data)
    {
      static D3D12_GPU_VIRTUAL_ADDRESS buffer_address = g_upload_buffer->GetGPUVirtualAddress();

      uint8_t* write_pointer = g_mapped_buffer;

      //float translate = -0.5f * 2.f;
      const float L = 0.f;
      const float R = ImGui::GetIO().DisplaySize.x;
      const float B = ImGui::GetIO().DisplaySize.y;
      const float T = 0.f;
      const float mvp[4][4] =
      {
        { 2.0f / (R - L), 0.0f, 0.0f, 0.0f},
        { 0.0f, 2.0f / (T - B), 0.0f, 0.0f, },
        { 0.0f, 0.0f, 0.5f, 0.0f },
        { (R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f },
      };

      g_command_list->SetGraphicsRootSignature(g_root_signature);
      g_command_list->SetPipelineState(g_pso);

      memcpy(write_pointer, &mvp[0], sizeof(mvp));

      g_command_list->SetDescriptorHeaps(1, &g_srv_desc_heap);
      g_command_list->SetGraphicsRootDescriptorTable(
        0,
        g_srv_desc_heap->GetGPUDescriptorHandleForHeapStart()
      );

      // Set constant buffer
      g_command_list->SetGraphicsRootConstantBufferView(1, buffer_address);
      write_pointer += sizeof(mvp);

      // Copy the vertices and indices for each command list
      for (int n = 0; n < draw_data->CmdListsCount; ++n)
      {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        size_t vertices_count = cmd_list->VtxBuffer.size();
        size_t indices_count = cmd_list->IdxBuffer.size();
        size_t vertices_size = vertices_count * sizeof(ImDrawVert);
        size_t indices_size = indices_count * sizeof(ImDrawIdx);

        // Copy the vertex data
        memcpy(write_pointer, &cmd_list->VtxBuffer[0], vertices_size);
        write_pointer += vertices_size;

        // Copy the index data
        memcpy(write_pointer, &cmd_list->IdxBuffer[0], indices_size);
        write_pointer += indices_size;
      }

      D3D12_VIEWPORT viewport;
      viewport.Width = ImGui::GetIO().DisplaySize.x;
      viewport.Height = ImGui::GetIO().DisplaySize.y;
      viewport.MinDepth = 0.f;
      viewport.MaxDepth = 1.f;
      viewport.TopLeftX = 0.f;
      viewport.TopLeftY = 0.f;

      g_command_list->RSSetViewports(1, &viewport);
      // NOT SETTING THE RENDER TARGET - THE RENDERER SHOULD DO THAT
      g_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      uint64_t read_pointer = sizeof(mvp);

      for (int n = 0; n < draw_data->CmdListsCount; ++n)
      {
        // Render command lists
        int vtx_offset = 0;
        int idx_offset = 0;

        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        size_t vertices_count = cmd_list->VtxBuffer.size();
        size_t indices_count = cmd_list->IdxBuffer.size();
        size_t vertices_size = vertices_count * sizeof(ImDrawVert);
        size_t indices_size = indices_count * sizeof(ImDrawIdx);

        D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;
        vertex_buffer_view.BufferLocation = buffer_address + read_pointer;
        vertex_buffer_view.StrideInBytes = sizeof(ImDrawVert);
        vertex_buffer_view.SizeInBytes = static_cast<UINT>(vertices_size);
        read_pointer += vertices_size;

        D3D12_INDEX_BUFFER_VIEW index_buffer_view;
        index_buffer_view.BufferLocation = buffer_address + read_pointer;
        index_buffer_view.SizeInBytes = static_cast<UINT>(indices_size);
        index_buffer_view.Format = DXGI_FORMAT_R16_UINT;
        read_pointer += indices_size;

        g_command_list->IASetVertexBuffers(0, 1, &vertex_buffer_view);
        g_command_list->IASetIndexBuffer(&index_buffer_view);

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); ++cmd_i)
        {
          const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
          if (pcmd->UserCallback)
          {
            pcmd->UserCallback(cmd_list, pcmd);
          }
          else
          {
            const D3D12_RECT r = {
              static_cast<LONG>(pcmd->ClipRect.x),
              static_cast<LONG>(pcmd->ClipRect.y),
              static_cast<LONG>(pcmd->ClipRect.z),
              static_cast<LONG>(pcmd->ClipRect.w)
            };

            g_command_list->RSSetScissorRects(1, &r);
            g_command_list->DrawIndexedInstanced(pcmd->ElemCount, 1, idx_offset, vtx_offset, 0);
          }
          idx_offset += pcmd->ElemCount;
        }
        vtx_offset += static_cast<int>(vertices_count);
      }

    }
    
    //------------------------------------------------------------------------------------------------------
    bool ImGui_ImplDX12_Init(
      void* hwnd,
      D3D12Device& device,
      ID3D12GraphicsCommandList* command_list)
    {
      g_device = &device;
      g_command_list = command_list;
      g_hwnd = (HWND)hwnd;

      if (!QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&g_ticks_per_second)))
        return false;
      if (!QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&g_time)))
        return false;

      if (!ImGui_Impl_CreateDeviceObjects())
        return false;

      ImGuiIO& io = ImGui::GetIO();
      io.KeyMap[ImGuiKey_Tab] = VK_TAB;   
      io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
      io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
      io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
      io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
      io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
      io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
      io.KeyMap[ImGuiKey_Home] = VK_HOME;
      io.KeyMap[ImGuiKey_End] = VK_END;
      io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
      io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
      io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
      io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
      io.KeyMap[ImGuiKey_A] = 'A';
      io.KeyMap[ImGuiKey_C] = 'C';
      io.KeyMap[ImGuiKey_V] = 'V';
      io.KeyMap[ImGuiKey_X] = 'X';
      io.KeyMap[ImGuiKey_Y] = 'Y';
      io.KeyMap[ImGuiKey_Z] = 'Z';

      io.RenderDrawListsFn = ImGui_ImplDX12_RenderDrawLists;
      io.ImeWindowHandle = hwnd;

      g_initialized = true;
      return true;
    }
    
    //------------------------------------------------------------------------------------------------------
    void ImGui_Impl_Shutdown()
    {
      SafeRelease(g_srv_desc_heap);
      foundation::Memory::Destruct(g_font_texture);
      SafeRelease(g_upload_buffer);
      SafeRelease(g_root_signature);
      SafeRelease(g_pso);
    }

    //------------------------------------------------------------------------------------------------------
    void ImGui_Impl_NewFrame()
    {
      ImGuiIO& io = ImGui::GetIO();
      
      // Setup display size (every frame to accommodate for window resizing)
      RECT rect;
      GetClientRect(g_hwnd, &rect);
      io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
      
      // Setup time step
      INT64 current_time;
      QueryPerformanceCounter((LARGE_INTEGER *)&current_time);
      io.DeltaTime = (float)(current_time - g_time) / g_ticks_per_second;
      g_time = current_time;
      
      // Read keyboard modifiers inputs
      io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
      io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
      io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
      // io.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
      // io.MousePos : filled by WM_MOUSEMOVE events
      // io.MouseDown : filled by WM_*BUTTON* events
      // io.MouseWheel : filled by WM_MOUSEWHEEL events
      
      // Hide OS mouse cursor if ImGui is drawing it
      SetCursor(io.MouseDrawCursor ? NULL : LoadCursor(NULL, IDC_ARROW));
      
      // Start the frame
      ImGui::NewFrame();
    }

    //------------------------------------------------------------------------------------------------------
    bool ImGui_Impl_CreateDeviceObjects()
    {
      static const char* vertex_shader =
        "cbuffer vertexBuffer : register(b0) \
        {\
        float4x4 ProjectionMatrix; \
        };\
        struct VS_INPUT\
        {\
        float2 pos : POSITION;\
        float4 col : COLOR0;\
        float2 uv  : TEXCOORD0;\
        };\
        \
        struct PS_INPUT\
        {\
        float4 pos : SV_POSITION;\
        float4 col : COLOR0;\
        float2 uv  : TEXCOORD0;\
        };\
        \
        PS_INPUT main(VS_INPUT input)\
        {\
        PS_INPUT output;\
        output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));\
        output.col = input.col;\
        output.uv  = input.uv;\
        return output;\
        }";

      static const char* pixel_shader =
        "struct PS_INPUT\
        {\
        float4 pos : SV_POSITION;\
        float4 col : COLOR0;\
        float2 uv  : TEXCOORD0;\
        };\
        SamplerState sampler0 : register(s0);\
        Texture2D texture0 : register(t0);\
        \
        float4 main(PS_INPUT input) : SV_Target\
        {\
        float4 out_col = input.col * texture0.Sample(sampler0, input.uv); \
        return out_col; \
        }";

      ID3DBlob* vs_blob = nullptr;
      ID3DBlob* ps_blob = nullptr;
      ID3DBlob* error_blob = nullptr;

      // D3D12 Imgui Input layout
      D3D12_INPUT_ELEMENT_DESC input_element_descs[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, (size_t)(&((ImDrawVert*)0)->pos), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, (size_t)(&((ImDrawVert*)0)->uv), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (size_t)(&((ImDrawVert*)0)->col), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
      };

      // Compile imgui shaders
      D3DCompile(
        vertex_shader,
        strlen(vertex_shader),
        NULL,
        NULL,
        NULL,
        "main",
        "vs_5_0",
        D3DCOMPILE_DEBUG,
        0,
        &vs_blob,
        &error_blob
      );

      D3DCompile(
        pixel_shader,
        strlen(pixel_shader),
        NULL,
        NULL,
        NULL,
        "main",
        "ps_5_0",
        D3DCOMPILE_DEBUG,
        0,
        &ps_blob,
        &error_blob
      );

      // Create fonts texture

      D3D12_RASTERIZER_DESC rasterizer_desc = {};
      rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID;
      rasterizer_desc.CullMode = D3D12_CULL_MODE_NONE;
      rasterizer_desc.FrontCounterClockwise = true;
      rasterizer_desc.DepthBias = 0;
      rasterizer_desc.DepthBiasClamp = 0.f;
      rasterizer_desc.SlopeScaledDepthBias = 0.f;
      rasterizer_desc.DepthClipEnable = true;
      rasterizer_desc.MultisampleEnable = false;
      rasterizer_desc.AntialiasedLineEnable = true;
      rasterizer_desc.ForcedSampleCount = 1;
      rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

      D3D12_BLEND_DESC blend_desc;
      blend_desc.AlphaToCoverageEnable = false;
      blend_desc.IndependentBlendEnable = false;
      blend_desc.RenderTarget[0].BlendEnable = true;
      blend_desc.RenderTarget[0].LogicOpEnable = false;
      blend_desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
      blend_desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
      blend_desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
      blend_desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
      blend_desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
      blend_desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
      blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

      D3D12_DESCRIPTOR_RANGE1 desc_range[] = {
        //{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
        { D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, 0 },
      };

      // This root signature will have two parameters, one descriptor table for SRVs and one constant buffer descriptor
      CD3DX12_ROOT_PARAMETER1 root_parameter[2];
      root_parameter[0].InitAsDescriptorTable(
        _countof(desc_range),
        desc_range,
        D3D12_SHADER_VISIBILITY_PIXEL
      );
      
      root_parameter[1].InitAsConstantBufferView(
        1,
        1,
        D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
        D3D12_SHADER_VISIBILITY_VERTEX
      );

      root_parameter[1].Descriptor.RegisterSpace = 0;
      root_parameter[1].Descriptor.ShaderRegister = 0;

      // Include a static sampler
      D3D12_STATIC_SAMPLER_DESC sampler_desc = {};
      sampler_desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
      sampler_desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
      sampler_desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
      sampler_desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
      sampler_desc.MipLODBias = 0.f;
      sampler_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
      sampler_desc.MinLOD = 0.f;
      sampler_desc.MaxLOD = 0.f;
      sampler_desc.ShaderRegister = 0;
      sampler_desc.RegisterSpace = 0;
      sampler_desc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

      // Root signature description
      CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc_root_signature = {};
      desc_root_signature.Init_1_1(
        _countof(root_parameter),
        root_parameter,
        1,
        &sampler_desc,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
      );

      if (g_device->CreateVersionedRootSignature(
        g_root_signature,
        desc_root_signature,
        D3D_ROOT_SIGNATURE_VERSION_1_1) == false)
      {
        PS_LOG(Error, "Failed to create d3d12 imgui root signature.\n");
        return false;
      }

      D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
      pso_desc.InputLayout = { input_element_descs, _countof(input_element_descs) };
      pso_desc.pRootSignature = g_root_signature;
      pso_desc.VS = { (uint8_t*)(vs_blob->GetBufferPointer()), vs_blob->GetBufferSize() };
      pso_desc.PS = { (uint8_t*)(ps_blob->GetBufferPointer()), ps_blob->GetBufferSize() };
      pso_desc.RasterizerState = rasterizer_desc;
      pso_desc.BlendState = blend_desc;
      pso_desc.DepthStencilState.DepthEnable = FALSE;
      pso_desc.DepthStencilState.StencilEnable = FALSE;
      pso_desc.SampleMask = UINT_MAX;
      pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
      pso_desc.NumRenderTargets = 1;
      pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
      pso_desc.SampleDesc.Count = 1;

      // Create graphics pipeline state
      if (g_device->CreateGraphicsPipelineState(g_pso, pso_desc) == false)
      {
        PS_LOG(Error, "Failed to create d3d12 imgui pipeline state object.\n");
        return false;
      }

      // Create SRV heap for the fonts texture
      g_device->CreateDescriptorHeap(
        g_srv_desc_heap,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        1,
        D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
      );

      // Upload buffer description
      D3D12_RESOURCE_DESC upload_desc = {};
      upload_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
      upload_desc.Alignment = 0;
      upload_desc.Width = IMGUI_GPU_BUFFER_SIZE * 8;
      upload_desc.Height = 1;
      upload_desc.DepthOrArraySize = 1;
      upload_desc.MipLevels = 1;
      upload_desc.Format = DXGI_FORMAT_UNKNOWN;
      upload_desc.SampleDesc.Count = 1;
      upload_desc.SampleDesc.Quality = 0;
      upload_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
      upload_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

      // Create upload buffer
      if (g_device->CreateCommittedResource(
        g_upload_buffer,
        CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        upload_desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr) == false)
      {
        PS_LOG(Error, "Failed to create upload buffer resource.\n");
        return false;
      }

      // Map upload buffer (will not be read back on the CPU)
      D3D12_RANGE read_range;
      read_range.End = 0;
      read_range.Begin = 1;
      g_upload_buffer->Map(0, &read_range, (void**)&g_mapped_buffer);

      if (g_mapped_buffer == nullptr)
      {
        PS_LOG(Error, "Failed to map the upload buffer (imgui).\n");
        return false;
      }

      // Create fonts texture
      if (ImGui_Impl_CreateFontsTexture() == false)
      {
        return false;
      }

      SafeRelease(error_blob);
      SafeRelease(ps_blob);
      SafeRelease(vs_blob);

      return true;
    }

    //------------------------------------------------------------------------------------------------------
    /**
    * @brief Input hack for imgui. Can replace the current WndProc function. 
    * @param[in] hWnd (void*) A window handle.
    * @param[in] message (UINT) The message to handle.
    * @param[in] wParam (WPARAM) Additional message information. Depends on the message.
    * @param[in] lParam (LPARAM) Additional message information. Depends on the message.
    * @return (LRESULT) A result value.
    */
    IMGUI_API LRESULT 
      ImGui_Impl_WndProcHandler(void* hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
      ImGuiIO& io = ImGui::GetIO();
      switch (message)
      {
      case WM_LBUTTONDOWN:
        io.MouseDown[0] = true;
        return true;
      case WM_LBUTTONUP:
        io.MouseDown[0] = false;
        return true;
      case WM_RBUTTONDOWN:
        io.MouseDown[1] = true;
        return true;
      case WM_RBUTTONUP:
        io.MouseDown[1] = false;
        return true;
      case WM_MBUTTONDOWN:
        io.MouseDown[2] = true;
        return true;
      case WM_MBUTTONUP:
        io.MouseDown[2] = false;
        return true;
      case WM_MOUSEWHEEL:
        io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
        return true;
      case WM_MOUSEMOVE:
        io.MousePos.x = (signed short)(lParam);
        io.MousePos.y = (signed short)(lParam >> 16);
        return true;
      case WM_KEYDOWN:
        if (wParam < 256)
          io.KeysDown[wParam] = 1;
        return true;
      case WM_KEYUP:
        if (wParam < 256)
          io.KeysDown[wParam] = 0;
        return true;
      case WM_CHAR:
        // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
        if (wParam > 0 && wParam < 0x10000)
          io.AddInputCharacter((unsigned short)wParam);
        return true;
      }
      return DefWindowProc((HWND)hWnd, message, wParam, lParam);
    }

    //------------------------------------------------------------------------------------------------------
    bool ImGui_Impl_CreateFontsTexture()
    {
      // Create fonts texture and SRV descriptor for it
      unsigned char* pixels = 0;
      int width, height;

      ImGuiIO& io = ImGui::GetIO();
      io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

      // Create fonts texture
      D3D12_RESOURCE_DESC desc;
      desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
      desc.Alignment = 0;
      desc.Width = width;
      desc.Height = height;
      desc.DepthOrArraySize = 1;
      desc.MipLevels = 1;
      desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
      desc.SampleDesc.Count = 1;
      desc.SampleDesc.Quality = 0;
      desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
      desc.Flags = D3D12_RESOURCE_FLAG_NONE;

      g_font_texture = foundation::Memory::Construct<D3D12Texture2D>();

      if (g_device->CreateTexture2D(
        pixels,
        desc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        nullptr,
        1,
        g_font_texture) == false)
      {
        PS_LOG(Error, "Failed to create fonts texture.\n");
        return false;
      }

      g_device->CreateShaderResourceView(g_font_texture);

      D3D12_CPU_DESCRIPTOR_HANDLE src_cpu_handle;
      g_device->persistent_descriptor_heap().GetCPUHandleForSRVDescriptor(
        g_font_texture->srv_persistent_index_,
        src_cpu_handle
      );

      g_device->CopyDescriptorsSimple(
        1,
        g_srv_desc_heap->GetCPUDescriptorHandleForHeapStart(),
        src_cpu_handle,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
      );

      return true;
    }

  }
}