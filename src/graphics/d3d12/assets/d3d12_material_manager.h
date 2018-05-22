#pragma once

#include <foundation/containers/vector.h>
#include <foundation/containers/map.h>

#include <cstdint>
#include <d3d12.h>

namespace sulphur
{
  namespace engine
  {
    class MaterialPass;
  }
  namespace graphics
  {
    class D3D12Device;
    class D3D12FrameDescriptorHeap;
    class D3D12ConstantBufferHeap;

    /**
    * @class sulphur::graphics::D3D12Material
    * @brief GPU representation of a material.
    * @remark Needs a lot of work to make this more modular (several descriptor tables of the same type, more than one constant buffer, etc).
    * @author Yana Mateeva
    */
    class D3D12Material
    {
    public:
      /**
      * @brief Constructor.
      * @param[in] device (sulphur::graphics::D3D12Device&) The DirectX 12 device object.
      * @param[in] material (sulphur::engine::MaterialPass&) The engine-side representation of this material.
      */
      D3D12Material(D3D12Device& device, const engine::MaterialPass& material);

      /**
      * @brief Resets the state of this object. Material state should be reset at the beginning of each frame.
      */
      void Reset();

      /**
      * @brief Copies this material's texture descriptors to the frame descriptor heap.
      * @param[in] frame_desc_heap (sulphur::graphics::D3D12FrameDescriptorHeap&) The frame descriptor heap to use.
      */
      void CopyDescriptorsToFrameDescriptorHeap(D3D12FrameDescriptorHeap& frame_desc_heap);

      /**
      * @brief Copies over constant buffer data from this material to the constant buffer heap.
      * @param[in] constant_buffer_heap (sulphur::graphics::D3D12ConstantBufferHeap&) The constant buffer heap to copy to.
      * @remarks Needs implementation.
      */
      void CopyConstantBufferData(D3D12ConstantBufferHeap& constant_buffer_heap);

      /**
      * @return (D3D12_GPU_DESCRIPTOR_HANDLE) A GPU handle to the beginning of the SRV descriptor table in the frame descriptor heap.
      */
      D3D12_GPU_DESCRIPTOR_HANDLE srv_descriptor_table_handle() const { return srv_descriptor_table_handle_; }

      /**
      * @return (D3D12_GPU_DESCRIPTOR_HANDLE) A GPU handle to the beginning of the UAV descriptor table in the frame descriptor heap.
      */
      D3D12_GPU_DESCRIPTOR_HANDLE uav_descriptor_table_handle() const { return uav_descriptor_table_handle_; }

      /**
      * @return (D3D12_GPU_VIRTUAL_ADDRESS) A GPU handle to the constant buffer.
      */
      D3D12_GPU_VIRTUAL_ADDRESS constant_buffer_handle() const { return constant_buffer_handle_; }

    private:
      bool was_used_this_frame_; //!< Was this material already used this frame?

      // TODO:
      // Needs some kind of uniform buffer structure right about here.
      // This would be copied over to the constant buffer heap every frame.

      foundation::Vector<uint32_t> persistent_srv_handles_; //!< Collection of persistent SRV handles for this material.
      foundation::Vector<uint32_t> persistent_uav_handles_; //!< Collection of persistent UAV handles for this material.

      D3D12_GPU_DESCRIPTOR_HANDLE srv_descriptor_table_handle_; //!< A GPU handle to the beginning of the SRV descriptor table in the frame descriptor heap.
      D3D12_GPU_DESCRIPTOR_HANDLE uav_descriptor_table_handle_; //!< A GPU handle to the beginning of the UAV descriptor table in the frame descriptor heap.
      D3D12_GPU_VIRTUAL_ADDRESS constant_buffer_handle_; //!< A GPU handle to the constant buffer.
    };

    /**
    * @class sulphur::graphics::D3D12MaterialManager
    * @brief Collection of GPU representations of materials.
    * @author Yana Mateeva
    */
    class D3D12MaterialManager
    {
    public:
      /**
      * @brief Constructor.
      * @param[in] device (sulphur::graphics::D3D12Device&) The DirectX 12 device object.
      */
      D3D12MaterialManager(D3D12Device& device);

      /**
      * @brief Resets the state of all materials.
      * @see sulphur::graphics::D3D12Material::Reset
      */
      void StartFrame();

      /**
      * @brief Releases all D3D12 resources.
      */
      void OnDestroy();

      /**
      * @brief Sets the currently used material.
      */
      void SetMaterial(const engine::MaterialPass& material);

      /**
      * @return (D3D12Material*) The currently set material.
      */
      D3D12Material* current_material() { return current_material_; }

    private:
      D3D12Device& device_; //!< The DirectX 12 device object.
      D3D12Material* current_material_; //!< The currently set material.
      foundation::Map<size_t, D3D12Material*> materials_; //!< Collection of all generated GPU materials.
    };
  }
}