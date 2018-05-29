#pragma once

#include "graphics/d3d12/assets/d3d12_asset_manager.h"
#include "graphics/d3d12/d3d12_persistent_descriptor_heap.h"

#include <foundation/memory/memory.h>
#include <foundation/utils/color.h>

#include <d3d12.h>
#include <cstdint>

namespace sulphur 
{
  namespace graphics 
  {
    struct Vertex;
    struct D3D12Texture2D;

    /**
    * @class sulphur::graphics::D3D12Device
    * @brief Wrapper class for the d3d12 device
    * @author Yana Mateeva
    */
    class D3D12Device
    {

    public:

      /**
      * @brief Default constructor.
      */
      D3D12Device();

      /**
      * @brief Creates D3D12 resources.
      */
      void Create();

      /**
      * @brief Destructor.
      */
      ~D3D12Device();

      /**
      * @brief Destroys the device. Releases all resources.
      */
      void OnDestroy();

      /**
      * @brief Creates a d3d12 command queue
      * @param[out] out_command_queue (ID3D12CommandQueue*&) The created command queue
      * @param[in] type (D3D12_COMMAND_LIST_TYPE) The command queue type
      * @param[in] priority (D3D12_COMMAND_QUEUE_PRIORITY) The priority of the command queue
      * @param[in] flags (D3D12_COMMAND_QUEUE_FLAGS) The command queue flags
      * @return (bool) Has the command queue been created successfully?
      */
      bool CreateCommandQueue(
        ID3D12CommandQueue*& out_command_queue,
        D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT,
        D3D12_COMMAND_QUEUE_PRIORITY priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        D3D12_COMMAND_QUEUE_FLAGS flags = D3D12_COMMAND_QUEUE_FLAG_NONE
      );

      /**
      * @brief Creates a d3d12 graphics command list
      * @param[out] out_command_list (ID3D12GraphicsCommandList*&) The created command list
      * @param[in] command_allocator (ID3D12CommandAllocator*) The command allocator associated with the command list
      * @param[in] type (D3D12_COMMAND_LIST_TYPE) The command list type
      * @return (bool) Has the command list been created successfully?
      */
      bool CreateGraphicsCommandList(
        ID3D12GraphicsCommandList*& out_command_list,
        ID3D12CommandAllocator* command_allocator,
        D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT,
        ID3D12PipelineState* pso = nullptr
      );

      /**
      * @brief Creates a d3d12 command allocator
      * @param[out] out_command_allocator (ID3D12CommandAllocator*&) The created command allocator
      * @param[in] type (D3D12_COMMAND_LIST_TYPE) The command queue type
      * @return (bool) Has the command allocator been created successfully?
      */
      bool CreateCommandAllocator(
        ID3D12CommandAllocator*& out_command_allocator,
        D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT
      );

      /**
      * @brief Creates a d3d12 descriptor heap
      * @param[out] out_descriptor_heap (ID3D12DescriptorHeap*&) The created descriptor heap
      * @param[in] type (D3D12_DESCRIPTOR_HEAP_TYPE) The type of the descriptor heap
      * @param[in] num_descriptors (uint32_t) The number of descriptors available in the descriptor heap
      * @param[in] flags (D3D12_DESCRIPTOR_HEAP_FLAGS) The descriptor heap flags
      * @return (bool) Has the descriptor heap been created successfully?
      */
      bool CreateDescriptorHeap(
        ID3D12DescriptorHeap*& out_descriptor_heap,
        D3D12_DESCRIPTOR_HEAP_TYPE type,
        uint32_t num_descriptors,
        D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE
      );
      
      /**
      * @brief Get the descriptor size of a specified descriptor heap type (platform dependent)
      * @param[in] type (D3D12_DESCRIPTOR_HEAP_TYPE) The type of the descriptor
      * @return (uint32_t) The size of the descriptor of the specified type
      */
      uint32_t GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type);

      /**
      * @brief Create a d3d12 heap
      * @param[out] out_heap (ID3D12Heap*&) The created heap
      * @param[in] heap_size (size_t) The size of the heap to create (in bytes)
      * @param[in] alignment (size_t) The alignment value of the heap
      * @param[in] flags (D3D12_HEAP_FLAGS) The heap flags
      * @param[in] type (D3D12_HEAP_TYPE) The type of the heap to create
      * @param[in] cpu_page_property (D3D12_CPU_PAGE_PROPERTY) The CPU-page properties for the heap
      * @param[in] memory_pool (D3D12_MEMORY_POOL) The memory pool for the heap
      * @return (bool) Has the heap been created successfully?
      */
      bool CreateHeap(
        ID3D12Heap*& out_heap,
        size_t heap_size,
        size_t alignment,
        D3D12_HEAP_FLAGS flags,
        D3D12_HEAP_TYPE type,
        D3D12_CPU_PAGE_PROPERTY cpu_page_property,
        D3D12_MEMORY_POOL memory_pool
      );

      /**
      * @brief Create a committed resource
      * @param[out] out_resource (ID3D12Resource*&) The created committed resource
      * @param[in] heap_properties (const D3D12_HEAP_PROPERTIES&) Heap properties 
      * @param[in] flags (D3D12_HEAP_FLAGS) The resource flags
      * @param[in] resource_desc (const D3D12_RESOURCE_DESC&) Resource description struct
      * @param[in] initial_state (D3D12_RESOURCE_STATES) The initial resource state of the resource
      * @param[in] clear_value (D3D12_CLEAR_VALUE*) The optimized clear value for the resource
      * @return (bool) Has the committed resource been created successfully?
      */
      bool CreateCommittedResource(
        ID3D12Resource*& out_resource,
        const D3D12_HEAP_PROPERTIES& heap_properties,
        D3D12_HEAP_FLAGS flags,
        const D3D12_RESOURCE_DESC& resource_desc,
        D3D12_RESOURCE_STATES initial_state,
        D3D12_CLEAR_VALUE* clear_value
      );

      /**
      * @brief Create a graphics pipeline state object.
      * @param[out] out_pso (ID3D12PipelineState*&) The created graphics pipeline state object.
      * @param[in] desc (const D3D12_GRAPHICS_PIPELINE_STATE_DESC&) Description of the graphics pipeline state.
      * @return (bool) Has the pipeline state object been created successfully?
      */
      bool CreateGraphicsPipelineState(
        ID3D12PipelineState*& out_pso,
        const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc
      );

      /**
      * @brief Create a compute pipeline state object.
      * @param[out] out_pso (ID3D12PipelineState*&) The created compute pipeline state object.
      * @param[in] desc (const D3D12_COMPUTE_PIPELINE_STATE_DESC&) Description of the compute pipeline state.
      * @return (bool) Has the compute pipeline state object been created successfully?
      */
      bool CreateComputePipelineState(
        ID3D12PipelineState*& out_pso,
        const D3D12_COMPUTE_PIPELINE_STATE_DESC& desc
      );

      /**
      * @brief Create a versioned root signature
      * @param[out] out_root_signature (ID3D12RootSignature*&) The created root signature
      * @param[in] desc (const D3D12_VERSIONED_ROOT_SIGNATURE_DESC&) The root signature description
      * @param[in] max_version (D3D_ROOT_SIGNATURE_VERSION) The maximum supported root signature version
      * @return (bool) Has the root signature been created successfully?
      */
      bool CreateVersionedRootSignature(
        ID3D12RootSignature*& out_root_signature,
        const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& desc,
        D3D_ROOT_SIGNATURE_VERSION max_version = D3D_ROOT_SIGNATURE_VERSION_1_1
      );

      /**
      * @brief Creates a render target view for a texture.
      * @param[in] texture (sulphur::graphics::D3D12Texture2D*) The texture to create a render target view for
      */
      void CreateRenderTargetView(D3D12Texture2D* texture);

      /**
      * @brief Create a render target view
      * @param[in] resource (ID3D12Resource*) The resource to create a render target view for
      * @param[in] desc (const D3D12_RENDER_TARGET_VIEW_DESC&) The render target view description
      * @param[in] out_heap_handle (uint32_t&) A handle for this descriptor in the persistent descriptor heap
      */
      void CreateRenderTargetView(
        ID3D12Resource* resource,
        const D3D12_RENDER_TARGET_VIEW_DESC& desc,
        uint32_t& out_heap_handle
      );

      /**
      * @brief Create a depth-stencil view for a texture
      * @param[in] texture (sulphur::graphics::D3D12Texture2D*) The texture to create a depth-stencil view for
      */
      void CreateDepthStencilView(D3D12Texture2D* texture);

      /**
      * @brief Create a depth-stencil view
      * @param[in] depth_stencil_resource (ID3D12Resource*) The resource to create a depth-stencil view for
      * @param[in] format (DXGI_FORMAT) The format of the depth-stencil view resource
      * @param[in] dimension (D3D12_DSV_DIMENSION) The dimension of the depth-stencil view
      * @param[in] flags (D3D12_DSV_FLAGS) The depth-stencil view flags
      * @param[in] cpu_handle (D3D12_CPU_DESCRIPTOR_HANDLE) Target CPU descriptor handle
      */
      void CreateDepthStencilView(
        ID3D12Resource* depth_stencil_resource,
        DXGI_FORMAT format,
        D3D12_DSV_DIMENSION dimension,
        D3D12_DSV_FLAGS flags,
        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle
      );

      /**
      * @brief Create a d3d12 fence object
      * @param[out] out_fence (ID3D12Fence*&) The created fence object
      * @param[in] initial_value (uint64_t) The initial value of the fence
      * @param[in] flags (D3D12_FENCE_FLAGS) The fence flags
      * @return (bool) Has the fence object been created successfully?
      */
      bool CreateFence(
        ID3D12Fence*& out_fence,
        uint64_t initial_value = 0,
        D3D12_FENCE_FLAGS flags = D3D12_FENCE_FLAG_NONE
      );

      /**
      * @brief Create a shader resource view for a texture.
      * @param[in] texture (sulphur::graphics::D3D12Texture2D*) The texture to create a shader resource view for
      */
      void CreateShaderResourceView(D3D12Texture2D* texture);

      /**
      * @brief Create a shader resource view
      * @param[in] resource (ID3D12Resource*) The resource to create a shader resource view for
      * @param[in] srv_desc (const D3D12_SHADER_RESOURCE_VIEW_DESC&) The shader resource view description
      * @param[in] out_srv_persistent_index (uint32_t&) A handle for this SRV in the persistent descriptor heap
      */
      void CreateShaderResourceView(
        ID3D12Resource* resource,
        const D3D12_SHADER_RESOURCE_VIEW_DESC& srv_desc,
        uint32_t& out_srv_persistent_index
      );

      /**
      * @brief Create an unordered access view.
      * @remarks Currently doesn't allow to have a counter resource (always created with nullptr)
      * @param[in] resource (ID3D12Resource*) The resource to create an unordered access view for
      * @param[in] srv_desc (const D3D12_UNORDERED_ACCESS_VIEW_DESC&) The unordered access view description
      * @param[in] out_uav_persistent_index (uint32_t&) A handle for this UAV in the persistent descriptor heap
      */
      void CreateUnorderedAccessView(
        ID3D12Resource* resource,
        const D3D12_UNORDERED_ACCESS_VIEW_DESC& uav_desc,
        uint32_t& out_uav_persistent_index
      );

      /**
      * @brief Create an unordered access view for a texture.
      * @remarks Currently doesn't allow to have a counter resource (always created with nullptr)
      * @param[in] texture (sulphur::graphics::D3D12Texture2D*) The texture to create an unordered access view for
      */
      void CreateUnorderedAccessView(D3D12Texture2D* texture);

      /**
      * @brief Copy descriptors from one descriptor heap to another
      * @param[in] num_descriptors (uint32_t) The number of consecutive descriptors to copy
      * @param[in] dest_descriptor_range_start (D3D12_CPU_DESCRIPTOR_HANDLE) The destination CPU handle of the first descriptor in the range
      * @param[in] src_descriptor_range_start (D3D12_CPU_DESCRIPTOR_HANDLE) The source CPU handle of the first descriptor in the range
      * @param[in] type (D3D12_DESCRIPTOR_HEAP_TYPE) The type of the descriptors being copied
      */
      void CopyDescriptorsSimple(
        uint32_t num_descriptors,
        D3D12_CPU_DESCRIPTOR_HANDLE dest_descriptor_range_start,
        D3D12_CPU_DESCRIPTOR_HANDLE src_descriptor_range_start,
        D3D12_DESCRIPTOR_HEAP_TYPE type
      );

      /**
      * @brief Creates vertex and index buffers, given an array of vertices and indices
      * @param[out] out_vertex_buffer (ID3D12Resource*&) The d3d12 resource corresponding to the vertex buffer
      * @param[out] out_index_buffer (ID3D12Resource*&) The d3d12 resource corresponding to the index buffer
      * @param[out] out_vertex_buffer_view (D3D12_VERTEX_BUFFER_VIEW&) The created vertex buffer view
      * @param[out] out_index_buffer_view (D3D12_INDEX_BUFFER_VIEW&) The created index buffer view
      * @param[in] vertex_array (const Vertex*) The array of vertices
      * @param[in] vertex_count (uint32_t) The number of vertices
      * @param[in] index_array (const uint32_t*) The array of indices
      * @param[in] index_count (uint32_t) The number of indices
      * @return (bool) Were the buffers created successfully?
      */
      bool CreateVertexAndIndexBuffer(
        ID3D12Resource*& out_vertex_buffer,
        ID3D12Resource*& out_index_buffer,
        D3D12_VERTEX_BUFFER_VIEW& out_vertex_buffer_view,
        D3D12_INDEX_BUFFER_VIEW& out_index_buffer_view,
        const Vertex* vertex_array,
        size_t vertex_count,
        const uint32_t* index_array,
        size_t index_count
      );

      /**
      * @brief Create a texture 2D from data
      * @param[in] bytes (const uint8_t) The texture data to create the resource with
      * @param[in] desc (D3D12_RESOURCE_DESC&) Structure, which describes the resource to create
      * @param[in] initial_state (D3D12_RESOURCE_STATES) The initial resource state to use for this resource
      * @param[in] sub_res_data (D3D12_SUBRESOURCE_DATA*) Pointer to an array of pointers of subresource descriptions
      * @param[in] mip_count (uint32_t) The amount of mipmaps for this resource
      * @param[out] out_texture (sulphur::graphics::D3D12Texture2D*) The created texture object
      * @param[in] clear_value (D3D12_CLEAR_VALUE*) The clear value for the texture resource
      * @return (bool) Was the texture created successfully?
      */
      bool CreateTexture2D(
        const uint8_t* bytes,
        D3D12_RESOURCE_DESC& desc,
        D3D12_RESOURCE_STATES initial_state,
        D3D12_SUBRESOURCE_DATA* sub_res_data,
        uint32_t mip_count,
        D3D12Texture2D* out_texture,
        D3D12_CLEAR_VALUE* clear_value
      );

      // TODO: Temporary. I find better fix. <3
      /**
      * @return (sulphur::graphics::D3D12PersistentDescriptorHeap&) The persistent descriptor heap. Stores all descriptors currently loaded in GPU memory. Not sorted.
      */
      D3D12PersistentDescriptorHeap& persistent_descriptor_heap() { return persistent_descriptor_heap_; }

      // TODO: Temporary. I find better fix. <3
      /**
      * @return (sulphur::graphics::D3D12MeshAssetManager&) The mesh asset manager. Can find a mesh object by its GPU handle.
      */
      D3D12MeshAssetManager& mesh_asset_manager() { return mesh_asset_manager_; }

      // TODO: Temporary. I find better fix. <3
      /**
      * @return (sulphur::graphics::D3D12TextureAssetManager&) The texture asset manager. Can find a texture object by its GPU handle.
      */
      D3D12TextureAssetManager& texture_asset_manager() { return texture_asset_manager_; }

      // TODO: Temporary. I find better fix. <3
      /**
      * @return (sulphur::graphics::D3D12ShaderAssetManager&) The shader asset manager. Can find a shader/root signature by its GPU handle.
      */
      D3D12ShaderAssetManager& shader_asset_manager() { return shader_asset_manager_; }

      /**
      * @return (ID3D12Device*) The D3D12 device object.
      */
      const ID3D12Device* device() const { return device_; }

    private:
      /**
      * @brief Convert feature level to a string
      * @param[in] feature_level (const D3D_FEATURE_LEVEL&) The feature level to get a string from
      * @return (char*) The converted string
      */
      const char* GetFeatureLevelString(const D3D_FEATURE_LEVEL& feature_level) const;

      /**
      * @brief Retrieves the size of a pixel based on a texture format
      * @param[in] format (DXGI_FORMAT) The format to retrieve a size from
      * @return (size_t) The size in bytes
      */
      size_t GetSizeFromFormat(DXGI_FORMAT format) const;

      D3D12PersistentDescriptorHeap persistent_descriptor_heap_; //!< Persistent descriptor heap. Stores all descriptors currently loaded in GPU memory. Not sorted.

      D3D12MeshAssetManager mesh_asset_manager_; //!< The mesh asset manager. Can find a mesh object by its GPU handle.

      D3D12TextureAssetManager texture_asset_manager_; //!< The texture asset manager. Can find a texture object by its GPU handle.

      D3D12ShaderAssetManager shader_asset_manager_; //!< The shader asset manager. Can find a shader/root signature by its GPU handle.

      ID3D12CommandQueue* command_queue_; //!< The command queue, reserved for exectuing device operations, such as copying resources.
      ID3D12GraphicsCommandList* command_list_; //!< The command list, reserved for recording device operations, such as copying resources.
      ID3D12CommandAllocator* command_allocator_; //!< The command allocator, associated with the device command list
      ID3D12Fence* fence_; //!< Fence object. Used to sync CPU/GPU after executing a device command queue
      uint64_t fence_value_; //!< Fence value. Tracks the current signaled fence value
      HANDLE fence_event_; //!< Fence event associated with the fence object. If sync is required, this event will be set when command queue is finished executing.

      ID3D12Device1* device_; //!< The D3D12 device interface object

    };
  }
}