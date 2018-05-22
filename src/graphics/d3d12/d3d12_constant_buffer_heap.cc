#include "d3d12_constant_buffer_heap.h"

#include "d3d12_device_wrapper.h"
#include "d3dx12.h"

namespace sulphur
{
  namespace graphics
  {
    //------------------------------------------------------------------------------------------------------
    void D3D12ConstantBufferHeap::Create(D3D12Device& device)
    {
      D3D12_HEAP_PROPERTIES properties = {};
      properties.CreationNodeMask = 0;
      properties.VisibleNodeMask = 0;
      properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
      properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
      properties.Type = D3D12_HEAP_TYPE_UPLOAD;

      device.CreateCommittedResource(
        heap_,
        properties,
        D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES,
        CD3DX12_RESOURCE_DESC::Buffer(heap_size_),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr
      );

      CD3DX12_RANGE range(0, 0);
      if (FAILED(heap_->Map(0, &range, reinterpret_cast<void**>(&data_begin_))))
      {
        // log error
      }
    }

    //------------------------------------------------------------------------------------------------------
    D3D12ConstantBufferHeap::~D3D12ConstantBufferHeap()
    {
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12ConstantBufferHeap::OnDestroy()
    {
      if (heap_ != nullptr)
      {
        heap_->Release();
      }

      heap_ = nullptr;
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12ConstantBufferHeap::Write(void* data, size_t size, size_t& out_offset_in_heap)
    {
      //If the data will overflow
      if (ptr_ + size > heap_size_)
      {
        //Return to the beginning of the heap
        ptr_ = 0;
      }

      memcpy(data_begin_ + ptr_, data, size);
      out_offset_in_heap = ptr_;
      ptr_ += size;
    }

  }
}