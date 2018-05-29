#pragma once
#include <foundation/containers/vector.h>

#include <d3d12.h>
#include <cstdint>

namespace sulphur 
{
  namespace graphics 
  {
    class D3D12Device;
    class D3D12PersistentDescriptorHeap;
    
    /**
    * @class sulphur::graphics::D3D12FrameDescriptorHeap
    * @brief D3D12 descriptor heap, updated every frame and bound to the graphics pipeline.
    * @todo Really need to find a better, more relevant name for this class.
    * @todo Add descriptor heaps of type D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER.
    * @author Yana Mateeva
    */
    class D3D12FrameDescriptorHeap
    {
    public:
      /**
      * @brief Constructor for the frame descriptor heap.
      * @param[in] device (sulphur::graphics::D3D12Device&) Reference to a DirectX 12 device wrapper.
      * @param[in] persistent_desc_heap (sulphur::graphics::D3D12PersistentDescriptorHeap&) Reference to a persistent descriptor heap.
      */
      D3D12FrameDescriptorHeap(
        D3D12Device& device,
        D3D12PersistentDescriptorHeap& persistent_desc_heap
      );

      /**
      * @brief Destructor.
      */
      ~D3D12FrameDescriptorHeap();

      /**
      * @brief Releases all D3D12 heap resources.
      */
      void OnDestroy();

      /**
      * @brief Initializes the frame descriptor heap. Creates the D3D12 resources required.
      * @param[in] frame_buffer_count (uint32_t) The amount of back buffers used by the swap chain.
      * @param[in] srv_uav_heap_size (size_t) The amount of reserved descriptors for SRVs and UAVs in the SRV/UAV heap.
      * @param[in] rtv_heap_size (size_t) The amount of reserved descriptors for RTVs in the RTV heap.
      * @param[in] dsv_heap_size (size_t) The amount of reserved descriptors for DSVs in the DSV heap.
      */
      void Initialize(
        uint32_t frame_buffer_count,
        size_t srv_uav_heap_size,
        size_t rtv_heap_size,
        size_t dsv_heap_size
      );

      /**
      * @brief Used to signal the frame descriptor heap that a new frame has started.
      * @param[in] frame_index (uint32_t) The current back buffer index (as retrieved from the swap chain).
      */
      void StartFrame(uint32_t frame_index);

      /**
      * @brief Copies an SRV descriptor from the persistent descriptor heap to the frame (bound) descriptor heap.
      * @param[in] persistent_index (uint32_t) The index of the SRV inside the persistent descriptor heap.
      * @param[out] out_cpu_handle (D3D12_CPU_DESCRIPTOR_HANDLE&) The CPU handle of the SRV inside the frame descriptor heap.
      * @param[out] out_gpu_handle (D3D12_GPU_DESCRIPTOR_HANDLE&) The GPU handle of the SRV inside the frame descriptor heap.
      */
      void CopySRVDescriptor(
        uint32_t persistent_index,
        D3D12_CPU_DESCRIPTOR_HANDLE& out_cpu_handle,
        D3D12_GPU_DESCRIPTOR_HANDLE& out_gpu_handle
      );

      /**
      * @brief Copies an UAV descriptor from the persistent descriptor heap to the frame (bound) descriptor heap.
      * @param[in] persistent_index (uint32_t) The index of the UAV inside the persistent descriptor heap.
      * @param[out] out_cpu_handle (D3D12_CPU_DESCRIPTOR_HANDLE&) The CPU handle of the UAV inside the frame descriptor heap.
      * @param[out] out_gpu_handle (D3D12_GPU_DESCRIPTOR_HANDLE&) The GPU handle of the UAV inside the frame descriptor heap.
      */
      void CopyUAVDescriptor(
        uint32_t persistent_index,
        D3D12_CPU_DESCRIPTOR_HANDLE& out_cpu_handle,
        D3D12_GPU_DESCRIPTOR_HANDLE& out_gpu_handle
      );

      /**
      * @brief Copies an RTV descriptor from the persistent descriptor heap to the frame (bound) descriptor heap.
      * @param[in] persistent_index (uint32_t) The index of the RTV inside the persistent descriptor heap.
      * @param[out] out_cpu_handle (D3D12_CPU_DESCRIPTOR_HANDLE&) The CPU handle of the RTV inside the frame descriptor heap.
      * @param[out] out_gpu_handle (D3D12_GPU_DESCRIPTOR_HANDLE&) The GPU handle of the RTV inside the frame descriptor heap.
      */
      void CopyRTVDescriptor(
        uint32_t persistent_index,
        D3D12_CPU_DESCRIPTOR_HANDLE& out_cpu_handle,
        D3D12_GPU_DESCRIPTOR_HANDLE& out_gpu_handle
      );

      /**
      * @brief Copies a DSV descriptor from the persistent descriptor heap to the frame (bound) descriptor heap.
      * @param[in] persistent_index (uint32_t) The index of the DSV inside the persistent descriptor heap.
      * @param[out] out_cpu_handle (D3D12_CPU_DESCRIPTOR_HANDLE&) The CPU handle of the DSV inside the frame descriptor heap.
      * @param[out] out_gpu_handle (D3D12_GPU_DESCRIPTOR_HANDLE&) The GPU handle of the DSV inside the frame descriptor heap.
      */
      void CopyDSVDescriptor(
        uint32_t persistent_index,
        D3D12_CPU_DESCRIPTOR_HANDLE& out_cpu_handle,
        D3D12_GPU_DESCRIPTOR_HANDLE& out_gpu_handle
      );

      /**
      * @return (ID3D12DescriptorHeap*) The D3D12 descriptor heap, type SRV/UAV, for the specified frame.
      */
      ID3D12DescriptorHeap* srv_uav_heap(uint32_t frame_index) { return srv_uav_heap_[frame_index]; }
      //ID3D12DescriptorHeap* rtv_heap(uint32_t frame_index) { return rtv_heap_[frame_index]; }
      //ID3D12DescriptorHeap* dsv_heap(uint32_t frame_index) { return dsv_heap_[frame_index]; }

    private:
      foundation::Vector<ID3D12DescriptorHeap*> srv_uav_heap_; //!< Vector, containing an SRV/UAV heap for each back buffer.
      foundation::Vector<ID3D12DescriptorHeap*> rtv_heap_; //!< Vector, containing an RTV heap for each back buffer.
      foundation::Vector<ID3D12DescriptorHeap*> dsv_heap_; //!< Vector, containing a DSV heap for each back buffer.

      size_t srv_uav_heap_size_; //!< The amount of reserved descriptors for SRVs and UAVs in the SRV/UAV heap.
      size_t rtv_heap_size_; //!< The amount of reserved descriptors for RTVs in the RTV heap.
      size_t dsv_heap_size_; //!< The amount of reserved descriptors for DSVs in the DSV heap.

      uint32_t current_frame_; //!< The current back buffer index.

      uint32_t current_srv_uav_write_index_; //!< The current index of the next free descriptor in the SRV/UAV heap.
      uint32_t current_rtv_write_index_; //!< The current index of the next free RTV descriptor in the RTV heap.
      uint32_t current_dsv_write_index_; //!< The current index of the next free DSV descriptor in the DSV heap.

      uint32_t srv_uav_descriptor_size_; //!< The handle increment size for a heap of type D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV.
      uint32_t rtv_descriptor_size_; //!< The handle increment size for a heap of type D3D12_DESCRIPTOR_HEAP_TYPE_RTV.
      uint32_t dsv_descriptor_size_; //!< The handle increment size for a heap of type D3D12_DESCRIPTOR_HEAP_TYPE_DSV.

      D3D12Device& device_; //!< A reference to the wrapper around a ID3D12Device.
      D3D12PersistentDescriptorHeap& persistent_descriptor_heap_; //!< A reference to the persistent descriptor heap.
    };
  }
}