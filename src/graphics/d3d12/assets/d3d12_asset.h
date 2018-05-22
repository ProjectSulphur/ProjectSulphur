#pragma once
#include <glm/glm.hpp>

#include <d3d12.h>
#include <cstdint>

namespace sulphur
{
  namespace graphics
  {
    /**
    * @struct sulphur::graphics::D3D12Resource
    * @brief Wrapper of a DirectX 12 resource object. Tracks state and provides easy transition.
    * @author Yana Mateeva
    */
    struct D3D12Resource
    {
      /**
      * @brief Constructor
      */
      D3D12Resource();

      /**
      * @brief Virtual destructor.
      */
      virtual ~D3D12Resource() = 0 {}

      /**
      * @brief Provides a resource barrier to transition this resource to a new state.
      * @param[in] new_state (D3D12_RESOURCE_STATES) The new state of this resource.
      * @param[out] out_barrier (D3D12_RESOURCE_BARRIER&) The resource barrier to use to transition this resource.
      * @return (bool) Does this resource need to transition using a resource barrier?
      */
      bool Transition(D3D12_RESOURCE_STATES new_state, D3D12_RESOURCE_BARRIER& out_barrier);

      ID3D12Resource* resource_; //!< The DirectX 12 resource object.
      D3D12_RESOURCE_STATES current_state_; //!< The current state of this resource.
      D3D12_RESOURCE_DESC resource_desc_; //!< The resource description.
      uint32_t mip_count_; //!< The amount of mip levels for this resource.
    };

    /**
    * @struct sulphur::graphics::D3D12Mesh
    * @brief GPU mesh asset
    * @author Yana Mateeva
    */
    struct D3D12Mesh
    {
      /**
      * @brief Default constructor.
      */
      D3D12Mesh();

      /**
      * @brief Constructor.
      * @param[in] vertex_buffer (ID3D12Resource*) The vertex buffer resource for this mesh.
      * @param[in] index_buffer (ID3D12Resource*) The index buffer resource for this mesh.
      * @param[in] vb_view (D3D12_VERTEX_BUFFER_VIEW) The vertex buffer view for this mesh.
      * @param[in] ib_view (D3D12_INDEX_BUFFER_VIEW) The index buffer view for this mesh.
      * @param[in] index_count (size_t) The amount of indices of this mesh.
      * @param[in] vertex_count (size_t) The amount of vertices in this mesh.
      */
      D3D12Mesh(
        ID3D12Resource* vertex_buffer,
        ID3D12Resource* index_buffer,
        D3D12_VERTEX_BUFFER_VIEW vb_view,
        D3D12_INDEX_BUFFER_VIEW ib_view,
        size_t index_count,
        size_t vertex_count
      );

      /**
      * @brief Destructor. Releases the DirectX 12 resources for this mesh.
      */
      ~D3D12Mesh();

      ID3D12Resource* vertex_buffer_; //!< The vertex buffer resource.
      ID3D12Resource* index_buffer_; //!< The index buffer resource.
      D3D12_VERTEX_BUFFER_VIEW vb_view_; //!< The vertex buffer view.
      D3D12_INDEX_BUFFER_VIEW ib_view_; //!< The index buffer view.
      size_t index_count_; //!< The amount of indices of this mesh.
      size_t vertex_count_; //!< The amount of vertices in this mesh.
    };

    /**
    * @enum sulphur::graphics::D3D12TextureType
    * @brief An enumerator for defining the type of a texture
    */
    enum struct D3D12TextureType
    {
      kTexture, //!< Shader resource texture.
      kRenderTarget, //!< A render target texture.
      kDepthStencil //!< A depth stencil texture.
    };

    /**
    * @struct sulphur::graphics::D3D12Texture2D : sulphur::graphics::D3D12Resource
    * @brief GPU texture 2D asset
    * @author Yana Mateeva
    */
    struct D3D12Texture2D : public D3D12Resource
    {
      /**
      * @brief Constructor.
      */
      D3D12Texture2D();
      
      /**
      * @brief Destructor.
      */
      ~D3D12Texture2D();

      DXGI_FORMAT format_; //!< The format of this texture
      glm::vec2 size_; //!< The size of this texture.

      bool has_srv_; //!< Does this texture have a shader resource view created?
      bool has_dsv_; //!< Does this texture have a depth-stencil view created?
      bool has_rtv_; //!< Does this texture have a render target view created?

      // TODO: add handles for other descriptor types if necessary
      uint32_t srv_persistent_index_; //!< The handle for this texture's shader resource view in the persistent descriptor heap
      uint32_t dsv_persistent_index_; //!< The handle for this texture's depth_stencil view in the persistent descriptor heap
      uint32_t rtv_persistent_index_; //!< The handle for this texture's render target view in the persistent descriptor heap
    };

  }
}