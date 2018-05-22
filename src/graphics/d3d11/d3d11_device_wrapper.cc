#include "d3d11_device_wrapper.h"
#include "d3d11_utils.h"

#include <engine/graphics/irenderer.h>
#include <engine/assets/mesh.h>
#include <engine/assets/texture.h>
#include <engine/assets/shader.h>

#include <foundation/utils/color.h>

#include <glm/glm.hpp>

#include <d3d11.h>
#include <d3dcompiler.h>

namespace sulphur
{
  namespace graphics
  {
    //--------------------------------------------------------------------------------
    void D3D11Device::Create()
    {
    #ifdef _DEBUG
      HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION, &device_, nullptr, &device_context_);
    #else
      HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &device_, nullptr, &device_context_);
    #endif // RENDERING_DEBUGGING

      if (FAILED(hr))
      {
        GenerateErrorMessage(hr);
        abort();
      }
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::CreateSwapChain(void* hWnd, const glm::ivec2& screen_size, bool vsync, uint refresh_rate)
    {
      DXGI_MODE_DESC bufferDesc;
      ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

      bufferDesc.Width = screen_size.x;
      bufferDesc.Height = screen_size.y;
      if (vsync)
      {
        bufferDesc.RefreshRate.Numerator = refresh_rate;
        bufferDesc.RefreshRate.Denominator = 1;
      }
      else
      {
        bufferDesc.RefreshRate.Numerator = 0;
        bufferDesc.RefreshRate.Denominator = 1;
      }
      bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
      bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
      bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

      DXGI_SWAP_CHAIN_DESC scd;
      ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

      scd.BufferDesc = bufferDesc;
      scd.SampleDesc.Count = 1;// MathUtils::clamp(antiAlias, 1u, 8u);
      scd.SampleDesc.Quality = 0;
      scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
      scd.BufferCount = 1;
      scd.OutputWindow = (HWND)hWnd;
      scd.Windowed = TRUE;
      scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;// DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
      scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

      // Create a DXGI factory
      IDXGIFactory* pFactory;
      CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)&pFactory);

      // Create swapchain
      HRESULT hr = pFactory->CreateSwapChain(device_.Get(), &scd, &swap_chain_);
      if (FAILED(hr))
      {
        GenerateErrorMessage(hr);
        abort();
      }

      // Store the used adapter for grabbing information
      pFactory->EnumAdapters(0, &adapter_);
      pFactory->Release();

      // Get GPU device data
      DXGI_ADAPTER_DESC adapterDescription;
      adapter_->GetDesc(&adapterDescription);
      device_info_.dedicatedVRAM = adapterDescription.DedicatedVideoMemory;
      
      device_info_.gpuDescription = std::wstring(adapterDescription.Description);

      // Get available display modes
      IDXGIOutput* adapterOutput;
      adapter_->EnumOutputs(0, &adapterOutput);

      uint numModes;
      adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
      displayModes.resize(numModes);
      adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, (DXGI_MODE_DESC*)&displayModes[0]);
      adapterOutput->Release();
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::CreateBackBuffer()
    {
      ID3D11Texture2D* back_buffer_texture;
      swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer_texture);

      device_->CreateRenderTargetView(back_buffer_texture, nullptr, back_buffer_.GetAddressOf());

      back_buffer_texture->Release();
    }

    //--------------------------------------------------------------------------------
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> D3D11Device::GetBackBuffer()
    {
      return back_buffer_;
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::ClearDepthBuffer(const Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& dsv)
    {
      device_context_->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::ClearBackBuffer(const foundation::Color& clear_color)
    {
      device_context_->ClearRenderTargetView(back_buffer_.Get(), clear_color.rgba);
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::ClearRenderTarget(const Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv, 
      const foundation::Color& clear_color)
    {
      device_context_->ClearRenderTargetView(rtv.Get(), clear_color.rgba);
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::CreateTexture(uint width, uint height, engine::TextureFormat format, const byte* data,
      Microsoft::WRL::ComPtr<ID3D11Texture2D>& out_tex,
      Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& out_view,
      bool allow_dsv,
      bool allow_rtv)
    {
      HRESULT hr;
      DXGI_FORMAT dxgi_format = DXGI_FORMAT_UNKNOWN;

      switch (format)
      {
      case engine::TextureFormat::kR8G8B8A8_UNORM:
        dxgi_format = DXGI_FORMAT_R8G8B8A8_UNORM;
        break;
      case engine::TextureFormat::kR16G16B16A16_FLOAT:
        dxgi_format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        break;
      case engine::TextureFormat::kR32G32B32A32_FLOAT:
        dxgi_format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        break;
      case engine::TextureFormat::kR32_TYPELESS:
        dxgi_format = DXGI_FORMAT_R32_TYPELESS;
        break;
      case engine::TextureFormat::kR24G8_TYPELESS:
        dxgi_format = DXGI_FORMAT_R24G8_TYPELESS;
        break;
      }

      // See if format is supported for auto-gen mipmaps (varies by feature level)
      bool autogen = false;

      uint fmtSupport = 0;
      hr = device_->CheckFormatSupport(dxgi_format, &fmtSupport);
      //if (SUCCEEDED(hr) && (fmtSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN))
      //	autogen = true;

      // Create GPU texture
      D3D11_TEXTURE2D_DESC desc;
      desc.Width = width;
      desc.Height = height;
      desc.MipLevels = (autogen) ? 0 : 1;
      desc.ArraySize = 1;
      desc.Format = dxgi_format;
      desc.SampleDesc.Count = 1;
      desc.SampleDesc.Quality = 0;
      desc.Usage = D3D11_USAGE_DEFAULT;
      desc.CPUAccessFlags = 0;

      if (autogen)
      {
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        desc.MiscFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_RESOURCE_MISC_GENERATE_MIPS;
      }
      else
      {
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.MiscFlags = 0;
      }

      if (allow_dsv)
        desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;

      if (allow_rtv)
        desc.BindFlags |= D3D11_BIND_RENDER_TARGET;

      D3D11_SUBRESOURCE_DATA initData;
      initData.pSysMem = data;
      initData.SysMemPitch = desc.Width * GetSizeFromFormat(desc.Format);
      initData.SysMemSlicePitch = initData.SysMemPitch * desc.Height;

      hr = device_->CreateTexture2D(&desc, (autogen) ? nullptr : &initData, out_tex.GetAddressOf());
      if (SUCCEEDED(hr) && out_tex != 0)
      {
        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
        SRVDesc.Format = desc.Format;

        switch (format)
        {
        case engine::TextureFormat::kR8G8B8A8_UNORM:
          SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
          break;
        case engine::TextureFormat::kR16G16B16A16_FLOAT:
          SRVDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
          break;
        case engine::TextureFormat::kR32G32B32A32_FLOAT:
          SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
          break;
        case engine::TextureFormat::kR32_TYPELESS:
          SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
          break;
        case engine::TextureFormat::kR24G8_TYPELESS:
          SRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
          break;
        }

        SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        SRVDesc.Texture2D.MipLevels = (autogen) ? -1 : 1;

        hr = device_->CreateShaderResourceView(out_tex.Get(), &SRVDesc, out_view.GetAddressOf());
        if (FAILED(hr))
        {
          out_tex = nullptr;
          PS_LOG(Error, "[RenderTexture] Unable to create a shader resource view");
        }

        /*if (autogen)
        {
        device_context_->UpdateSubresource(out_tex.Get(), 0, nullptr, data, static_cast<uint>(rowPitch), static_cast<uint>(imageSize));
        device_context_->GenerateMips(out_view.Get());
        }*/
      }
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::CreateDepthBufferFromTexture(const Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture, Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& out_dsv)
    {
      D3D11_TEXTURE2D_DESC texture_desc;
      texture->GetDesc(&texture_desc);

      D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
      ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

      switch (texture_desc.Format)
      {
      case DXGI_FORMAT_R32_TYPELESS:
        descDSV.Format = DXGI_FORMAT_D32_FLOAT;
        break;
      case DXGI_FORMAT_R24G8_TYPELESS:
      default:
        descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        break;
      }

      descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
      descDSV.Texture2D.MipSlice = 0;
      device_->CreateDepthStencilView(texture.Get(), &descDSV, out_dsv.GetAddressOf());
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::CreateRenderTargetFromTexture(const Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture, Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& out_rtv)
    {
      device_->CreateRenderTargetView(texture.Get(), nullptr, out_rtv.GetAddressOf());
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::CreateObjectBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer, size_t source_size)
    {
      D3D11_BUFFER_DESC cbbd;
      ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

      cbbd.Usage = D3D11_USAGE_DEFAULT;
      cbbd.ByteWidth = static_cast<UINT>(source_size);
      cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
      cbbd.CPUAccessFlags = 0;
      cbbd.MiscFlags = 0;

      device_->CreateBuffer(&cbbd, nullptr, buffer.GetAddressOf());
      
      /*cbbd.ByteWidth = sizeof(cbPerWindow);
      dev->CreateBuffer(&cbbd, nullptr, &cbPerWindowBuffer);

      // Update constant buffer
      cbPerWindow.windowSize = vec4f((vec2f)screenSize, 1.0f / (vec2f)screenSize);
      devcon->UpdateSubresource(cbPerWindowBuffer, 0, nullptr, &cbPerWindow, 0, 0);
      devcon->VSSetConstantBuffers(1, 1, &cbPerWindowBuffer);*/
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::UpdateObjectBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer, const void* source)
    {
      device_context_->UpdateSubresource(buffer.Get(), 0, nullptr, source, 0, 0);
      device_context_->VSSetConstantBuffers(0, 1, buffer.GetAddressOf());
      device_context_->GSSetConstantBuffers(0, 1, buffer.GetAddressOf());
      device_context_->PSSetConstantBuffers(0, 1, buffer.GetAddressOf());
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::SetViewport(float x, float y, float width, float height)
    {
      D3D11_VIEWPORT viewport;
      ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

      viewport.TopLeftX = x;
      viewport.TopLeftY = y;
      viewport.Width = width;
      viewport.Height = height;
      viewport.MinDepth = 0.0f;
      viewport.MaxDepth = 1.0f;

      device_context_->RSSetViewports(1, &viewport);
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::SetPrimitiveTopology(TopologyType topology_type)
    {
      switch (topology_type)
      {
      case TopologyType::kPoint:
        device_context_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
        break;
      case TopologyType::kLine:
        device_context_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
        break;
      case TopologyType::kTriangle:
      default:
        device_context_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        break;
      }
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::CreateDepthStencilState()
    {
      D3D11_DEPTH_STENCIL_DESC dsDesc;
      dsDesc.DepthEnable = true;
      dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
      dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
      dsDesc.StencilEnable = true;
      dsDesc.StencilReadMask = 0xFF;
      dsDesc.StencilWriteMask = 0xFF;

      // Stencil operations if pixel is front-facing.
      dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
      dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
      dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE; // Replace with '1'
      dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL; // Not == 1

      // Stencil operations if pixel is back-facing.
      dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
      dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
      dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
      dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;

      device_->CreateDepthStencilState(&dsDesc, &default_depth_stencil_state_);

      device_context_->OMSetDepthStencilState(default_depth_stencil_state_.Get(), 0);
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::CreateRasterizerState()
    {
      D3D11_RASTERIZER_DESC rasterizerState;
      ZeroMemory(&rasterizerState, sizeof(D3D11_RASTERIZER_DESC));
      rasterizerState.FillMode = D3D11_FILL_SOLID;
      rasterizerState.CullMode = D3D11_CULL_BACK;
      rasterizerState.FrontCounterClockwise = false;
      rasterizerState.DepthBias = 10000;
      rasterizerState.DepthBiasClamp = 0.0001f;
      rasterizerState.SlopeScaledDepthBias = 1.0f;
      rasterizerState.DepthClipEnable = true;
      rasterizerState.ScissorEnable = false;
      rasterizerState.MultisampleEnable = false;
      rasterizerState.AntialiasedLineEnable = false;

      device_->CreateRasterizerState(&rasterizerState, &default_rasterizer_state_);

      device_context_->RSSetState(default_rasterizer_state_.Get());
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::CreateBlendState()
    {
      D3D11_BLEND_DESC blendDesc;
      D3D11_RENDER_TARGET_BLEND_DESC rtbd;

      //No Blend
      ZeroMemory(&blendDesc, sizeof(blendDesc));
      ZeroMemory(&rtbd, sizeof(rtbd));

      rtbd.BlendEnable = false;
      rtbd.SrcBlend = D3D11_BLEND_ONE;
      rtbd.DestBlend = D3D11_BLEND_ZERO;
      rtbd.BlendOp = D3D11_BLEND_OP_ADD;
      rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
      rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
      rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
      rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

      blendDesc.AlphaToCoverageEnable = false;
      blendDesc.IndependentBlendEnable = true;
      for (int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
      {
        blendDesc.RenderTarget[i] = rtbd;
      }

      device_->CreateBlendState(&blendDesc, &default_blend_state_);

      float blendFactor[] = { 0.75f, 0.75f, 0.75f, 1.0f };
      //device_context_->OMSetBlendState(default_blend_state_.Get(), blendFactor, 0xFFFFFFFF);
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::CreateSamplers()
    {
      D3D11_SAMPLER_DESC sampDesc;
      ZeroMemory(&sampDesc, sizeof(sampDesc));
      sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

      sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
      sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
      sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
      sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
      sampDesc.MaxAnisotropy = 0;
      sampDesc.MinLOD = 0;
      sampDesc.MaxLOD = 0;

      device_->CreateSamplerState(&sampDesc, point_sampler_.GetAddressOf());
      
      sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

      sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
      sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
      sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

      device_->CreateSamplerState(&sampDesc, linear_sampler_.GetAddressOf());

      device_context_->PSSetSamplers(0, 1, linear_sampler_.GetAddressOf());
      device_context_->PSSetSamplers(1, 1, point_sampler_.GetAddressOf());
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::CreateDefaultShaders()
    {
      // Create the vertex shader
      {
        static const char* vertexShader =
          "cbuffer cbPerObject : register(b0) \
            {\
              row_major float4x4 model; \
              row_major float4x4 view; \
              row_major float4x4 projection; \
            };\
            struct VS_INPUT\
            {\
            float4 pos : POSITION;\
            float3 normal : NORMAL;\
            float2 uv  : TEXCOORD;\
            float4 col : COLOR;\
            };\
            \
            struct PS_INPUT\
            {\
            float4 pos : SV_POSITION;\
            float4 col : COLOR;\
            float2 uv  : TEXCOORD;\
            };\
            \
            PS_INPUT main(VS_INPUT input)\
            {\
            PS_INPUT output;\
            output.pos = mul(input.pos, mul(mul(model, view), projection));\
            output.col = input.col;\
            output.uv  = input.uv;\
            return output;\
            }";

        D3DCompile(vertexShader, strlen(vertexShader), NULL, NULL, NULL, "main", "vs_4_0", 0, 0, &default_vertex_shader_blob_, NULL);
        if (default_vertex_shader_blob_ == NULL)
        { // NB: Pass ID3D10Blob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
          return;
        }
        if (device_->CreateVertexShader((DWORD*)default_vertex_shader_blob_->GetBufferPointer(), 
          default_vertex_shader_blob_->GetBufferSize(), NULL, &default_vertex_shader_) != S_OK)
        {
          return;
        }
        
        // Create the input layout
        D3D11_INPUT_ELEMENT_DESC local_layout[] = {
          { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
          { "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
          { "TANGENT" , 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
          { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT      , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
          { "COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        if (device_->CreateInputLayout(local_layout, 5, default_vertex_shader_blob_->GetBufferPointer(),
          default_vertex_shader_blob_->GetBufferSize(), &default_input_layout_) != S_OK)
        {
          return;
        }
      }

      // Create the pixel shader
      {
        static const char* pixelShader =
          "struct PS_INPUT\
            {\
            float4 pos : SV_POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            \
            Texture2D albedoMap;\
            \
            SamplerState samplerState;\
            \
            float4 main(PS_INPUT input) : SV_Target\
            {\
            return input.col * albedoMap.Sample(samplerState, input.uv.xy); \
            }";

        D3DCompile(pixelShader, strlen(pixelShader), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, &default_pixel_shader_blob_, NULL);
        if (default_pixel_shader_blob_ == NULL)
        {  // NB: Pass ID3D10Blob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
          return;
        }
        if (device_->CreatePixelShader((DWORD*)default_pixel_shader_blob_->GetBufferPointer(), 
          default_pixel_shader_blob_->GetBufferSize(), NULL, &default_pixel_shader_) != S_OK)
        {
          return;
        }
      }

      device_context_->IASetInputLayout(default_input_layout_.Get());
      device_context_->VSSetShader(default_vertex_shader_.Get(), 0, 0);
      device_context_->PSSetShader(default_pixel_shader_.Get(), 0, 0);
    }

    //--------------------------------------------------------------------------------
    bool D3D11Device::CreateBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer, const void* data, size_t size,
      D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage, D3D11_CPU_ACCESS_FLAG cpuFlags, uint miscFlags)
    {
      if (!data || !device_context_ || size <= 0)
      {
        return false;
      }

      D3D11_BUFFER_DESC bufferDesc;
      ZeroMemory(&bufferDesc, sizeof(bufferDesc));

      bufferDesc.Usage = usage;
      bufferDesc.ByteWidth = static_cast<UINT>(size);
      bufferDesc.BindFlags = bindFlags;
      bufferDesc.CPUAccessFlags = cpuFlags;
      bufferDesc.MiscFlags = miscFlags;

      D3D11_SUBRESOURCE_DATA iinitData;
      iinitData.pSysMem = data;

      HRESULT hr = device_->CreateBuffer(&bufferDesc, &iinitData, buffer.GetAddressOf());
      if (FAILED(hr))
      {
        PS_LOG(Warning, "[Renderer]: Could not create buffer!");
        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::CreateVertexShader(const void* srcData, size_t srcSize, Microsoft::WRL::ComPtr<ID3D11VertexShader>& out_shader)
    {
      if (FAILED(device_->CreateVertexShader(srcData, srcSize, nullptr, out_shader.GetAddressOf())))
		  PS_LOG(Warning, "[Renderer]: Could not vertex shader!");
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::CreateHullShader(const void* srcData, size_t srcSize, Microsoft::WRL::ComPtr<ID3D11HullShader>& out_shader)
    {
      if (FAILED(device_->CreateHullShader(srcData, srcSize, nullptr, out_shader.GetAddressOf())))
		  PS_LOG(Warning, "[Renderer]: Could not hull shader!");
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::CreateDomainShader(const void* srcData, size_t srcSize, Microsoft::WRL::ComPtr<ID3D11DomainShader>& out_shader)
    {
      if (FAILED(device_->CreateDomainShader(srcData, srcSize, nullptr, out_shader.GetAddressOf())))
		  PS_LOG(Warning, "[Renderer]: Could not domain shader!");
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::CreateGeometryShader(const void* srcData, size_t srcSize, Microsoft::WRL::ComPtr<ID3D11GeometryShader>& out_shader)
    {
      if (FAILED(device_->CreateGeometryShader(srcData, srcSize, nullptr, out_shader.GetAddressOf())))
		  PS_LOG(Warning, "[Renderer]: Could not geometry shader!");
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::CreatePixelShader(const void* srcData, size_t srcSize, Microsoft::WRL::ComPtr<ID3D11PixelShader>& out_shader)
    {
      if (FAILED(device_->CreatePixelShader(srcData, srcSize, nullptr, out_shader.GetAddressOf())))
		  PS_LOG(Warning, "[Renderer]: Could not pixel shader!");
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::SetRenderTargets(uint num_render_targets, ID3D11RenderTargetView* const* render_targets, 
      const Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& depth_stencil_view)
    {
      device_context_->OMSetRenderTargets(num_render_targets, render_targets, depth_stencil_view.Get());
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::SetTexture(uint register_id, const D3D11Texture* texture)
    {
      if (texture)
      {
        device_context_->PSSetShaderResources(register_id, 1, texture->resource_view.GetAddressOf());
      }
      else
      {
		  PS_LOG(Warning, "Invalid texture id, setting the register to nullptr!");
        ID3D11ShaderResourceView* view = nullptr;
        device_context_->PSSetShaderResources(register_id, 1, &view);
      }
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::SetMesh(const D3D11Mesh* mesh)
    {
      if (!mesh)
      {
        render_size_ = 0;
        return;
      }

      render_size_ = mesh->index_count;
      device_context_->IASetIndexBuffer(mesh->index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

      static uint stride[1] = { sizeof(D3D11Vertex) };
      static uint offset[1] = { 0 };
      device_context_->IASetVertexBuffers(0, 1, mesh->vertex_buffer.GetAddressOf(), stride, offset);

      device_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::SetShader(const D3D11Shader* shader)
    {
      // TODO: Could optimize by preventing of setting the same shader program twice
      if(shader->vertex_shader)
        device_context_->VSSetShader(shader->vertex_shader->shader_program.Get(), 0, 0);

      if(shader->hull_shader)
        device_context_->HSSetShader(shader->hull_shader->shader_program.Get(), 0, 0);

      if (shader->domain_shader)
        device_context_->DSSetShader(shader->domain_shader->shader_program.Get(), 0, 0);

      if (shader->geometry_shader)
        device_context_->GSSetShader(shader->geometry_shader->shader_program.Get(), 0, 0);

      if (shader->pixel_shader)
        device_context_->PSSetShader(shader->pixel_shader->shader_program.Get(), 0, 0);
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::Draw()
    {
      device_context_->DrawIndexed(render_size_, 0, 0);
    }

    //--------------------------------------------------------------------------------
    void D3D11Device::Present(bool vsync)
    {
      swap_chain_->Present(vsync ? 1 : 0, 0);
    }

    //--------------------------------------------------------------------------------
    ID3D11Device* D3D11Device::GetDevice()
    {
      return device_.Get();
    }

    //--------------------------------------------------------------------------------
    ID3D11DeviceContext* D3D11Device::GetDeviceContext()
    {
      return device_context_.Get();
    }

    //--------------------------------------------------------------------------------
    uint D3D11Device::GetSizeFromFormat(DXGI_FORMAT format) const
    {
      switch (format)
      {
      case DXGI_FORMAT_R16_FLOAT:
        return 2u;
      case DXGI_FORMAT_R8G8B8A8_UINT:
      case DXGI_FORMAT_R8G8B8A8_UNORM:
      case DXGI_FORMAT_R10G10B10A2_TYPELESS:
      case DXGI_FORMAT_R11G11B10_FLOAT:
      case DXGI_FORMAT_D24_UNORM_S8_UINT:
      case DXGI_FORMAT_R32_FLOAT:
      case DXGI_FORMAT_R32_TYPELESS:
      case DXGI_FORMAT_R24G8_TYPELESS:
        return 4u;
      case DXGI_FORMAT_R16G16B16A16_FLOAT:
      case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        return 8u;
      case DXGI_FORMAT_R32G32B32A32_FLOAT:
      case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        return 16u;
      default:
        assert(false && "Invalid format!");
        return 0u;
      }
    }
  }
}