#include "d3d12_material_manager.h"

#include "d3d12_asset.h"

#include <graphics/d3d12/d3d12_device_wrapper.h>
#include <graphics/d3d12/utils/d3d12_utility.h>
#include <graphics/d3d12/d3d12_frame_descriptor_heap.h>

#include <foundation/memory/memory.h>

#include <engine/assets/material.h>
#include <engine/assets/post_process_material.h>
#include <engine/assets/texture.h>

namespace sulphur
{
  namespace graphics
  {
    //------------------------------------------------------------------------------------------------------
    D3D12Material::D3D12Material(
      const foundation::Vector<D3D12Resource*>& srvs,
      const foundation::Vector<D3D12Resource*>& uavs)
      : was_used_this_frame_(false)
    {
      // Yolo (time to kms <3)
      for (size_t i = 0; i < srvs.size(); ++i)
      {
        persistent_srv_handles_.push_back(srvs[i]->srv_persistent_index_);
      }

      for (size_t i = 0; i < uavs.size(); ++i)
      {
        persistent_uav_handles_.push_back(uavs[i]->uav_persistent_index_);
      }

      descriptor_table_handle_ = D3D12_GPU_DESCRIPTOR_HANDLE();
      constant_buffer_handle_ = D3D12_GPU_VIRTUAL_ADDRESS();
    }

    //------------------------------------------------------------------------------------------------------
    D3D12Material::D3D12Material(const D3D12Material& other)
    {
      for (size_t i = 0; i < other.persistent_srv_handles_.size(); ++i)
      {
        persistent_srv_handles_.push_back(other.persistent_srv_handles_[i]);
      }
      for (size_t i = 0; i < other.persistent_uav_handles_.size(); ++i)
      {
        persistent_uav_handles_.push_back(other.persistent_uav_handles_[i]);
      }

      // TODO: Should the code below be there :thinking:
      was_used_this_frame_ = other.was_used_this_frame_;
	  descriptor_table_handle_ = other.descriptor_table_handle_;
      constant_buffer_handle_ = other.constant_buffer_handle_;
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Material::Reset()
    {
      was_used_this_frame_ = false;
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Material::CopyDescriptorsToFrameDescriptorHeap(D3D12FrameDescriptorHeap& frame_desc_heap)
    {
      if (was_used_this_frame_ == true)
      {
        return;
      }

      descriptor_table_handle_ = D3D12_GPU_DESCRIPTOR_HANDLE();

      D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;
      D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle;

      for (size_t i = 0; i < persistent_srv_handles_.size(); ++i)
      {
        frame_desc_heap.CopySRVDescriptor(persistent_srv_handles_[i], cpu_handle, gpu_handle);
        if (i == 0)
        {
          descriptor_table_handle_ = gpu_handle;
        }
      }

      for (size_t i = 0; i < persistent_uav_handles_.size(); ++i)
      {
        frame_desc_heap.CopyUAVDescriptor(persistent_uav_handles_[i], cpu_handle, gpu_handle);
        if (i == 0)
        {
          if (persistent_srv_handles_.empty() == true)
          {
            descriptor_table_handle_ = gpu_handle;
          }
        }
      }

      was_used_this_frame_ = true;
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12Material::CopyConstantBufferData(D3D12ConstantBufferHeap&/* constant_buffer_heap*/)
    {
      // Needs implementation...
    }

    //------------------------------------------------------------------------------------------------------

    using material_pair = eastl::pair<const size_t, D3D12Material*>; //!< Typedef for a pair of material ID and material pointer
    using material_iterator = eastl::rbtree_iterator<material_pair, material_pair*, material_pair&>; //!< Typedef for iterator over a pair of material ID and material pointer

    //------------------------------------------------------------------------------------------------------
    D3D12MaterialManager::D3D12MaterialManager(D3D12Device& device) :
      device_(device),
      current_material_(nullptr),
      materials_(nullptr)
    {

    }

    //------------------------------------------------------------------------------------------------------
    void D3D12MaterialManager::StartFrame()
    {
      for (material_pair& key : materials_)
      {
        key.second->Reset();
      }
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12MaterialManager::OnDestroy()
    {
      for (material_pair& key : materials_)
      {
        foundation::Memory::Destruct(key.second);
      }

      materials_.clear();
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12MaterialManager::SetMaterial(const D3D12Material& material)
    {
      size_t hash_code = 0;
      const static size_t random_seed = 54678785653423;
      const foundation::Vector<uint32_t>& srvs = material.persistent_srv_handles();
      const foundation::Vector<uint32_t>& uavs = material.persistent_uav_handles();

      for (size_t i = 0; i < srvs.size(); ++i)
      {
        graphics::hash_combine(hash_code, srvs[i]);
      }

      graphics::hash_combine(hash_code, random_seed);

      for (size_t i = 0; i < uavs.size(); ++i)
      {
        graphics::hash_combine(hash_code, uavs[i]);
      }

      material_iterator it = materials_.find(hash_code);
      if (it != materials_.end())
      {
        assert(it->second);
        current_material_ = it->second;
        return;
      }

      D3D12Material*& mat = materials_[hash_code];
      mat = foundation::Memory::Construct<D3D12Material>(material);

      current_material_ = mat;
    }
  }
}