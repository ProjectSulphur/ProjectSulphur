#pragma once
#include "d3d11_asset_types.h"

#include <engine/assets/mesh.h>
#include <engine/assets/texture.h>
#include <engine/assets/shader.h>
#include <engine/assets/shader_program.h>
#include <engine/assets/asset_interfaces.h>

#include <foundation/containers/vector.h>

namespace sulphur
{
  namespace graphics
  {
    class D3D11Device;

    class D3D11AssetManager : public engine::IGPUAssetManager
    {
    public:
      D3D11AssetManager(D3D11Device& device);
      void OnDestroy();

      void CreateMesh(const engine::MeshHandle& mesh);
      void CreateTexture(const engine::TextureHandle& texture);
      void CreateDepthBuffer(const engine::TextureHandle& texture);
      void CreateRenderTarget(const engine::TextureHandle& texture);
      void CreateShader(const engine::ShaderHandle& shader);

      D3D11Mesh* GetMesh(const engine::MeshHandle& handle) const;
      D3D11Texture* GetTexture(const engine::TextureHandle& handle) const;
      D3D11Shader* GetShader(const engine::ShaderHandle& handle) const;

      D3D11Asset* GetAsset(const engine::GPUAssetHandle& handle) const;

      void Release(engine::GPUAssetHandle& handle) override;

    private:
      void CreateVertexShaderProgram(const engine::ShaderProgramHandle& shader_program);
      void CreateHullShaderProgram(const engine::ShaderProgramHandle& shader_program);
      void CreateDomainShaderProgram(const engine::ShaderProgramHandle& shader_program);
      void CreateGeometryShaderProgram(const engine::ShaderProgramHandle& shader_program);
      void CreatePixelShaderProgram(const engine::ShaderProgramHandle& shader_program);

      D3D11Device& device_;

      foundation::Vector<D3D11Asset*> assets_;
    };
  }
}