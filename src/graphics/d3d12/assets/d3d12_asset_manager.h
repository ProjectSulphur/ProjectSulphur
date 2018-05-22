#pragma once
#include "d3d12_asset.h"

#include <engine/assets/asset_interfaces.h>
#include <engine/assets/mesh.h>
#include <engine/assets/texture.h>
#include <engine/assets/shader.h>

#include <foundation/utils/color.h>
#include <foundation/containers/vector.h>

#include <d3d12.h>

namespace sulphur
{
  namespace graphics
  {
    class D3D12Device;
    class D3D12RootSignature;

    /**
    * @class sulphur::graphics::D3D12MeshAssetManager : public sulphur::engine::IGPUAssetManager
    * @brief A GPU asset manager for DirectX 12 mesh objects.
    * @author Yana Mateeva
    */
    class D3D12MeshAssetManager : public engine::IGPUAssetManager
    {
    public:
      /**
      * @brief Constructor.
      * @param[in] device (sulphur::graphics::D3D12Device&) A reference to a DirectX 12 device object.
      */
      D3D12MeshAssetManager(D3D12Device& device);

      /**
      * @brief Destructor.
      */
      ~D3D12MeshAssetManager();

      /**
      * @brief Releases all D3D12 resources within this object.
      */
      void OnDestroy();

      /**
      * @brief Creates necessary GPU resources for a mesh.
      * @param[in] mesh (const sulphur::engine::MeshHandle&) A handle to a mesh to create GPU resources for.
      */
      void Create(const engine::MeshHandle& mesh);

      /**
      * @see sulphur::engine::GPUAssetManager::Release
      */
      virtual void Release(engine::GPUAssetHandle& handle) override;

      /**
      * @brief Retrieves a DirectX 12 mesh object by a given GPU asset handle.
      * @param[in] handle (sulphur::engine::GPUAssetHandle&) The handle corresponding to the mesh
      */
      D3D12Mesh* GetMesh(engine::GPUAssetHandle& handle) const;

    private:
      foundation::Vector<D3D12Mesh*> meshes_; //!< Collection of GPU meshes.
      D3D12Device& device_; //!< The DirectX 12 device object
    };

    /**
    * @class sulphur::graphics::D3D12TextureAssetManager : public sulphur::engine::IGPUAssetManager
    * @brief A GPU asset manager for DirectX 12 texture objects.
    * @author Yana Mateeva
    */
    class D3D12TextureAssetManager : public engine::IGPUAssetManager
    {
    public:
      /**
      * @brief Constructor.
      * @param[in] device (sulphur::graphics::D3D12Device&) A reference to a DirectX 12 device object.
      */
      D3D12TextureAssetManager(D3D12Device& device);

      /**
      * @brief Destructor.
      */
      ~D3D12TextureAssetManager();

      /**
      * @brief Releases all D3D12 resources within this object. 
      */
      void OnDestroy();

      /**
      * @brief Creates GPU resources for a texture.
      * @param[in] texture (const sulphur::engine::TextureHandle&) A handle to the texture to create resources for.
      * @param[in] type (const sulphur::graphics::D3D12TextureType) The type of the texture to create resources for.
      * @param[in] clear_color (sulphur::foundation::Color) 
      */
      void Create(
        const engine::TextureHandle& texture,
        const D3D12TextureType type,
        const foundation::Color clear_color = foundation::Color::kBlackTransparent
      );

      /**
      * @see sulphur::engine::GPUAssetManager::Release
      */
      virtual void Release(engine::GPUAssetHandle& handle) override;

      /**
      * @brief Retrieves a DirectX 12 texture object by a given GPU asset handle.
      * @param[in] handle (sulphur::engine::GPUAssetHandle&) The handle corresponding to the texture
      */
      D3D12Texture2D* GetTexture(engine::GPUAssetHandle& handle) const;

    private:

      foundation::Vector<D3D12Texture2D*> textures_; //!< Collection of GPU textures.
      D3D12Device& device_; //!< Reference to the DirectX 12 device.
    };

    /**
    * @class sulphur::graphics::D3D12ShaderAssetManager : public sulphur::engine::IGPUAssetManager
    * @brief A GPU asset manager for DirectX 12 shader resources. 
    * @author Yana Mateeva
    */
    class D3D12ShaderAssetManager : public engine::IGPUAssetManager
    {
    public:
      /**
      * @brief Constructor.
      * @param[in] device (sulphur::graphics::D3D12Device&) A reference to the DirectX 12 device.
      */
      D3D12ShaderAssetManager(D3D12Device& device);

      /**
      * @brief Destructor.
      */
      ~D3D12ShaderAssetManager();

      /**
      * @brief Releases all DirectX 12 resources within this object.
      */
      void OnDestroy();

      /**
      * @brief Creates GPU resources for a given shader.
      * @param[in] shader (const sulphur::engine::ShaderHandle&) A handle to the shader to create resources for.
      */
      void Create(const engine::ShaderHandle& shader);

      /**
      * @see sulphur::engine::IGPUAssetManager::Release
      */
      virtual void Release(engine::GPUAssetHandle& handle) override;

      /**
      * @brief Retrieves a root signature for a shader by a given GPU asset handle.
      * @param[in] handle (sulphur::engine::GPUAssetHandle&) The handle corresponding to the shader to get a root signature for.
      */
      D3D12RootSignature* GetRootSignatureForShader(engine::GPUAssetHandle& handle) const;

    private:
      foundation::Vector<D3D12RootSignature*> root_signatures_; //!< Collection of root signatures for shaders.
      D3D12Device& device_; //!< Reference to the DirectX 12 device.
    };
  }
}