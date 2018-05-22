#include "d3d12_asset_manager.h"

#include "d3d12_asset.h"
#include "graphics/d3d12/d3d12_root_signature.h"
#include "graphics/d3d12/d3d12_vertex.h"
#include "graphics/d3d12/d3d12_device_wrapper.h"
#include "graphics/d3d12/d3dx12.h"

#include <foundation/memory/memory.h>
#include <foundation/logging/logger.h>
#include <engine/assets/mesh.h>
#include <engine/assets/texture.h>

namespace sulphur
{
  namespace graphics
  {
    //------------------------------------------------------------------------------------------------------
    D3D12MeshAssetManager::D3D12MeshAssetManager(D3D12Device& device)
      : device_(device)
    {
      D3D12Mesh* invalid_mesh = foundation::Memory::Construct<D3D12Mesh>();
      meshes_.push_back(invalid_mesh);
    }

    //------------------------------------------------------------------------------------------------------
    D3D12MeshAssetManager::~D3D12MeshAssetManager()
    {
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12MeshAssetManager::OnDestroy()
    {
      for (size_t i = 0; i < meshes_.size(); ++i)
      {
        if (meshes_[i] != nullptr)
        {
          foundation::Memory::Destruct<D3D12Mesh>(meshes_[i]);
        }
      }
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12MeshAssetManager::Create(const engine::MeshHandle& mesh)
    {
      meshes_.push_back(foundation::Memory::Construct<D3D12Mesh>());
      size_t index = meshes_.size() - 1;

      engine::Mesh* mesh_data = mesh.GetRaw();

      foundation::Vector<Vertex> vertices;

      const foundation::Vector<glm::fvec3>& positions = mesh_data->GetVertices();
      const foundation::Vector<glm::fvec3>& normals = mesh_data->GetNormals();
      const foundation::Vector<glm::fvec3>& tangents = mesh_data->GetTangents();
      const foundation::Vector<glm::fvec2>& uv = mesh_data->GetUVs();
      const foundation::Vector<foundation::Color>& colors = mesh_data->GetColors();
      const foundation::Vector<uint32_t>& indices = mesh_data->GetIndices();

      Vertex temp;

      for (size_t i = 0; i < mesh_data->GetVertexCount(); ++i)
      {
        temp.position = positions[i];
        temp.normal = normals.empty() ? glm::fvec3() : normals[i];
        temp.tangent = tangents.empty() ? glm::fvec3() : tangents[i];
        temp.color = colors.empty() ? foundation::Color::kWhite : colors[i];
        temp.uv = uv.empty() ? glm::fvec2() : uv[i];

        vertices.push_back(temp);
      }

      device_.CreateVertexAndIndexBuffer(
        meshes_[index]->vertex_buffer_,
        meshes_[index]->index_buffer_,
        meshes_[index]->vb_view_,
        meshes_[index]->ib_view_,
        vertices.data(),
        static_cast<uint32_t>(vertices.size()),
        indices.data(),
        static_cast<uint32_t>(indices.size())
      );

      meshes_[index]->vertex_count_ = vertices.size();
      meshes_[index]->index_count_ = indices.size();

      engine::GPUAssetHandle& handle = mesh.GetGPUHandle();
      handle = engine::GPUAssetHandle(this, static_cast<uintptr_t>(index));
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12MeshAssetManager::Release(engine::GPUAssetHandle& handle)
    {
      size_t index = static_cast<size_t>(handle);
      if (index == 0)
      {
        // log warning - attempting to release non-existing resource
      }

      assert(index < meshes_.size());

      foundation::Memory::Destruct<D3D12Mesh>(meshes_[index]);
      meshes_[index] = nullptr;
      // TODO: PROBABLY SHOULDN'T HAVE THE ARRAY/VECTOR GET LIKE 9427597698892 big <3
    }

    //------------------------------------------------------------------------------------------------------
    D3D12Mesh* D3D12MeshAssetManager::GetMesh(engine::GPUAssetHandle& handle) const
    {
      return meshes_[static_cast<size_t>(handle)];
    }

    //------------------------------------------------------------------------------------------------------
    D3D12TextureAssetManager::D3D12TextureAssetManager(D3D12Device& device) :
      device_(device)
    {
      textures_.push_back(nullptr);
    }

    //------------------------------------------------------------------------------------------------------
    D3D12TextureAssetManager::~D3D12TextureAssetManager()
    {
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12TextureAssetManager::OnDestroy()
    {
      for (uint32_t i = 0; i < textures_.size(); ++i)
      {
        if (textures_[i] != nullptr)
        {
          foundation::Memory::Destruct<D3D12Texture2D>(textures_[i]);
        }
      }
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12TextureAssetManager::Create(
      const engine::TextureHandle& texture,
      const D3D12TextureType type,
      const foundation::Color clear_color)
    {
      engine::Texture* texture_data = texture.GetRaw();

      D3D12Texture2D* created_texture = foundation::Memory::Construct<D3D12Texture2D>();
      
      D3D12_RESOURCE_DESC desc = {};

      switch (texture->format())
      {
      case sulphur::engine::TextureFormat::kR8G8B8A8_UNORM:
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        break;
      case sulphur::engine::TextureFormat::kR32G32B32A32_FLOAT:
        desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        break;
      case sulphur::engine::TextureFormat::kR16G16B16A16_FLOAT:
        desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        break;
      case sulphur::engine::TextureFormat::kR32_TYPELESS:
        desc.Format = DXGI_FORMAT_R32_TYPELESS;
        break;
      case sulphur::engine::TextureFormat::kR24G8_TYPELESS:
        desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
        break;
      default:
        desc.Format = DXGI_FORMAT_UNKNOWN;
        break;
      }

      desc.DepthOrArraySize = 1;
      desc.Width = texture_data->width();
      desc.Height = texture_data->height();
      desc.MipLevels = 1;
      desc.SampleDesc.Count = 1;
      desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

      device_.CreateTexture2D(
        texture_data->raw_data().data(),
        desc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        nullptr,
        1,
        created_texture,
        type,
        clear_color);
      
      engine::GPUAssetHandle& handle = texture.GetGPUHandle();
      handle = engine::GPUAssetHandle(this, static_cast<uintptr_t>(textures_.size()));

      textures_.push_back(created_texture);
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12TextureAssetManager::Release(engine::GPUAssetHandle& handle)
    {
      size_t index = static_cast<size_t>(handle);
      if (index == 0)
      {
        // log warning - attempting to release non-existing resource
      }

      assert(index < textures_.size());

      foundation::Memory::Destruct<D3D12Texture2D>(textures_[index]);
      textures_[index] = nullptr;

      // TODO: PROBABLY SHOULDN'T HAVE THE ARRAY/VECTOR GET LIKE 9427597698892 big <3
    }

    //------------------------------------------------------------------------------------------------------
    D3D12Texture2D* D3D12TextureAssetManager::GetTexture(engine::GPUAssetHandle& handle) const
    {
      size_t index = static_cast<size_t>(handle);
      if (index == 0)
      {
        // log error - accessing invalid texture
      }
      assert(index < textures_.size());

      return textures_[index];
    }

    //------------------------------------------------------------------------------------------------------
    D3D12ShaderAssetManager::D3D12ShaderAssetManager(D3D12Device& device) :
      device_(device)
    {
      // First is invalid
      root_signatures_.push_back(nullptr);
    }

    //------------------------------------------------------------------------------------------------------
    D3D12ShaderAssetManager::~D3D12ShaderAssetManager()
    {
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12ShaderAssetManager::OnDestroy()
    {
      for (uint32_t i = 0; i < root_signatures_.size(); ++i)
      {
        if (root_signatures_[i] != nullptr)
        {
          foundation::Memory::Destruct(root_signatures_[i]);
        }
      }
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12ShaderAssetManager::Create(const engine::ShaderHandle& shader)
    {
      // Use shader reflection to create a root signature for the shader
      ID3D12RootSignature* d3d12_rs = D3D12RootSignature::CreateRootSignatureFromShader(
        shader,
        device_);

      D3D12RootSignature* root_signature = foundation::Memory::Construct<D3D12RootSignature>(
        d3d12_rs,
        1,
        1,
        0);

      engine::GPUAssetHandle& handle = shader.GetGPUHandle();
      handle = engine::GPUAssetHandle(this, static_cast<uintptr_t>(root_signatures_.size()));

      root_signatures_.push_back(root_signature);
    }

    //------------------------------------------------------------------------------------------------------
    void D3D12ShaderAssetManager::Release(engine::GPUAssetHandle& handle)
    {
      size_t index = static_cast<size_t>(handle);
      if (index == 0)
      {
        foundation::DefaultLogger::Log(
          foundation::Verbosity::kWarning,
          "Attempted to release invalid D3D12 shader asset.\n");
      }

      assert(index < root_signatures_.size());

      foundation::Memory::Destruct(root_signatures_[index]);
      root_signatures_[index] = nullptr;

      // TODO: PROBABLY SHOULDN'T HAVE THE ARRAY/VECTOR GET LIKE 9427597698892 big <3
    }

    //------------------------------------------------------------------------------------------------------
    D3D12RootSignature*
      D3D12ShaderAssetManager::GetRootSignatureForShader(engine::GPUAssetHandle& handle) const
    {
      size_t index = static_cast<size_t>(handle);
      if (index == 0)
      {
        PS_LOG(Warning, "Attempted to get invalid D3D12 shader asset.\n");
      }
      assert(index < root_signatures_.size());

      return root_signatures_[index];
    }


}
}