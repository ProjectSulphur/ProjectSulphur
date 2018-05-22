#include "d3d12_material_manager.h"

#include "d3d12_asset.h"

#include <graphics/d3d12/d3d12_device_wrapper.h>
#include <graphics/d3d12/utils/d3d12_utility.h>
#include <graphics/d3d12/d3d12_frame_descriptor_heap.h>

#include <foundation/memory/memory.h>
#include <engine/assets/material.h>
#include <engine/assets/texture.h>

namespace sulphur
{
  namespace graphics
  {
    //------------------------------------------------------------------------------------------------------
    D3D12Material::D3D12Material(D3D12Device& device, const engine::MaterialPass& material) : 
      was_used_this_frame_(false)
    {
      const foundation::Vector<engine::TextureHandle>& textures = material.textures();
      
      D3D12Texture2D* curr_texture;

      for (size_t i = 0; i < textures.size(); ++i)
      {
        // TODO: I need info whether the texture is an UAV or an SRV....!!!
        curr_texture = device.texture_asset_manager().GetTexture(textures[i].GetGPUHandle());
        // Currently assuming all bound textures are SRVs

        // Create SRV for this texture if required
        if (curr_texture->has_srv_ == false)
        {
          device.CreateShaderResourceView(curr_texture);
        }

        persistent_srv_handles_.push_back(curr_texture->srv_persistent_index_);

      }
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

      D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;
      D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle;

      for (size_t i = 0; i < persistent_srv_handles_.size(); ++i)
      {
        frame_desc_heap.CopySRVDescriptor(persistent_srv_handles_[i], cpu_handle, gpu_handle);
        if (i == 0)
        {
          srv_descriptor_table_handle_ = gpu_handle;
        }
      }

      for (size_t i = 0; i < persistent_uav_handles_.size(); ++i)
      {
        frame_desc_heap.CopySRVDescriptor(persistent_uav_handles_[i], cpu_handle, gpu_handle);
        if (i == 0)
        {
          uav_descriptor_table_handle_ = gpu_handle;
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
    void D3D12MaterialManager::SetMaterial(const engine::MaterialPass& material)
    {
      size_t hash_code = graphics::HashState(&material);

      material_iterator it = materials_.find(hash_code);
      if (it != materials_.end())
      {
        assert(it->second);
        current_material_ = it->second;
        return;
      }

      // Take a reference to the pointer in the map (not constructed yet)
      // And create it
      D3D12Material*& mat = materials_[hash_code];
      mat = foundation::Memory::Construct<D3D12Material>(device_, material);

      current_material_ = mat;
    }


  }
}