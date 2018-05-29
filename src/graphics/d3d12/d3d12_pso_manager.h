#pragma once

#include "graphics/platform/pipeline_state.h"

#include <foundation/containers/map.h>
#include <engine/assets/shader.h>
#include <engine/assets/compute_shader.h>

struct ID3D12PipelineState;
struct ID3D12RootSignature;
struct D3D12_SHADER_BYTECODE;

namespace sulphur
{
  namespace graphics
  {
    class D3D12Device;

    /**
    * @class sulphur::graphics::PsoManager
    * @brief Generates and keeps track of pipeline states.
    * @author Jelle de Haan
    */
    class PsoManager
    {
    public:
      /**
      * @brief Constructor.
      * @param[in] device (sulphur::graphics::D3D12Device&) The DirectX 12 device object.
      */
      PsoManager(D3D12Device& device);

      /**
      * @brief Destructor.
      */
      ~PsoManager();

      /**
      * @brief Releases all D3D12 objects.
      */
      void OnDestroy();

      /**
      * @return (ID3D12PipelineState*) The current pipeline state
      */
      ID3D12PipelineState* pipeline_state_object() const
      {
        return current_pipeline_state_;
      }

      // Use of root signature and default shaders in this function is temporary
      /**
      * @brief Sets the current pipeline state. Generates D3D12 resources if necessary.
      * @param[in] pipeline_state (const sulphur::graphics::PipelineState&) A pipeline state description.
      * @param[in] root_signature (ID3D12RootSignature*) A root signature, matching the used shaders.
      * @param[in] default_vs (const D3D12_SHADER_BYTECODE&) Vertex shader bytecode.
      * @param[in] default_ps (const D3D12_SHADER_BYTECODE&) Pixel shader bytecode.
      */
      void SetPipelineState(const PipelineState& pipeline_state, ID3D12RootSignature* root_signature, 
        const D3D12_SHADER_BYTECODE& default_vs, const D3D12_SHADER_BYTECODE& default_ps);

      /**
      * @brief Sets the current pipeline state. Generates D3D12 resources if necessary.
      * @param[in] pipeline_state (const sulphur::graphics::PipelineState&) A pipeline state description.
      * @param[in] shader (const sulphur::engine::ShaderHandle&) A shader to use with this pipeline state.
      */
      void SetPipelineState(const PipelineState& pipeline_state, const engine::ShaderHandle& shader);

      /**
      * @brief Sets the current pipeline state. This will be a compute pipeline state. Generates D3D12 resources if necessary.
      * @param[in] compute_shader (const sulphur::engine::ComputeShaderHandle&) The compute shader to use for this pipeline state.
      */
      void SetPipelineState(const engine::ComputeShaderHandle& compute_shader);

    private:
      ID3D12PipelineState* current_pipeline_state_; //!< The currently set pipeline state.
      foundation::Map<size_t, ID3D12PipelineState*> pipeline_states_; //!< Collection of all generated pipeline state objects.

      D3D12Device& device_; //!< The DirectX 12 device object.
    };
  }
}