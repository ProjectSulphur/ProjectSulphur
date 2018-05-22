#pragma once

#include <foundation/utils/type_definitions.h>

#include <cstdint>
#include <d3d12.h>

namespace sulphur
{
  namespace graphics
  {
    class D3D12Device;

    /**
    * @class sulphur::graphics::D3D12ConstantBufferHeap
    * @brief A wrapper class for a heap, used to upload constant buffer data to the GPU
    * @author Yana Mateeva
    */
    class D3D12ConstantBufferHeap
    {
    public:
      /**
      * @brief Creates the constant buffer heap
      * @param[in] device (sulphur::graphics::D3D12Device&) The device, used to create D3D12 resources
      */
      void Create(D3D12Device& device);
      
      /**
      * @brief Destructor.
      */
      ~D3D12ConstantBufferHeap();

      /**
      * @brief Releases constant buffer d3d12 resources.
      */
      void OnDestroy();

      /**
      * @return (D3D12_GPU_VIRTUAL_ADDRESS) Returns the GPU virtual address to the start of the heap.
      */
      D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() { return heap_->GetGPUVirtualAddress(); }

      /**
      * @brief Copies constant buffer data to the heap
      * @param[in] data (void*) Pointer to the beginning of the data to be copied
      * @param[in] size (size_t) The size of the data to copy
      * @param[out] out_offset_in_heap (size_t&) The offset in the constant buffer heap
      * @todo For easier use, return a GPU handle instead of an offset.
      * @todo Currently breaks if the size of the data is not a multiple of 256 bytes. Make this safe.
      */
      void Write(void* data, size_t size, size_t& out_offset_in_heap);

    private:
      ID3D12Resource* heap_ = nullptr; //!< The GPU committed resource, upload heap
      static const size_t heap_size_ = 4194304; //!< The size of the constant buffer heap in bytes
      size_t ptr_ = 0; //!< The current pointer inside the heap, used to write new data

      uint8_t* data_begin_; //!< Points to the beginning of the mapped heap
    };
  }
}