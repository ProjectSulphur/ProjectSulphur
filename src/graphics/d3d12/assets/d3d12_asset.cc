#include "d3d12_asset.h"

#include "graphics/d3d12/d3d12_root_signature.h"
#include "graphics/d3d12/d3dx12.h"

#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace graphics
  {
    //------------------------------------------------------------------------------------------------------
    D3D12Mesh::D3D12Mesh() :
      vertex_buffer_(nullptr),
      index_buffer_(nullptr),
      index_count_(0),
      vertex_count_(0)
    {
    }

    //------------------------------------------------------------------------------------------------------
    D3D12Mesh::D3D12Mesh(
      ID3D12Resource* vertex_buffer,
      ID3D12Resource* index_buffer,
      D3D12_VERTEX_BUFFER_VIEW vb_view,
      D3D12_INDEX_BUFFER_VIEW ib_view,
      size_t index_count,
      size_t vertex_count) :
      vertex_buffer_(vertex_buffer),
      index_buffer_(index_buffer),
      vb_view_(vb_view),
      ib_view_(ib_view),
      index_count_(index_count),
      vertex_count_(vertex_count)
    {
    }

    //------------------------------------------------------------------------------------------------------
    D3D12Mesh::~D3D12Mesh()
    {
      SafeRelease(index_buffer_);
      SafeRelease(vertex_buffer_);
    }

    //------------------------------------------------------------------------------------------------------
    D3D12Texture2D::D3D12Texture2D(
      D3D12Resource* first_buffer,
      D3D12Resource* second_buffer) :
      has_srv_(false),
      has_dsv_(false),
      has_rtv_(false),
      has_uav_(false),
      buffers_{first_buffer, second_buffer},
      has_ping_pong_(second_buffer != nullptr)
    {}

    //------------------------------------------------------------------------------------------------------
    D3D12Texture2D::~D3D12Texture2D()
    {
		for (uint32_t i = 0; i < 2; ++i)
		{
			if (buffers_[i] != nullptr)
			{
				foundation::Memory::Destruct(buffers_[i]);
			}
		}
    }

    //------------------------------------------------------------------------------------------------------
    D3D12Resource::D3D12Resource() :
      resource_(nullptr)
    {}

    //------------------------------------------------------------------------------------------------------
    D3D12Resource::~D3D12Resource()
    {
      SafeRelease(resource_);
    }

    //------------------------------------------------------------------------------------------------------
    bool D3D12Resource::Transition(D3D12_RESOURCE_STATES new_state, D3D12_RESOURCE_BARRIER& out_barrier)
    {
      if (new_state != current_state_)
      {
        out_barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource_, current_state_, new_state);
        current_state_ = new_state;
        return true;
      }

      return false;
    }

  }
}