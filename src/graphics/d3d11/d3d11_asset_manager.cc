#include "d3d11_asset_manager.h"
#include "d3d11_device_wrapper.h"

#include <engine/assets/mesh.h>
#include <engine/assets/texture.h>
#include <engine/assets/shader.h>

#include <foundation/memory/memory.h>

#include <d3d11.h>
#include <glm/glm.hpp>

namespace sulphur
{
  namespace graphics
  {
    //--------------------------------------------------------------------------------
    D3D11AssetManager::D3D11AssetManager(D3D11Device& device) :
      device_(device)
    {}

    //--------------------------------------------------------------------------------
    void D3D11AssetManager::OnDestroy()
    {
      for (D3D11Asset* asset : assets_)
      {
        foundation::Memory::Destruct<D3D11Asset>(asset);
      }

      assets_.clear();
    }

    //--------------------------------------------------------------------------------
    void D3D11AssetManager::CreateMesh(const engine::MeshHandle& mesh_handle)
    {
      D3D11Mesh* gpu_mesh = foundation::Memory::Construct<D3D11Mesh>();
      engine::Mesh* mesh = mesh_handle.GetRaw();

      gpu_mesh->index_count = mesh->GetIndexCount();

      foundation::Vector<D3D11Vertex> verticies(mesh->GetVertexCount());

      const foundation::Vector<glm::vec3>& positions = mesh->GetVertices();
      const foundation::Vector<glm::vec3>& normals = mesh->GetNormals();
      const foundation::Vector<glm::vec3>& tangents = mesh->GetTangents();
      const foundation::Vector<glm::vec2>&  uvs = mesh->GetUVs();
      const foundation::Vector<foundation::Color>& colors = mesh->GetColors();
      
      for (size_t i = 0; i < mesh->GetVertexCount(); ++i)
      {
        verticies[i] = D3D11Vertex
        {
          positions[i],
          normals.empty()   ? glm::vec3()           : normals[i],
          tangents.empty()  ? glm::vec3()           : tangents[i],
          uvs.empty()       ? glm::vec2()           : uvs[i],
          colors.empty()    ? foundation::Color::kWhite : colors[i]
        };
      }

      device_.CreateBuffer(gpu_mesh->index_buffer, mesh->GetIndices().data(), sizeof(uint32_t) * mesh->GetIndexCount(), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE, (D3D11_CPU_ACCESS_FLAG)0, 0);
      device_.CreateBuffer(gpu_mesh->vertex_buffer, verticies.data(), sizeof(D3D11Vertex) * mesh->GetVertexCount(), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE, (D3D11_CPU_ACCESS_FLAG)0, 0);

      assets_.emplace_back(std::move(gpu_mesh));

      mesh_handle.GetGPUHandle() = engine::GPUAssetHandle(this, assets_.size());
    }

    //--------------------------------------------------------------------------------
    void D3D11AssetManager::CreateTexture(const engine::TextureHandle& texture_handle)
    {
      D3D11Texture* gpu_texture = foundation::Memory::Construct<D3D11Texture>();
      engine::Texture* texture = texture_handle.GetRaw();

      device_.CreateTexture(texture->width(), texture->height(), texture->format(), texture->raw_data().data(), 
        gpu_texture->texture, gpu_texture->resource_view, false, false);
      
      assets_.emplace_back(std::move(gpu_texture));
      texture_handle.GetGPUHandle() = engine::GPUAssetHandle(this, assets_.size());
    }

    //--------------------------------------------------------------------------------
    void D3D11AssetManager::CreateDepthBuffer(const engine::TextureHandle& texture_handle)
    {
      D3D11Texture* gpu_texture = foundation::Memory::Construct<D3D11Texture>();
      engine::Texture* texture = texture_handle.GetRaw();

      device_.CreateTexture(texture->width(), texture->height(), texture->format(), texture->raw_data().data(), 
        gpu_texture->texture, gpu_texture->resource_view, true, false);

      device_.CreateDepthBufferFromTexture(gpu_texture->texture, gpu_texture->depth_stencil_view);

      assets_.emplace_back(std::move(gpu_texture));
      texture_handle.GetGPUHandle() = engine::GPUAssetHandle(this, assets_.size());
    }

    //--------------------------------------------------------------------------------
    void D3D11AssetManager::CreateRenderTarget(const engine::TextureHandle& texture_handle)
    {
      D3D11Texture* gpu_texture = foundation::Memory::Construct<D3D11Texture>();
      engine::Texture* texture = texture_handle.GetRaw();

      device_.CreateTexture(texture->width(), texture->height(), texture->format(), texture->raw_data().data(), 
        gpu_texture->texture, gpu_texture->resource_view, false, true);

      device_.CreateRenderTargetFromTexture(gpu_texture->texture, gpu_texture->render_target_view);

      assets_.emplace_back(std::move(gpu_texture));
      texture_handle.GetGPUHandle() = engine::GPUAssetHandle(this, assets_.size());
    }

    //--------------------------------------------------------------------------------
    void D3D11AssetManager::CreateShader(const engine::ShaderHandle& shader_handle)
    {
      D3D11Shader* gpu_shader = foundation::Memory::Construct<D3D11Shader>();
      engine::Shader* shader = shader_handle.GetRaw();

      if (shader->vertex_shader())
      {
        if (!shader->vertex_shader().GetGPUHandle())
          CreateVertexShaderProgram(shader->vertex_shader());

        gpu_shader->vertex_shader = static_cast<D3D11ShaderProgram<ID3D11VertexShader>*>(
          GetAsset(shader->vertex_shader().GetGPUHandle()));
      }

      if (shader->hull_shader())
      {
        if (!shader->hull_shader().GetGPUHandle())
          CreateHullShaderProgram(shader->hull_shader());

        gpu_shader->hull_shader = static_cast<D3D11ShaderProgram<ID3D11HullShader>*>(
          GetAsset(shader->hull_shader().GetGPUHandle()));
      }

      if (shader->domain_shader())
      {
        if (!shader->domain_shader().GetGPUHandle())
          CreateDomainShaderProgram(shader->domain_shader());

        gpu_shader->domain_shader = static_cast<D3D11ShaderProgram<ID3D11DomainShader>*>(
          GetAsset(shader->domain_shader().GetGPUHandle()));
      }

      if (shader->geometry_shader())
      {
        if (!shader->geometry_shader().GetGPUHandle())
          CreateGeometryShaderProgram(shader->geometry_shader());

        gpu_shader->geometry_shader = static_cast<D3D11ShaderProgram<ID3D11GeometryShader>*>(
          GetAsset(shader->geometry_shader().GetGPUHandle()));
      }

      if (shader->pixel_shader())
      {
        if (!shader->pixel_shader().GetGPUHandle())
          CreatePixelShaderProgram(shader->pixel_shader());

        gpu_shader->pixel_shader = static_cast<D3D11ShaderProgram<ID3D11PixelShader>*>(
          GetAsset(shader->pixel_shader().GetGPUHandle()));
      }
      
      assets_.emplace_back(std::move(gpu_shader));
      shader_handle.GetGPUHandle() = engine::GPUAssetHandle(this, assets_.size());
    }

    //--------------------------------------------------------------------------------
    void D3D11AssetManager::CreateVertexShaderProgram(const engine::ShaderProgramHandle& shader_program_handle)
    {
      D3D11ShaderProgram<ID3D11VertexShader>* gpu_shader_program = foundation::Memory::Construct<
        D3D11ShaderProgram<ID3D11VertexShader>>();

      engine::ShaderProgram* shader_program = shader_program_handle.GetRaw();

      if (shader_program->IsValid())
      {
        device_.CreateVertexShader(
          shader_program->byte_code_data(),
          shader_program->byte_code_size(),
          gpu_shader_program->shader_program);
      }

      assets_.emplace_back(std::move(gpu_shader_program));
      shader_program_handle.GetGPUHandle() = engine::GPUAssetHandle(this, assets_.size());
    }

    //--------------------------------------------------------------------------------
    void D3D11AssetManager::CreateHullShaderProgram(const engine::ShaderProgramHandle& shader_program_handle)
    {
      D3D11ShaderProgram<ID3D11HullShader>* gpu_shader_program = foundation::Memory::Construct<
        D3D11ShaderProgram<ID3D11HullShader>>();

      engine::ShaderProgram* shader_program = shader_program_handle.GetRaw();

      if (shader_program->IsValid())
      {
        device_.CreateHullShader(
          shader_program->byte_code_data(),
          shader_program->byte_code_size(),
          gpu_shader_program->shader_program);
      }

      assets_.emplace_back(std::move(gpu_shader_program));
      shader_program_handle.GetGPUHandle() = engine::GPUAssetHandle(this, assets_.size());
    }

    //--------------------------------------------------------------------------------
    void D3D11AssetManager::CreateDomainShaderProgram(const engine::ShaderProgramHandle& shader_program_handle)
    {
      D3D11ShaderProgram<ID3D11DomainShader>* gpu_shader_program = foundation::Memory::Construct<
        D3D11ShaderProgram<ID3D11DomainShader>>();

      engine::ShaderProgram* shader_program = shader_program_handle.GetRaw();

      if (shader_program->IsValid())
      {
        device_.CreateDomainShader(
          shader_program->byte_code_data(),
          shader_program->byte_code_size(),
          gpu_shader_program->shader_program);
      }

      assets_.emplace_back(std::move(gpu_shader_program));
      shader_program_handle.GetGPUHandle() = engine::GPUAssetHandle(this, assets_.size());
    }

    //--------------------------------------------------------------------------------
    void D3D11AssetManager::CreateGeometryShaderProgram(const engine::ShaderProgramHandle& shader_program_handle)
    {
      D3D11ShaderProgram<ID3D11GeometryShader>* gpu_shader_program = foundation::Memory::Construct<
        D3D11ShaderProgram<ID3D11GeometryShader>>();

      engine::ShaderProgram* shader_program = shader_program_handle.GetRaw();

      if (shader_program->IsValid())
      {
        device_.CreateGeometryShader(
          shader_program->byte_code_data(),
          shader_program->byte_code_size(),
          gpu_shader_program->shader_program);
      }

      assets_.emplace_back(std::move(gpu_shader_program));
      shader_program_handle.GetGPUHandle() = engine::GPUAssetHandle(this, assets_.size());
    }

    //--------------------------------------------------------------------------------
    void D3D11AssetManager::CreatePixelShaderProgram(const engine::ShaderProgramHandle& shader_program_handle)
    {
      D3D11ShaderProgram<ID3D11PixelShader>* gpu_shader_program = foundation::Memory::Construct<
        D3D11ShaderProgram<ID3D11PixelShader>>();

      engine::ShaderProgram* shader_program = shader_program_handle.GetRaw();

      if (shader_program->IsValid())
      {
        device_.CreatePixelShader(
          shader_program->byte_code_data(),
          shader_program->byte_code_size(),
          gpu_shader_program->shader_program);
      }

      assets_.emplace_back(std::move(gpu_shader_program));
      shader_program_handle.GetGPUHandle() = engine::GPUAssetHandle(this, assets_.size());
    }

    //--------------------------------------------------------------------------------
    D3D11Mesh* D3D11AssetManager::GetMesh(const engine::MeshHandle& handle) const
    {
      return static_cast<D3D11Mesh*>(GetAsset(handle.GetGPUHandle()));
    }

    //--------------------------------------------------------------------------------
    D3D11Texture* D3D11AssetManager::GetTexture(const engine::TextureHandle& handle) const
    {
      return static_cast<D3D11Texture*>(GetAsset(handle.GetGPUHandle()));
    }

    //--------------------------------------------------------------------------------
    D3D11Shader* D3D11AssetManager::GetShader(const engine::ShaderHandle& handle) const
    {
      return static_cast<D3D11Shader*>(GetAsset(handle.GetGPUHandle()));
    }

    //--------------------------------------------------------------------------------
    D3D11Asset* D3D11AssetManager::GetAsset(const engine::GPUAssetHandle& handle) const
    {
      if (handle && assets_.size() >= static_cast<uintptr_t>(handle))
      {
        return assets_[static_cast<uintptr_t>(handle) - 1];
      }

      return nullptr;
    }

    //--------------------------------------------------------------------------------
    void D3D11AssetManager::Release(engine::GPUAssetHandle& handle)
    {
      if (!handle)
      {
        return;
      }

      D3D11Asset* data = assets_[static_cast<uintptr_t>(handle) - 1];
      foundation::Memory::Destruct<D3D11Asset>(data);
      data = nullptr;
    }
  }
}