#pragma once
#include <foundation/containers/vector.h>

#include <cstdint>
#include <d3d12.h>

namespace sulphur 
{
  namespace graphics 
  {
    class D3D12Device;

    /**
    * @class sulphur::graphics::D3D12PersistentDescriptorHeap
    * @brief Holds persistent descriptors for all texture resources currently loaded in GPU memory.
    * @author Yana Mateeva
    */
    class D3D12PersistentDescriptorHeap
    {
    public:
      /**
      * @brief Constructor.
      * @param[in] device (sulphur::graphics::D3D12Device&) Wrapper around the D3D12 device object
      */
      D3D12PersistentDescriptorHeap(D3D12Device& device);

      /**
      * @brief Destructor.
      */
      ~D3D12PersistentDescriptorHeap();

      /**
      * @brief Releases all D3D12 objects.
      */
      void OnDestroy();

      /**
      * @brief Initializes the persistent descriptor heap. Creates D3D12 descriptor heap objects.
      * @param[in] srv_heap_size (uint32_t) The size of a single SRV heap page in the persistent descriptor heap
      * @param[in] dsv_heap_size (uint32_t) The size of a single DSV heap page in the persistent descriptor heap
      * @param[in] rtv_heap_size (uint32_t) The size of a single RTV heap page in the persistent descriptor heap
      * @param[in] uav_heap_size (uint32_t) The size of a single UAV heap page in the persistent descriptor heap
      * @return (bool) Was this object initialized successfully?
      */
      bool Initialize(
        uint32_t srv_heap_size,
        uint32_t dsv_heap_size,
        uint32_t rtv_heap_size,
        uint32_t uav_heap_size
      );

      /**
      * @brief Allocates a slot for a single SRV descriptor.
      * @param[out] out_heap_index (uint32_t&) Persistent SRV index. Used to look up the descriptor.
      * @return (bool) Has the descriptor been reserved successfully?
      */
      bool AllocateSRVDescriptor(uint32_t& out_heap_index);

      /**
      * @brief Allocates a slot for a single DSV descriptor.
      * @param[out] out_heap_index (uint32_t&) Persistent DSV index. Used to look up the descriptor.
      * @return (bool) Has the descriptor been reserved successfully?
      */
      bool AllocateDSVDescriptor(uint32_t& out_heap_index);

      /**
      * @brief Allocates a slot for a single RTV descriptor.
      * @param[out] out_heap_index (uint32_t&) Persistent RTV index. Used to look up the descriptor.
      * @return (bool) Has the descriptor been reserved successfully?
      */
      bool AllocateRTVDescriptor(uint32_t& out_heap_index);

      /**
      * @brief Allocates a slot for a single UAV descriptor.
      * @param[out] out_heap_index (uint32_t&) Persistent UAV index. Used to look up the descriptor.
      * @return (bool) Has the descriptor been reserved successfully?
      */
      bool AllocateUAVDescriptor(uint32_t& out_heap_index);

      /**
      * @brief Frees the SRV slot in the persistent descriptor heap at the specified index.
      * @param[in] heap_index (uint32_t) The index of the descriptor to free.
      * @return (bool) Was the slot freed successfully
      */
      bool FreeSRVDescriptor(uint32_t heap_index);

      /**
      * @brief Frees the DSV slot in the persistent descriptor heap at the specified index.
      * @param[in] heap_index (uint32_t) The index of the descriptor to free.
      * @return (bool) Was the slot freed successfully
      */
      bool FreeDSVDescriptor(uint32_t heap_index);

      /**
      * @brief Frees the RTV slot in the persistent descriptor heap at the specified index.
      * @param[in] heap_index (uint32_t) The index of the descriptor to free.
      * @return (bool) Was the slot freed successfully
      */
      bool FreeRTVDescriptor(uint32_t heap_index);

      /**
      * @brief Frees the UAV slot in the persistent descriptor heap at the specified index.
      * @param[in] heap_index (uint32_t) The index of the descriptor to free.
      * @return (bool) Was the slot freed successfully
      */
      bool FreeUAVDescriptor(uint32_t heap_index);

      /**
      * @brief Gets a CPU descriptor handle for a SRV descriptor at the specified index.
      * @param[in] heap_index (uint32_t) The index of the descriptor
      * @param[out] out_cpu_handle (D3D12_CPU_DESCRIPTOR_HANDLE&) The CPU handle to the specified descriptor
      * @return (bool) Did the requested index have an associated descriptor?
      */
      bool GetCPUHandleForSRVDescriptor(uint32_t heap_index, D3D12_CPU_DESCRIPTOR_HANDLE& out_cpu_handle);

      /**
      * @brief Gets a CPU descriptor handle for a DSV descriptor at the specified index.
      * @param[in] heap_index (uint32_t) The index of the descriptor
      * @param[out] out_cpu_handle (D3D12_CPU_DESCRIPTOR_HANDLE&) The CPU handle to the specified descriptor
      * @return (bool) Did the requested index have an associated descriptor?
      */
      bool GetCPUHandleForDSVDescriptor(uint32_t heap_index, D3D12_CPU_DESCRIPTOR_HANDLE& out_cpu_handle);

      /**
      * @brief Gets a CPU descriptor handle for a RTV descriptor at the specified index.
      * @param[in] heap_index (uint32_t) The index of the descriptor
      * @param[out] out_cpu_handle (D3D12_CPU_DESCRIPTOR_HANDLE&) The CPU handle to the specified descriptor
      * @return (bool) Did the requested index have an associated descriptor?
      */
      bool GetCPUHandleForRTVDescriptor(uint32_t heap_index, D3D12_CPU_DESCRIPTOR_HANDLE& out_cpu_handle);

      /**
      * @brief Gets a CPU descriptor handle for a UAV descriptor at the specified index.
      * @param[in] heap_index (uint32_t) The index of the descriptor
      * @param[out] out_cpu_handle (D3D12_CPU_DESCRIPTOR_HANDLE&) The CPU handle to the specified descriptor
      * @return (bool) Did the requested index have an associated descriptor?
      */
      bool GetCPUHandleForUAVDescriptor(uint32_t heap_index, D3D12_CPU_DESCRIPTOR_HANDLE& out_cpu_handle);


      /**
      * @brief Gets a GPU descriptor handle for a SRV descriptor at the specified index.
      * @param[in] heap_index (uint32_t) The index of the descriptor
      * @param[out] out_gpu_handle (D3D12_GPU_DESCRIPTOR_HANDLE&) The GPU handle to the specified descriptor
      * @return (bool) Did the requested index have an associated descriptor?
      */
      bool GetGPUHandleForSRVDescriptor(uint32_t heap_index, D3D12_GPU_DESCRIPTOR_HANDLE& out_gpu_handle);

      /**
      * @brief Gets a GPU descriptor handle for a DSV descriptor at the specified index.
      * @param[in] heap_index (uint32_t) The index of the descriptor
      * @param[out] out_gpu_handle (D3D12_GPU_DESCRIPTOR_HANDLE&) The GPU handle to the specified descriptor
      * @return (bool) Did the requested index have an associated descriptor?
      */
      bool GetGPUHandleForDSVDescriptor(uint32_t heap_index, D3D12_GPU_DESCRIPTOR_HANDLE& out_gpu_handle);

      /**
      * @brief Gets a GPU descriptor handle for a RTV descriptor at the specified index.
      * @param[in] heap_index (uint32_t) The index of the descriptor
      * @param[out] out_gpu_handle (D3D12_GPU_DESCRIPTOR_HANDLE&) The GPU handle to the specified descriptor
      * @return (bool) Did the requested index have an associated descriptor?
      */
      bool GetGPUHandleForRTVDescriptor(uint32_t heap_index, D3D12_GPU_DESCRIPTOR_HANDLE& out_gpu_handle);

      /**
      * @brief Gets a GPU descriptor handle for a UAV descriptor at the specified index.
      * @param[in] heap_index (uint32_t) The index of the descriptor
      * @param[out] out_gpu_handle (D3D12_GPU_DESCRIPTOR_HANDLE&) The GPU handle to the specified descriptor
      * @return (bool) Did the requested index have an associated descriptor?
      */
      bool GetGPUHandleForUAVDescriptor(uint32_t heap_index, D3D12_GPU_DESCRIPTOR_HANDLE& out_gpu_handle);

    protected:

      /**
      * @class sulphur::graphics::D3D12PersistentDescriptorHeap::D3D12DescriptorPage
      * @brief A descriptor heap page. Wrapper for a DirectX 12 descriptor heap.
      * @author Yana Mateeva
      */
      class D3D12DescriptorPage
      {
      public:

        /**
        * @brief Constructor. Creates a d3d12 descriptor heap with the specified type and size.
        * @param[in] device (sulphur::graphics::D3D12Device&) The device, used to create the descriptor heap
        * @param[in] type (D3D12_DESCRIPTOR_HEAP_TYPE) The type of the descriptor heap to create
        * @param[in] heap_size (uint32_t) The size of the heap in descriptors
        */
        D3D12DescriptorPage(
          D3D12Device& device,
          D3D12_DESCRIPTOR_HEAP_TYPE type,
          uint32_t heap_size
        );

        /**
        * @brief Destructor. Releases the d3d12 descriptor heap object
        */
        ~D3D12DescriptorPage();

        /**
        * @brief Reserves the first free slot in the descriptor heap.
        * @param[out] out_descriptor_index (uint32_t&) The index of the reserved descriptor
        * @return (bool) Was there a free descriptor slot?
        */
        bool AllocateDescriptor(uint32_t& out_descriptor_index);

        /**
        * @brief Frees the descriptor slot at the specified index inside the descriptor heap.
        * @param[in] descriptor_index (uint32_t) The index of the descriptor to free
        * @return (bool) Was the specified index freed successfully?
        */
        bool FreeDescriptor(uint32_t descriptor_index);

        /**
        * @return (bool) Is the descriptor heap empty?
        */
        bool IsEmpty();

        /**
        * @return (bool) Is the descriptor heap full?
        */
        bool IsFull();

        /**
        * @brief Gets a CPU descriptor handle for the descriptor at the specified index in the descriptor heap.
        * @param[in] descriptor_index (uint32_t) The index of the descriptor
        * @return (D3D12_CPU_DESCRIPTOR_HANDLE) The CPU descriptor handle for the specified descriptor
        */
        D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t descriptor_index);

        /**
        * @brief Gets a GPU descriptor handle for the descriptor at the specified index in the descriptor heap.
        * @param[in] descriptor_index (uint32_t) The index of the descriptor
        * @return (D3D12_GPU_DESCRIPTOR_HANDLE) The CPU descriptor handle for the specified descriptor
        */
        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t descriptor_index);

      private:
        ID3D12DescriptorHeap* descriptor_heap_; //!< The D3D12 descriptor heap object
        uint32_t descriptor_increment_size_; //!< The descriptor increment size for the type of this heap
        uint32_t descriptor_heap_size_; //!< The size of the D3D12 descriptor heap (in descriptors)
        uint32_t free_descriptor_count_; //!< The amount of free descriptors in this descriptor heap
        foundation::Vector<bool> flag_array_; //!< Specifies if the descriptor at an index in the descriptor heap is free or taken
      };

    private:
      /**
      * @brief Adds an SRV descriptor heap page in the persistent descriptor heap.
      */
      void AddSRVPage();

      /**
      * @brief Adds an DSV descriptor heap page in the persistent descriptor heap.
      */
      void AddDSVPage();

      /**
      * @brief Adds an RTV descriptor heap page in the persistent descriptor heap.
      */
      void AddRTVPage();

      /**
      * @brief Adds an UAV descriptor heap page in the persistent descriptor heap.
      */
      void AddUAVPage();

      foundation::Vector<D3D12DescriptorPage*> srv_descriptor_pages_; //!< Vector with SRV descriptor pages / heaps
      foundation::Vector<D3D12DescriptorPage*> dsv_descriptor_pages_; //!< Vector with DSV descriptor pages / heaps
      foundation::Vector<D3D12DescriptorPage*> rtv_descriptor_pages_; //!< Vector with RTV descriptor pages / heaps
      foundation::Vector<D3D12DescriptorPage*> uav_descriptor_pages_; //!< Vector with UAV descriptor pages / heaps

      uint32_t srv_heap_size_; //!< The size in descriptors of each SRV heap
      uint32_t dsv_heap_size_; //!< The size in descriptors of each DSV heap
      uint32_t rtv_heap_size_; //!< The size in descriptors of each RTV heap
      uint32_t uav_heap_size_; //!< The size in descriptors of each UAV heap

      D3D12Device& device_; //!< Reference to the sulphur::graphics::D3D12Device. (Wrapper for ID3D12Device)
    };
  }
}