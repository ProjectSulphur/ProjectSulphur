#pragma once
#include "d3dx12.h"

#include <engine/assets/shader.h>
#include <engine/assets/compute_shader.h>

#include <d3d12.h>

namespace sulphur //!< Sulphur namespace
{
  namespace graphics //!< Graphics namespace
  {
    class D3D12Device;

    /**
    * @class sulphur::graphics::D3D12RootSignature
    * @brief Wrapper class for the ID3D12RootSignature object.
    * @author Yana Mateeva
    */
    class D3D12RootSignature
    {
    public:
      /**
      * @brief Constructor.
      * @param[in] root_signature (ID3D12RootSignature*) The D3D12 root signature object.
      * @param[in] num_constant_buffers (uint32_t) The amount of constant buffers.
      * @param[in] num_descriptor_tables (uint32_t) The amount of descriptor tables.
      * @param[in] num_constants (uint32_t) The amount of constant variables.
      */
      D3D12RootSignature(
        ID3D12RootSignature* root_signature,
        uint32_t num_constant_buffers,
        uint32_t num_descriptor_tables,
        uint32_t num_constants) :
        root_signature_(root_signature),
        num_constant_buffers_(num_constant_buffers),
        num_descriptor_tables_(num_descriptor_tables),
        num_constants_(num_constants)
      {}

      /**
      * @brief Destructor.
      */
      ~D3D12RootSignature();

      /**
      * @brief Creates a root signature description based on the shader reflection.
      * @param[in] shader (const sulphur::engine::ShaderHandle&) The shader to create a root signature description for.
      * @param[in] device (sulphur::graphics::D3D12Device&) The D3D12 device object.
      * @return (sulphur::graphics::D3D12RootSignature*) The created root signature
      */
      static D3D12RootSignature* CreateRootSignatureFromShader(
        const engine::ShaderHandle& shader,
        D3D12Device& device);

      /**
      * @brief Creates a root signature description based on the compute shader reflection.
      * @param[in] compute_shader (const sulphur::engine::ComputeShaderHandle&) The compute shader to create a root signature description for.
      * @param[in] device (sulphur::graphics::D3D12Device&) The D3D12 device object.
      * @return (sulphur::graphics::D3D12RootSignature*) The created root signature
      */
      static D3D12RootSignature* CreateRootSignatureFromShader(
        const engine::ComputeShaderHandle& compute_shader,
        D3D12Device& device);

      /**
      * @return (ID3D12RootSignature*) The D3D12 root signature object.
      */
      ID3D12RootSignature* root_signature() const { return root_signature_; }

    private:
      uint32_t num_constant_buffers_; //!< The amount of constant buffers.
      uint32_t num_descriptor_tables_; //!< The amount of descriptor tables.
      uint32_t num_constants_; //!< The amount of constants.

      ID3D12RootSignature* root_signature_; //!< The D3D12 root signature object.

    };

  }
}