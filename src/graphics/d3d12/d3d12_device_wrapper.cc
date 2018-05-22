#include "graphics/d3d12/d3d12_device_wrapper.h"

#include "graphics/d3d12/d3d12_vertex.h"
#include "graphics/d3d12/assets/d3d12_asset.h"
#include "graphics/d3d12/d3dx12.h"

#include <foundation/logging/logger.h>

#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <cstdio>

namespace sulphur
{
  namespace graphics
  {
    //------------------------------------------------------------------------------------------------------
    void D3D12Device::Create()
    {
      // Create DXGI factory
      IDXGIFactory4* factory;

      if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))))
      {
        PS_LOG(Error, "Unable to create DXGI factory");
        //return PUG_RESULT_GRAPHICS_ERROR;
      }

      // Query adapters
      IDXGIAdapter1* adapter = nullptr;
      DXGI_ADAPTER_DESC1 adapter_desc;
      {
        IDXGIAdapter1* current;
        size_t max_VRAM = 0;
        for (uint32_t i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(i, &current); ++i)
        {
          DXGI_ADAPTER_DESC1 current_desc;
          current->GetDesc1(&current_desc);

          if (current_desc.Flags & D3D_DRIVER_TYPE_SOFTWARE)
          {
            continue;
          }

          if (max_VRAM <= current_desc.DedicatedVideoMemory)
          {
            adapter = current;
            max_VRAM = current_desc.DedicatedVideoMemory;
            adapter_desc = current_desc;
          }
        }
      }

      D3D_FEATURE_LEVEL supported_feature_levels[] =
      {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
      };

      // Create D3D12 device

      bool found = false;

      for (uint32_t i = 0; i < _countof(supported_feature_levels) && !found; ++i)
      {
        if (SUCCEEDED(D3D12CreateDevice(
          adapter,
          supported_feature_levels[i],
          __uuidof(ID3D12Device1),
          nullptr)))
        {
          if (SUCCEEDED(D3D12CreateDevice(
            adapter,
            supported_feature_levels[i],
            IID_PPV_ARGS(&device_))))
          {
            PS_LOG(
              Info,
              "Created device at feature level %s",
              GetFeatureLevelString(supported_feature_levels[i])
            );

            PS_LOG(
              Info,
              "Adapter used: %ls",
              adapter_desc.Description
            );

            found = true;
            break;
          }
        }
      }

      if (!device_)
      {
        //log::Error("Failed to create D3D12 device.");
        //return PUG_RESULT_GRAPHICS_ERROR;
      }

      SafeRelease(adapter);
      SafeRelease(factory);

      persistent_descriptor_heap_.Initialize(
        512,
        512,
        512,
        512
      );

      // TODO: Might need to change to direct to transition resource states
      // Create command queue
      CreateCommandQueue(command_queue_, D3D12_COMMAND_LIST_TYPE_DIRECT);

      // Create command allocator
      CreateCommandAllocator(command_allocator_, D3D12_COMMAND_LIST_TYPE_DIRECT);

      // Create command list
      CreateGraphicsCommandList(
        command_list_,
        command_allocator_,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        nullptr
      );
      command_list_->Close();

      CreateFence(fence_, 0);
      fence_value_ = 0;
      fence_event_ = CreateEventA(NULL, FALSE, FALSE, NULL);
    }

    //------------------------------------------------------------------------------------------------------
    D3D12Device::~D3D12Device()
    {
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Device::OnDestroy()
    {
      shader_asset_manager_.OnDestroy();
      texture_asset_manager_.OnDestroy();
      mesh_asset_manager_.OnDestroy();
      persistent_descriptor_heap_.OnDestroy();

      SafeRelease(fence_);
      SafeRelease(command_list_);
      SafeRelease(command_allocator_);
      SafeRelease(command_queue_);
      SafeRelease(device_);
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12Device::CreateCommandQueue(
      ID3D12CommandQueue*& out_command_queue,
      D3D12_COMMAND_LIST_TYPE type,
      D3D12_COMMAND_QUEUE_PRIORITY priority,
      D3D12_COMMAND_QUEUE_FLAGS flags)
    {
      D3D12_COMMAND_QUEUE_DESC command_queue_desc = {};
      command_queue_desc.Flags = flags;
      command_queue_desc.Priority = priority;
      command_queue_desc.Type = type;
      command_queue_desc.NodeMask = 0;
      if (FAILED(
        device_->CreateCommandQueue(&command_queue_desc, IID_PPV_ARGS(&out_command_queue))
      ))
      {
        PS_LOG(Error, "Failed to create command queue");
        return false;
      }

      return true;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12Device::CreateGraphicsCommandList(
      ID3D12GraphicsCommandList*& out_command_list,
      ID3D12CommandAllocator* command_allocator,
      D3D12_COMMAND_LIST_TYPE type,
      ID3D12PipelineState* pso)
    {
      if (FAILED(device_->CreateCommandList(
        0,
        type,
        command_allocator,
        pso,
        IID_PPV_ARGS(&out_command_list))))
      {
        //log::Error("Failed to create graphics command list.");
        return false;
      }

      return true;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12Device::CreateCommandAllocator(
      ID3D12CommandAllocator*& out_command_allocator,
      D3D12_COMMAND_LIST_TYPE type)
    {
      if (FAILED(device_->CreateCommandAllocator(type, IID_PPV_ARGS(&out_command_allocator))))
      {
        PS_LOG(Error, "Error creating command allocators");
        return false;
      }

      return true;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12Device::CreateDescriptorHeap(
      ID3D12DescriptorHeap*& out_descriptor_heap,
      D3D12_DESCRIPTOR_HEAP_TYPE type,
      uint32_t num_descriptors,
      D3D12_DESCRIPTOR_HEAP_FLAGS flags)
    {
      D3D12_DESCRIPTOR_HEAP_DESC desc = {};
      desc.Flags = flags;
      desc.Type = type;
      desc.NodeMask = 0;
      desc.NumDescriptors = num_descriptors;

      if (FAILED(device_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&out_descriptor_heap))))
      {
        PS_LOG(Error, "Error creating descriptor heap");
        return false;
      }

      return true;
    }

    //------------------------------------------------------------------------------------------------------
    uint32_t D3D12Device::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type)
    {
      return device_->GetDescriptorHandleIncrementSize(type);
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12Device::CreateHeap(
      ID3D12Heap*& out_heap,
      size_t heap_size,
      size_t alignment,
      D3D12_HEAP_FLAGS flags,
      D3D12_HEAP_TYPE type,
      D3D12_CPU_PAGE_PROPERTY cpu_page_property,
      D3D12_MEMORY_POOL memory_pool)
    {
      D3D12_HEAP_DESC desc = {};
      desc.Alignment = alignment;
      desc.Flags = flags;
      desc.Properties.CreationNodeMask = 0; // TODO: might be broken? Set to 1.
      desc.Properties.VisibleNodeMask = 0; // TODO: might be broken? Set to 1.
      desc.Properties.CPUPageProperty = cpu_page_property;
      desc.Properties.MemoryPoolPreference = memory_pool;
      desc.Properties.Type = type;
      desc.SizeInBytes = heap_size;

      if (FAILED(device_->CreateHeap(&desc, IID_PPV_ARGS(&out_heap))))
      {
        PS_LOG(Error, "Failed to create heap");
        return false;
      }

      return true;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12Device::CreateCommittedResource(
      ID3D12Resource*& out_resource,
      D3D12_HEAP_PROPERTIES heap_properties,
      D3D12_HEAP_FLAGS flags,
      D3D12_RESOURCE_DESC resource_desc,
      D3D12_RESOURCE_STATES initial_state,
      D3D12_CLEAR_VALUE* clear_value)
    {
      if (FAILED(device_->CreateCommittedResource(
        &heap_properties,
        flags,
        &resource_desc,
        initial_state,
        clear_value,
        IID_PPV_ARGS(&out_resource))))
      {
        PS_LOG(Error, "Error creating committed resource");
        return false;
      }

      return true;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12Device::CreateGraphicsPipelineState(
      ID3D12PipelineState*& out_pso,
      D3D12_GRAPHICS_PIPELINE_STATE_DESC &desc)
    {
      if (FAILED(device_->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&out_pso))))
      {
        PS_LOG(Error, "Failed to create graphics pipeline state object");
        return false;
      }

      return true;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12Device::CreateVersionedRootSignature(
      ID3D12RootSignature*& out_root_signature,
      const D3D12_VERSIONED_ROOT_SIGNATURE_DESC desc,
      D3D_ROOT_SIGNATURE_VERSION max_version)
    {
      // Serialize description
      ID3DBlob* root_description_blob;
      ID3DBlob* error_blob;

      HRESULT hresult = D3DX12SerializeVersionedRootSignature(
        &desc,
        max_version,
        &root_description_blob,
        &error_blob
      );

      if (FAILED(hresult))
      {
        PS_LOG(
          Error,
          "Failed to serialize root signature with error: %s",
          static_cast<char*>(error_blob->GetBufferPointer())
        );

        if (error_blob)
          error_blob->Release();
        if (root_description_blob)
          root_description_blob->Release();
        return false;
      }

      if (FAILED(device_->CreateRootSignature(
        0,
        root_description_blob->GetBufferPointer(),
        root_description_blob->GetBufferSize(),
        IID_PPV_ARGS(&out_root_signature)
      )))
      {
        PS_LOG(Error, "Failed to create root signature");

        if (error_blob)
          error_blob->Release();
        if (root_description_blob)
          root_description_blob->Release();
        return false;
      }

      if (error_blob)
        error_blob->Release();
      if (root_description_blob)
        root_description_blob->Release();

      return true;
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Device::CreateRenderTargetView(D3D12Texture2D* texture)
    {
      if (texture->has_rtv_ == true)
      {
        return;
      }

      persistent_descriptor_heap_.AllocateRTVDescriptor(texture->rtv_persistent_index_);

      D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
      rtv_desc.Format = texture->format_;
      rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
      rtv_desc.Texture2D.MipSlice = 0;

      CreateRenderTargetView(texture->resource_, rtv_desc, texture->rtv_persistent_index_);
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Device::CreateRenderTargetView(
      ID3D12Resource* resource,
      D3D12_RENDER_TARGET_VIEW_DESC desc,
      uint32_t& out_heap_handle)
    {
      persistent_descriptor_heap_.AllocateRTVDescriptor(out_heap_handle);
      D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;
      persistent_descriptor_heap_.GetCPUHandleForRTVDescriptor(out_heap_handle, cpu_handle);
      device_->CreateRenderTargetView(resource, &desc, cpu_handle);
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Device::CreateDepthStencilView(D3D12Texture2D* texture)
    {
      if (texture->has_dsv_ == true)
      {
        return;
      }

      persistent_descriptor_heap_.AllocateDSVDescriptor(texture->dsv_persistent_index_);

      D3D12_CPU_DESCRIPTOR_HANDLE dsv_cpu_handle;
      persistent_descriptor_heap_.GetCPUHandleForDSVDescriptor(
        texture->dsv_persistent_index_,
        dsv_cpu_handle
      );

      DXGI_FORMAT format = texture->format_ ==
        DXGI_FORMAT_R32_TYPELESS ? 
        DXGI_FORMAT_D32_FLOAT :
        DXGI_FORMAT_D24_UNORM_S8_UINT;

      CreateDepthStencilView(
        texture->resource_,
        format,
        D3D12_DSV_DIMENSION_TEXTURE2D,
        D3D12_DSV_FLAG_NONE,
        dsv_cpu_handle
      );

      texture->has_dsv_ = true;
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Device::CreateDepthStencilView(
      ID3D12Resource* depth_stencil_resource,
      DXGI_FORMAT format,
      D3D12_DSV_DIMENSION dimension,
      D3D12_DSV_FLAGS flags,
      D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle)
    {
      D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
      desc.Format = format;
      desc.Flags = flags;
      desc.ViewDimension = dimension;

      device_->CreateDepthStencilView(depth_stencil_resource, &desc, cpu_handle);
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12Device::CreateFence(
      ID3D12Fence*& out_fence,
      uint64_t initial_value,
      D3D12_FENCE_FLAGS flags)
    {
      if (FAILED(device_->CreateFence(initial_value, flags, IID_PPV_ARGS(&out_fence))))
      {
        PS_LOG(Error, "Failed to create fence sync object");
        return false;
      }

      return true;
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Device::CreateShaderResourceView(D3D12Texture2D* texture)
    {
      if (texture->has_srv_ == true)
      {
        return;
      }

      D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
      srv_desc.Format = texture->resource_desc_.Format;
      srv_desc.Texture2D.MipLevels = texture->mip_count_;
      srv_desc.Texture2D.MostDetailedMip = 0;
      srv_desc.Texture2D.ResourceMinLODClamp = 0.0f;
      srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
      srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

      CreateShaderResourceView(texture->resource_, srv_desc, texture->srv_persistent_index_);

      texture->has_srv_ = true;
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Device::CreateShaderResourceView(
      ID3D12Resource* resource,
      D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc,
      uint32_t& out_srv_persistent_index)
    {
      persistent_descriptor_heap_.AllocateSRVDescriptor(out_srv_persistent_index);
      D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;
      persistent_descriptor_heap_.GetCPUHandleForSRVDescriptor(
        out_srv_persistent_index,
        cpu_handle
      );

      device_->CreateShaderResourceView(resource, &srv_desc, cpu_handle);
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Device::CopyDescriptorsSimple(
      uint32_t num_descriptors,
      D3D12_CPU_DESCRIPTOR_HANDLE dest_descriptor_range_start,
      D3D12_CPU_DESCRIPTOR_HANDLE src_descriptor_range_start,
      D3D12_DESCRIPTOR_HEAP_TYPE type)
    {
      device_->CopyDescriptorsSimple(
        num_descriptors,
        dest_descriptor_range_start,
        src_descriptor_range_start,
        type
      );
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12Device::CreateVertexAndIndexBuffer(
      ID3D12Resource*& out_vertex_buffer,
      ID3D12Resource*& out_index_buffer,
      D3D12_VERTEX_BUFFER_VIEW& out_vertex_buffer_view,
      D3D12_INDEX_BUFFER_VIEW& out_index_buffer_view,
      const Vertex* vertex_array,
      size_t vertex_count,
      const uint32_t* index_array,
      size_t index_count)
    {
      // Create vertex buffer
      {
        const size_t vertex_buffer_size = sizeof(Vertex) * vertex_count;

        if (!CreateCommittedResource(
          out_vertex_buffer,
          CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
          D3D12_HEAP_FLAG_NONE,
          CD3DX12_RESOURCE_DESC::Buffer(vertex_buffer_size),
          D3D12_RESOURCE_STATE_GENERIC_READ,
          nullptr
        ))
        {
          return false;
        }

        // Copy the triangle data to the vertex buffer.
        uint8_t* p_vertex_data_begin;
        CD3DX12_RANGE read_range(0, 0);
        if (FAILED(
          out_vertex_buffer->Map(0, &read_range, reinterpret_cast<void**>(&p_vertex_data_begin))
        ))
        {
          PS_LOG(Error, "Unknown error");
          return false;
        }
        memcpy(p_vertex_data_begin, vertex_array, vertex_buffer_size);
        out_vertex_buffer->Unmap(0, nullptr);

        out_vertex_buffer_view.BufferLocation = out_vertex_buffer->GetGPUVirtualAddress();
        out_vertex_buffer_view.StrideInBytes = sizeof(Vertex);
        out_vertex_buffer_view.SizeInBytes = static_cast<UINT>(vertex_buffer_size);

      }

      // Create index buffer
      {
        const size_t index_buffer_size = sizeof(uint32_t) * index_count;

        if (!CreateCommittedResource(
          out_index_buffer,
          CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
          D3D12_HEAP_FLAG_NONE,
          CD3DX12_RESOURCE_DESC::Buffer(index_buffer_size),
          D3D12_RESOURCE_STATE_GENERIC_READ,
          nullptr
        ))
        {
          return false;
        }

        // Copy the triangle data to the vertex buffer.
        uint8_t* p_index_data_begin;
        CD3DX12_RANGE read_range(0, 0);
        if (FAILED(
          out_index_buffer->Map(0, &read_range, reinterpret_cast<void**>(&p_index_data_begin))
        ))
        {
          PS_LOG(Error, "Unknown error");
          return false;
        }
        memcpy(p_index_data_begin, index_array, index_buffer_size);
        out_index_buffer->Unmap(0, nullptr);

        out_index_buffer_view.BufferLocation = out_index_buffer->GetGPUVirtualAddress();
        out_index_buffer_view.Format = DXGI_FORMAT_R32_UINT;
        out_index_buffer_view.SizeInBytes = static_cast<UINT>(index_buffer_size);
      }

      return true;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12Device::CreateTexture2D(
      const uint8_t* bytes,
      D3D12_RESOURCE_DESC desc,
      D3D12_RESOURCE_STATES initial_state,
      D3D12_SUBRESOURCE_DATA* sub_res_data,
      uint32_t mip_count,
      D3D12Texture2D* out_texture,
      D3D12TextureType texture_type,
      foundation::Color clear_color)
    {
      D3D12_CLEAR_VALUE clear_value;
      bool use_clear_value = false;

      switch (texture_type)
      {
      case D3D12TextureType::kTexture:
        desc.Flags = D3D12_RESOURCE_FLAG_NONE;
        break;
      case D3D12TextureType::kDepthStencil:
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        use_clear_value = true;
        clear_value = {
          desc.Format == 
          DXGI_FORMAT_R32_TYPELESS ?
          DXGI_FORMAT_D32_FLOAT :
          DXGI_FORMAT_D24_UNORM_S8_UINT,
          {1.0f, 0} 
        };
        break;
      case D3D12TextureType::kRenderTarget:
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        use_clear_value = true;
        clear_value = {desc.Format, {clear_color.r, clear_color.g, clear_color.b, clear_color.a}};
        break;
      default:
        desc.Flags = D3D12_RESOURCE_FLAG_NONE;
        break;
      }

      if (!CreateCommittedResource(
        out_texture->resource_,
        CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        desc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        use_clear_value ? &clear_value : nullptr
      ))
      {
        return false;
      }

      out_texture->current_state_ = D3D12_RESOURCE_STATE_COPY_DEST;
      out_texture->resource_desc_ = desc;
      out_texture->format_ = desc.Format;
      out_texture->mip_count_ = mip_count;

      // If there is no data, exit early
      if (bytes == nullptr)
      {
        return true;
      }

      const uint64_t uploadHeapSize = GetRequiredIntermediateSize(
        out_texture->resource_,
        0,
        mip_count
      );

      ID3D12Resource* tex_upload_heap;

      CreateCommittedResource(
        tex_upload_heap,
        CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        CD3DX12_RESOURCE_DESC::Buffer(uploadHeapSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr
      );

      command_allocator_->Reset();
      command_list_->Reset(command_allocator_, nullptr);

      if (sub_res_data == nullptr)
      {
        D3D12_SUBRESOURCE_DATA subres_data = {};
        subres_data.pData = bytes;
        subres_data.RowPitch = desc.Width * GetSizeFromFormat(desc.Format);
        subres_data.SlicePitch = subres_data.RowPitch * desc.Height;

        UpdateSubresources(
          command_list_,
          out_texture->resource_,
          tex_upload_heap,
          0,
          0,
          1,
          &subres_data
        );
      }
      else
      {
        UpdateSubresources(
          command_list_,
          out_texture->resource_,
          tex_upload_heap,
          0,
          0,
          mip_count,
          sub_res_data
        );
      }

      CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        out_texture->resource_,
        D3D12_RESOURCE_STATE_COPY_DEST,
        initial_state
      );

      command_list_->ResourceBarrier(
        1,
        &barrier
      );
      command_list_->Close();

      // Execute the command list.
      ID3D12CommandList* pp_command_lists[] = { command_list_ };
      command_queue_->ExecuteCommandLists(_countof(pp_command_lists), pp_command_lists);

      fence_value_++;
      command_queue_->Signal(fence_, fence_value_);

      if (fence_->GetCompletedValue() < fence_value_)
      {
        fence_->SetEventOnCompletion(fence_value_, fence_event_);
        WaitForSingleObjectEx(fence_event_, INFINITE, FALSE);
      }

      tex_upload_heap->Release();

      out_texture->current_state_ = initial_state;

      return true;
    }

    //------------------------------------------------------------------------------------------------------
    const char* D3D12Device::GetFeatureLevelString(const D3D_FEATURE_LEVEL &feature_level) const
    {
      switch (feature_level)
      {
      case D3D_FEATURE_LEVEL_11_0:
        return "D3D_FEATURE_LEVEL_11_0";

      case D3D_FEATURE_LEVEL_11_1:
        return "D3D_FEATURE_LEVEL_11_1";

      case D3D_FEATURE_LEVEL_12_0:
        return "D3D_FEATURE_LEVEL_12_0";

      case D3D_FEATURE_LEVEL_12_1:
        return "D3D_FEATURE_LEVEL_12_1";

      default:
        return "INVALID";
      }
    }

    //------------------------------------------------------------------------------------------------------
    size_t D3D12Device::GetSizeFromFormat(DXGI_FORMAT format) const
    {
      switch (format)
      {
      case DXGI_FORMAT_R8G8B8A8_UINT: return 4;
      case DXGI_FORMAT_R8G8B8A8_UNORM: return 4;
      case DXGI_FORMAT_R10G10B10A2_TYPELESS: return 4;
      case DXGI_FORMAT_R11G11B10_FLOAT: return 4;

      case DXGI_FORMAT_R16_FLOAT: return 2;
      case DXGI_FORMAT_R16G16B16A16_FLOAT: return 8;

      case DXGI_FORMAT_D24_UNORM_S8_UINT: return 4;
      case DXGI_FORMAT_R32_FLOAT: return 4;
      case DXGI_FORMAT_R32G32B32A32_FLOAT: return 16;
      default: return 0;
      }
    }

    //------------------------------------------------------------------------------------------------------
    D3D12Device::D3D12Device() :
      persistent_descriptor_heap_(*this),
      mesh_asset_manager_(*this),
      texture_asset_manager_(*this),
      shader_asset_manager_(*this)
    {}

  }
}