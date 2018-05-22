#include "d3d12_frame_descriptor_heap.h"

#include "graphics/d3d12/d3d12_device_wrapper.h"

#include <foundation/logging/logger.h>

#include "d3dx12.h"

namespace sulphur
{
  namespace graphics
  {
    //------------------------------------------------------------------------------------------------------
    D3D12FrameDescriptorHeap::D3D12FrameDescriptorHeap(
      D3D12Device& device,
      D3D12PersistentDescriptorHeap& persistent_desc_heap) :
      device_(device),
      persistent_descriptor_heap_(persistent_desc_heap)
    {

    }

    //------------------------------------------------------------------------------------------------------
    D3D12FrameDescriptorHeap::~D3D12FrameDescriptorHeap()
    {
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12FrameDescriptorHeap::OnDestroy()
    {
      // Release srv heaps
      for (uint32_t i = 0; i < srv_uav_heap_.size(); ++i)
      {
        SafeRelease(srv_uav_heap_[i]);
      }

      // Release rtv heaps
      for (uint32_t i = 0; i < rtv_heap_.size(); ++i)
      {
        SafeRelease(rtv_heap_[i]);
      }

      // Release dsv heaps
      for (uint32_t i = 0; i < dsv_heap_.size(); ++i)
      {
        SafeRelease(dsv_heap_[i]);
      }
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12FrameDescriptorHeap::Initialize(
      uint32_t frame_buffer_count,
      size_t srv_heap_size,
      size_t uav_heap_size,
      size_t rtv_heap_size,
      size_t dsv_heap_size)
    {
      srv_heap_size_ = srv_heap_size;
      uav_heap_size_ = uav_heap_size;
      rtv_heap_size_ = rtv_heap_size;
      dsv_heap_size_ = dsv_heap_size;

      srv_uav_descriptor_size_ = 
        device_.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
      rtv_descriptor_size_ = 
        device_.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
      dsv_descriptor_size_ = 
        device_.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

      for (uint32_t i = 0; i < frame_buffer_count; ++i)
      {
        // Create srv/uav heap
        srv_uav_heap_.push_back(nullptr);
        if (!device_.CreateDescriptorHeap(
          srv_uav_heap_[i],
          D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
          static_cast<uint32_t>(srv_heap_size + uav_heap_size),
          D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE))
        {
          PS_LOG(Error, "Failed to create frame SRV/UAV heap.\n");
        }

        // Create rtv heap
        rtv_heap_.push_back(nullptr);
        if (!device_.CreateDescriptorHeap(
          rtv_heap_[i],
          D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
          static_cast<uint32_t>(rtv_heap_size),
          D3D12_DESCRIPTOR_HEAP_FLAG_NONE))
        {
          PS_LOG(Error, "Failed to create frame RTV heap.\n");
        }

        // Create dsv heap
        dsv_heap_.push_back(nullptr);
        if (!device_.CreateDescriptorHeap(
          dsv_heap_[i],
          D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
          static_cast<uint32_t>(dsv_heap_size),
          D3D12_DESCRIPTOR_HEAP_FLAG_NONE))
        {
          PS_LOG(Error, "Failed to create frame DSV heap.\n");
        }
      }
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12FrameDescriptorHeap::StartFrame(uint32_t frame_index)
    {
      current_frame_ = frame_index;
      current_srv_write_index_ = 0;
      current_uav_write_index_ = (uint32_t)srv_heap_size_;
      current_rtv_write_index_ = 0;
      current_dsv_write_index_ = 0;
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12FrameDescriptorHeap::CopySRVDescriptor(
      uint32_t persistent_index,
      D3D12_CPU_DESCRIPTOR_HANDLE& out_cpu_handle,
      D3D12_GPU_DESCRIPTOR_HANDLE& out_gpu_handle)
    {
      D3D12_CPU_DESCRIPTOR_HANDLE persistent_cpu_handle_;
      persistent_descriptor_heap_.GetCPUHandleForSRVDescriptor(
        persistent_index,
        persistent_cpu_handle_
      );

      out_cpu_handle = srv_uav_heap_[current_frame_]->GetCPUDescriptorHandleForHeapStart();
      out_cpu_handle.ptr += srv_uav_descriptor_size_ * current_srv_write_index_;

      device_.CopyDescriptorsSimple(
        1,
        out_cpu_handle,
        persistent_cpu_handle_,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
      );
      out_gpu_handle = srv_uav_heap_[current_frame_]->GetGPUDescriptorHandleForHeapStart();
      out_gpu_handle.ptr += srv_uav_descriptor_size_ * current_srv_write_index_;

      ++current_srv_write_index_;
      if (current_srv_write_index_ == srv_heap_size_)
      {
        PS_LOG(Debug, "Uh-oh! Frame SRV/UAV heap is too small! Resize not implemented yet. :(\n");
        // ResizeSRVHeap(size_t new_size);
      }
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12FrameDescriptorHeap::CopyUAVDescriptor(
      uint32_t persistent_index,
      D3D12_CPU_DESCRIPTOR_HANDLE& out_cpu_handle,
      D3D12_GPU_DESCRIPTOR_HANDLE& out_gpu_handle)
    {
      D3D12_CPU_DESCRIPTOR_HANDLE persistent_cpu_handle_;
      persistent_descriptor_heap_.GetCPUHandleForUAVDescriptor(
        persistent_index,
        persistent_cpu_handle_
      );

      out_cpu_handle = srv_uav_heap_[current_frame_]->GetCPUDescriptorHandleForHeapStart();
      out_cpu_handle.ptr += srv_uav_descriptor_size_ * current_uav_write_index_;

      device_.CopyDescriptorsSimple(
        1,
        out_cpu_handle,
        persistent_cpu_handle_,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
      );

      out_gpu_handle = srv_uav_heap_[current_frame_]->GetGPUDescriptorHandleForHeapStart();
      out_gpu_handle.ptr += srv_uav_descriptor_size_ * current_uav_write_index_;

      ++current_uav_write_index_;
      if (current_uav_write_index_ == srv_heap_size_ + uav_heap_size_)
      {
        PS_LOG(Debug, "Uh-oh! Frame SRV/UAV heap is too small! Resize not implemented yet. :(\n");
        // ResizeSRVUAVHeap(size_t new_size);
      }
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12FrameDescriptorHeap::CopyRTVDescriptor(
      uint32_t persistent_index,
      D3D12_CPU_DESCRIPTOR_HANDLE& out_cpu_handle,
      D3D12_GPU_DESCRIPTOR_HANDLE& out_gpu_handle)
    {
      D3D12_CPU_DESCRIPTOR_HANDLE persistent_cpu_handle_;
      persistent_descriptor_heap_.GetCPUHandleForRTVDescriptor(
        persistent_index,
        persistent_cpu_handle_
      );

      out_cpu_handle = rtv_heap_[current_frame_]->GetCPUDescriptorHandleForHeapStart();
      out_cpu_handle.ptr += rtv_descriptor_size_ * current_rtv_write_index_;

      device_.CopyDescriptorsSimple(
        1, 
        out_cpu_handle, 
        persistent_cpu_handle_, 
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV
      );
      out_gpu_handle = rtv_heap_[current_frame_]->GetGPUDescriptorHandleForHeapStart();
      out_gpu_handle.ptr += rtv_descriptor_size_ * current_rtv_write_index_;

      ++current_rtv_write_index_;
      if (current_rtv_write_index_ == rtv_heap_size_)
      {
        PS_LOG(Debug, "Uh-oh! Frame RTV heap is too small! Resize not implemented yet. :(\n");
        // ResizeSRVUAVHeap(size_t new_size);
      }
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12FrameDescriptorHeap::CopyDSVDescriptor(
      uint32_t persistent_index,
      D3D12_CPU_DESCRIPTOR_HANDLE& out_cpu_handle,
      D3D12_GPU_DESCRIPTOR_HANDLE& out_gpu_handle)
    {
      D3D12_CPU_DESCRIPTOR_HANDLE persistent_cpu_handle_;
      persistent_descriptor_heap_.GetCPUHandleForRTVDescriptor(
        persistent_index,
        persistent_cpu_handle_
      );

      out_cpu_handle = dsv_heap_[current_frame_]->GetCPUDescriptorHandleForHeapStart();
      out_cpu_handle.ptr += dsv_descriptor_size_ * current_dsv_write_index_;

      device_.CopyDescriptorsSimple(
        1,
        out_cpu_handle,
        persistent_cpu_handle_, 
        D3D12_DESCRIPTOR_HEAP_TYPE_DSV
      );
      out_gpu_handle = dsv_heap_[current_frame_]->GetGPUDescriptorHandleForHeapStart();
      out_gpu_handle.ptr += dsv_descriptor_size_ * current_dsv_write_index_;

      ++current_dsv_write_index_;
      if (current_dsv_write_index_ == dsv_heap_size_)
      {
        PS_LOG(Debug, "Uh-oh! Frame DSV heap is too small! Resize not implemented yet. :(\n");
        // ResizeSRVUAVHeap(size_t new_size);
      }
    }
  }
}