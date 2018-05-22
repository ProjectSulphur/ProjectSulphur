#include "d3d12_persistent_descriptor_heap.h"

#include "d3d12_device_wrapper.h"

#include <foundation/logging/logger.h>
#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace graphics
  {

    //------------------------------------------------------------------------------------------------------
    D3D12PersistentDescriptorHeap::D3D12DescriptorPage::D3D12DescriptorPage(
      D3D12Device& device,
      D3D12_DESCRIPTOR_HEAP_TYPE type,
      uint32_t heap_size) :
      descriptor_heap_size_(heap_size),
      free_descriptor_count_(heap_size)
    {
      // TODO: Add possibility for "shader visible" flag
      if (!device.CreateDescriptorHeap(
        descriptor_heap_,
        type,
        heap_size,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE))
      {
        // log error - failed to create a descriptor heap page
      }

      flag_array_.resize(heap_size);
      memset(flag_array_.data(), 0, sizeof(bool) * heap_size);

      descriptor_increment_size_ = device.GetDescriptorHandleIncrementSize(type);
    }

    //------------------------------------------------------------------------------------------------------
    D3D12PersistentDescriptorHeap::D3D12DescriptorPage::~D3D12DescriptorPage()
    {
      descriptor_heap_->Release();
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::D3D12DescriptorPage::AllocateDescriptor(
      uint32_t& out_descriptor_index)
    {
      for (uint32_t i = 0; i < descriptor_heap_size_; ++i)
      {
        if (flag_array_[i] == false) // not taken
        {
          flag_array_[i] = true;
          free_descriptor_count_--;
          out_descriptor_index = i;

          return true;
        }
      }

      return false;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::D3D12DescriptorPage::FreeDescriptor(
      uint32_t descriptor_index)
    {
      if (flag_array_[descriptor_index] == false)
      {
        return false; // Already free
      }

      flag_array_[descriptor_index] = false;

      return true;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::D3D12DescriptorPage::IsEmpty()
    {
      return free_descriptor_count_ == descriptor_heap_size_;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::D3D12DescriptorPage::IsFull()
    {
      return free_descriptor_count_ == 0;
    }

    //------------------------------------------------------------------------------------------------------
    D3D12_CPU_DESCRIPTOR_HANDLE
      D3D12PersistentDescriptorHeap::D3D12DescriptorPage::GetCPUDescriptorHandle(
      uint32_t descriptor_index)
    {
      if (!flag_array_[descriptor_index])
      {
        PS_LOG(Warning, "Attempted to get a GPU descriptor handle for a non-existing resource.");
      }

      D3D12_CPU_DESCRIPTOR_HANDLE out_handle = 
        descriptor_heap_->GetCPUDescriptorHandleForHeapStart();

      out_handle.ptr += descriptor_index * descriptor_increment_size_;

      return out_handle;
    }

    //------------------------------------------------------------------------------------------------------
    D3D12_GPU_DESCRIPTOR_HANDLE
      D3D12PersistentDescriptorHeap::D3D12DescriptorPage::GetGPUDescriptorHandle(
      uint32_t descriptor_index)
    {
      if (!flag_array_[descriptor_index])
      {
        PS_LOG(Warning, "Attempted to get a GPU descriptor handle for a non-existing resource.");
      }

      D3D12_GPU_DESCRIPTOR_HANDLE out_handle =
        descriptor_heap_->GetGPUDescriptorHandleForHeapStart();

      out_handle.ptr += descriptor_index * descriptor_increment_size_;

      return out_handle;
    }

    //------------------------------------------------------------------------------------------------------
    D3D12PersistentDescriptorHeap::D3D12PersistentDescriptorHeap(D3D12Device& device) :
      device_(device),
      srv_heap_size_(0),
      dsv_heap_size_(0),
      rtv_heap_size_(0),
      uav_heap_size_(0)
    {
      
    }

    //------------------------------------------------------------------------------------------------------
    D3D12PersistentDescriptorHeap::~D3D12PersistentDescriptorHeap()
    {
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12PersistentDescriptorHeap::OnDestroy()
    {
      // delete descriptor pages
      for (uint32_t i = 0; i < srv_descriptor_pages_.size(); ++i)
      {
        foundation::Memory::Destruct(srv_descriptor_pages_[i]);
      }

      for (uint32_t i = 0; i < dsv_descriptor_pages_.size(); ++i)
      {
        foundation::Memory::Destruct(dsv_descriptor_pages_[i]);
      }

      for (uint32_t i = 0; i < rtv_descriptor_pages_.size(); ++i)
      {
        foundation::Memory::Destruct(rtv_descriptor_pages_[i]);
      }

      for (uint32_t i = 0; i < uav_descriptor_pages_.size(); ++i)
      {
        foundation::Memory::Destruct(uav_descriptor_pages_[i]);
      }
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::Initialize(
      uint32_t srv_heap_size,
      uint32_t dsv_heap_size,
      uint32_t rtv_heap_size,
      uint32_t uav_heap_size)
    {      
      srv_heap_size_ = srv_heap_size;
      dsv_heap_size_ = dsv_heap_size;
      rtv_heap_size_ = rtv_heap_size;
      uav_heap_size_ = uav_heap_size;

      AddSRVPage();
      AddDSVPage();
      AddRTVPage();
      AddUAVPage();

      return true;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::AllocateSRVDescriptor(uint32_t& out_heap_index)
    {
      bool found = false;

      uint32_t page_index = 0;

      do
      {
        if (srv_descriptor_pages_.size() == page_index)
        {
          AddSRVPage();
        }

        if (srv_descriptor_pages_[page_index]->IsFull())
        {
          ++page_index;
          continue;
        }

        if (srv_descriptor_pages_[page_index]->AllocateDescriptor(out_heap_index))
        {
          found = true;
        }
        else
        {
          ++page_index;
        }
      }
      while (!found);

      out_heap_index += srv_heap_size_ * page_index;

      return true;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::AllocateDSVDescriptor(uint32_t& out_heap_index)
    {
      bool found = false;

      uint32_t page_index = 0;

      do
      {
        if (dsv_descriptor_pages_.size() == page_index)
        {
          AddDSVPage();
        }

        if (dsv_descriptor_pages_[page_index]->IsFull())
        {
          ++page_index;
          continue;
        }

        if (dsv_descriptor_pages_[page_index]->AllocateDescriptor(out_heap_index))
        {
          found = true;
        }
        else
        {
          ++page_index;
        }
      }
      while (!found);

      out_heap_index += srv_heap_size_ * page_index;

      return true;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::AllocateRTVDescriptor(uint32_t& out_heap_index)
    {
      bool found = false;

      uint32_t page_index = 0;

      do
      {
        if (rtv_descriptor_pages_.size() == page_index)
        {
          AddRTVPage();
        }

        if (rtv_descriptor_pages_[page_index]->IsFull())
        {
          ++page_index;
          continue;
        }

        if (rtv_descriptor_pages_[page_index]->AllocateDescriptor(out_heap_index))
        {
          found = true;
        }
        else
        {
          ++page_index;
        }
      }
      while (!found);

      out_heap_index += srv_heap_size_ * page_index;

      return true;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::AllocateUAVDescriptor(uint32_t& out_heap_index)
    {
      bool found = false;

      uint32_t page_index = 0;

      do
      {
        if (uav_descriptor_pages_.size() == page_index)
        {
          AddUAVPage();
        }

        if (uav_descriptor_pages_[page_index]->IsFull())
        {
          ++page_index;
          continue;
        }

        if (uav_descriptor_pages_[page_index]->AllocateDescriptor(out_heap_index))
        {
          found = true;
        }
        else
        {
          ++page_index;
        }
      }
      while (!found);

      out_heap_index += srv_heap_size_ * page_index;

      return true;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::FreeSRVDescriptor(uint32_t heap_index)
    {
      uint32_t page_index = heap_index / srv_heap_size_;
      uint32_t slot_index = heap_index % srv_heap_size_;

      if (srv_descriptor_pages_[page_index]->FreeDescriptor(slot_index))
      {
        return true;
      }

      return false;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::FreeDSVDescriptor(uint32_t heap_index)
    {
      uint32_t page_index = heap_index / dsv_heap_size_;
      uint32_t slot_index = heap_index % dsv_heap_size_;

      if (dsv_descriptor_pages_[page_index]->FreeDescriptor(slot_index))
      {
        return true;
      }

      return false;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::FreeRTVDescriptor(uint32_t heap_index)
    {
      uint32_t page_index = heap_index / rtv_heap_size_;
      uint32_t slot_index = heap_index % rtv_heap_size_;

      if (rtv_descriptor_pages_[page_index]->FreeDescriptor(slot_index))
      {
        return true;
      }

      return false;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::FreeUAVDescriptor(uint32_t heap_index)
    {
      uint32_t page_index = heap_index / uav_heap_size_;
      uint32_t slot_index = heap_index % uav_heap_size_;

      if (uav_descriptor_pages_[page_index]->FreeDescriptor(slot_index))
      {
        return true;
      }

      return false;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::GetCPUHandleForSRVDescriptor(
      uint32_t heap_index,
      D3D12_CPU_DESCRIPTOR_HANDLE& out_cpu_handle)
    {
      uint32_t page_index = heap_index / srv_heap_size_;
      uint32_t slot_index = heap_index % srv_heap_size_;

      out_cpu_handle = srv_descriptor_pages_[page_index]->GetCPUDescriptorHandle(slot_index);

      // TODO(Yana): Never returns an error.
      // Should probably return false whenever the handle points to a non-allocated descriptor
      return true;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::GetCPUHandleForDSVDescriptor(
      uint32_t heap_index,
      D3D12_CPU_DESCRIPTOR_HANDLE& out_cpu_handle)
    {
      uint32_t page_index = heap_index / dsv_heap_size_;
      uint32_t slot_index = heap_index % dsv_heap_size_;

      out_cpu_handle = dsv_descriptor_pages_[page_index]->GetCPUDescriptorHandle(slot_index);

      // TODO(Yana): Never returns an error.
      // Should probably return false whenever the handle points to a non-allocated descriptor
      return true;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::GetCPUHandleForRTVDescriptor(
      uint32_t heap_index,
      D3D12_CPU_DESCRIPTOR_HANDLE& out_cpu_handle)
    {
      uint32_t page_index = heap_index / rtv_heap_size_;
      uint32_t slot_index = heap_index % rtv_heap_size_;

      out_cpu_handle = rtv_descriptor_pages_[page_index]->GetCPUDescriptorHandle(slot_index);

      // TODO(Yana): Never returns an error.
      // Should probably return false whenever the handle points to a non-allocated descriptor
      return true;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::GetCPUHandleForUAVDescriptor(
      uint32_t heap_index,
      D3D12_CPU_DESCRIPTOR_HANDLE& out_cpu_handle)
    {
      uint32_t page_index = heap_index / uav_heap_size_;
      uint32_t slot_index = heap_index % uav_heap_size_;

      out_cpu_handle = uav_descriptor_pages_[page_index]->GetCPUDescriptorHandle(slot_index);

      // TODO(Yana): Never returns an error.
      // Should probably return false whenever the handle points to a non-allocated descriptor
      return true;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::GetGPUHandleForSRVDescriptor(
      uint32_t heap_index,
      D3D12_GPU_DESCRIPTOR_HANDLE& out_gpu_handle)
    {
      uint32_t page_index = heap_index / srv_heap_size_;
      uint32_t slot_index = heap_index % srv_heap_size_;

      out_gpu_handle = srv_descriptor_pages_[page_index]->GetGPUDescriptorHandle(slot_index);

      // TODO(Yana): Never returns an error.
      // Should probably return false whenever the handle points to a non-allocated descriptor
      return true;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::GetGPUHandleForDSVDescriptor(
      uint32_t heap_index,
      D3D12_GPU_DESCRIPTOR_HANDLE& out_gpu_handle)
    {
      uint32_t page_index = heap_index / dsv_heap_size_;
      uint32_t slot_index = heap_index % dsv_heap_size_;

      out_gpu_handle = dsv_descriptor_pages_[page_index]->GetGPUDescriptorHandle(slot_index);

      // TODO(Yana): Never returns an error.
      // Should probably return false whenever the handle points to a non-allocated descriptor
      return true;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::GetGPUHandleForRTVDescriptor(
      uint32_t heap_index,
      D3D12_GPU_DESCRIPTOR_HANDLE& out_gpu_handle)
    {
      uint32_t page_index = heap_index / rtv_heap_size_;
      uint32_t slot_index = heap_index % rtv_heap_size_;

      out_gpu_handle = rtv_descriptor_pages_[page_index]->GetGPUDescriptorHandle(slot_index);

      // TODO(Yana): Never returns an error.
      // Should probably return false whenever the handle points to a non-allocated descriptor
      return true;
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12PersistentDescriptorHeap::GetGPUHandleForUAVDescriptor(
      uint32_t heap_index,
      D3D12_GPU_DESCRIPTOR_HANDLE& out_gpu_handle)
    {
      uint32_t page_index = heap_index / uav_heap_size_;
      uint32_t slot_index = heap_index % uav_heap_size_;

      out_gpu_handle = uav_descriptor_pages_[page_index]->GetGPUDescriptorHandle(slot_index);

      // TODO(Yana): Never returns an error.
      // Should probably return false whenever the handle points to a non-allocated descriptor
      return true;
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12PersistentDescriptorHeap::AddSRVPage()
    {
      srv_descriptor_pages_.push_back(
        foundation::Memory::Construct<D3D12DescriptorPage>(
          device_,
          D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
          srv_heap_size_)
      );
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12PersistentDescriptorHeap::AddDSVPage()
    {
      dsv_descriptor_pages_.push_back(
        foundation::Memory::Construct<D3D12DescriptorPage>(
          device_,
          D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
          dsv_heap_size_)
      );
    }
    
    //------------------------------------------------------------------------------------------------------
    void D3D12PersistentDescriptorHeap::AddRTVPage()
    {
      rtv_descriptor_pages_.push_back(
        foundation::Memory::Construct<D3D12DescriptorPage>(
          device_,
          D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
          rtv_heap_size_)
      );
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12PersistentDescriptorHeap::AddUAVPage()
    {
      uav_descriptor_pages_.push_back(
        foundation::Memory::Construct<D3D12DescriptorPage>(
          device_,
          D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
          uav_heap_size_)
      );
    }



  }
}