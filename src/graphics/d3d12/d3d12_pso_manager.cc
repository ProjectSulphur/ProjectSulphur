#include "d3d12_pso_manager.h"

#include "graphics/d3d12/d3d12_device_wrapper.h"
#include "graphics/d3d12/d3dx12.h"
#include "graphics/d3d12/utils/d3d12_utility.h"
#include "graphics/d3d12/assets/d3d12_asset.h"
#include "graphics/d3d12/d3d12_root_signature.h"

#include <engine/assets/shader.h>
#include <d3d12.h>

namespace sulphur
{
  namespace graphics
  {
    //------------------------------------------------------------------------------------------------------
    /**
    * @brief Converts a sulphur::graphics::ComparisonFunc to a D3D12_COMPARISON_FUNC.
    * @param[in] cf (sulphur::graphics::ComparisonFunc) The comparison func to convert.
    * @return (D3D12_COMPARISON_FUNC) The converted result.
    */
    D3D12_COMPARISON_FUNC ConvertComparisonFunc(ComparisonFunc cf)
    {
      switch (cf)
      {
      case ComparisonFunc::kAlways:
        return D3D12_COMPARISON_FUNC_ALWAYS;
      case ComparisonFunc::kEqual:
        return D3D12_COMPARISON_FUNC_EQUAL;
      case ComparisonFunc::kGreater:
        return D3D12_COMPARISON_FUNC_GREATER;
      case ComparisonFunc::kGreaterEqual:
        return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
      case ComparisonFunc::kLess:
        return D3D12_COMPARISON_FUNC_LESS;
      case ComparisonFunc::kLessEqual:
        return D3D12_COMPARISON_FUNC_LESS_EQUAL;
      case ComparisonFunc::kNever:
        return D3D12_COMPARISON_FUNC_NEVER;
      case ComparisonFunc::kNotEqual:
        return D3D12_COMPARISON_FUNC_NOT_EQUAL;
      default:
        return D3D12_COMPARISON_FUNC_ALWAYS;
      }
    }

    //------------------------------------------------------------------------------------------------------
    /**
    * @brief Converts a sulphur::graphics::StencilOp to a D3D12_STENCIL_OP.
    * @param[in] so (sulphur::graphics::StencilOp) The stencil operation to convert.
    * @return (D3D12_STENCIL_OP) The converted result.
    */
    D3D12_STENCIL_OP ConvertStencilOp(StencilOp so)
    {
      switch (so)
      {
      case StencilOp::kDecrement:
        return D3D12_STENCIL_OP_DECR;
      case StencilOp::kDecrementSat:
        return D3D12_STENCIL_OP_DECR_SAT;
      case StencilOp::kIncrement:
        return D3D12_STENCIL_OP_INCR;
      case StencilOp::kIncrementSat:
        return D3D12_STENCIL_OP_INCR_SAT;
      case StencilOp::kInvert:
        return D3D12_STENCIL_OP_INVERT;
      case StencilOp::kKeep:
        return D3D12_STENCIL_OP_KEEP;
      case StencilOp::kReplace:
        return D3D12_STENCIL_OP_REPLACE;
      case StencilOp::kZero:
        return D3D12_STENCIL_OP_ZERO;
      default:
        return D3D12_STENCIL_OP_KEEP;
      }
    }

    //------------------------------------------------------------------------------------------------------
    /**
    * @brief Converts a sulphur::graphics::TopologyType to a D3D12_PRIMITIVE_TOPOLOGY_TYPE.
    * @param[in] to (sulphur::graphics::TopologyType) The topology type to convert.
    * @return (D3D12_PRIMITIVE_TOPOLOGY_TYPE) The converted result.
    */
    D3D12_PRIMITIVE_TOPOLOGY_TYPE ConvertPrimitiveTopology(TopologyType to)
    {
      switch (to)
      {
      case TopologyType::kLine:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
      case TopologyType::kPoint:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
      case TopologyType::kTriangle:
      default:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
      }
    }

    using pso_pair = eastl::pair<const size_t, ID3D12PipelineState*>; //!< Typedef for a pair of pso ID and PipelineState pointer
    using pso_iterator = eastl::rbtree_iterator<pso_pair, pso_pair*, pso_pair&>; //!< Typedef for iterator over a pair of pso ID and PipelineState pointer

    /**
    * @brief Converts a combination of a pipeline state description, root signature and vertex/pixel shader bytecode into a D3D12_GRAPHICS_PIPELINE_STATE_DESC.
    * @param[in] pipeline_state (const sulphur::graphics::PipelineState&) The pipeline state description.
    * @param[in] root_signature (ID3D12RootSignature*) A root signature object, matching the vertex/pixel shaders.
    * @param[in] default_vs (const D3D12_SHADER_BYTECODE&) Vertex shader bytecode.
    * @param[in] default_ps (const D3D12_SHADER_BYTECODE&) Pixel shader bytecode.
    * @return (D3D12_GRAPHICS_PIPELINE_STATE_DESC) A D3D12 pipeline state description structure. Used to create a pipeline state object.
    */
    D3D12_GRAPHICS_PIPELINE_STATE_DESC ConvertPipelineState(
      const PipelineState& pipeline_state, 
      ID3D12RootSignature* root_signature,
      const D3D12_SHADER_BYTECODE& default_vs,
      const D3D12_SHADER_BYTECODE& default_ps);

    /**
    * @brief Converts a blend state description to a D3D12_BLEND_DESC struct.
    * @param[in] blend_state (const sulphur::graphics::BlendState&) The blend state description to convert.
    * @return (D3D12_BLEND_DESC) The converted blend state description.
    */
    D3D12_BLEND_DESC ConvertBlendState(const BlendState& blend_state);

    /**
    * @brief Converts a rasterizer description to a D3D12_RASTERIZER_DESC struct.
    * @param[in] rasterizer_state (const sulphur::graphics::RasterizerState&) The rasterizer description to convert.
    * @return (D3D12_RASTERIZER_DESC) The converted rasterizer description.
    */
    D3D12_RASTERIZER_DESC ConvertRasterizerState(const RasterizerState& rasterizer_state);

    const static D3D12_INPUT_ELEMENT_DESC element_descs[] =
    {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
      { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
      { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
      { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
      { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
    }; //!< Temporary input layout description.

    //------------------------------------------------------------------------------------------------------
    PsoManager::PsoManager(D3D12Device& device):
      device_(device),
      pipeline_states_(nullptr),
      current_pipeline_state_(nullptr)
    {
    }

    //------------------------------------------------------------------------------------------------------
    PsoManager::~PsoManager()
    {
    }

    //------------------------------------------------------------------------------------------------------
    void PsoManager::OnDestroy()
    {
      for (pso_pair& key : pipeline_states_)
      {
        key.second->Release();
      }
    }

    //------------------------------------------------------------------------------------------------------
    void PsoManager::SetPipelineState(
      const PipelineState& pipeline_state,
      ID3D12RootSignature* root_signature, 
      const D3D12_SHADER_BYTECODE& default_vs,
      const D3D12_SHADER_BYTECODE& default_ps)
    {
      D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = ConvertPipelineState(
        pipeline_state,
        root_signature,
        default_vs,
        default_ps
      );
      
      size_t hash_code = graphics::HashState(&desc);

      pso_iterator it = pipeline_states_.find(hash_code);
      if (it != pipeline_states_.end())
      {
        assert(it->second);
        current_pipeline_state_ = it->second;
        return;
      }
      
      ID3D12PipelineState*& pso = pipeline_states_[hash_code];

      if (!device_.CreateGraphicsPipelineState(pso, desc))
      {
        // log error
        assert(false && "Could not create pipeline state.");
      }
      current_pipeline_state_ = pso;
    }

    //------------------------------------------------------------------------------------------------------
    void PsoManager::SetPipelineState(
      const PipelineState& pipeline_state,
      const engine::ShaderHandle& shader)
    {
      engine::ShaderProgram* vertex_shader = shader->vertex_shader().GetRaw();
      engine::ShaderProgram* pixel_shader = shader->pixel_shader().GetRaw();
      CD3DX12_SHADER_BYTECODE vert(
        vertex_shader->byte_code_data(),
        vertex_shader->byte_code_size()
      );

      CD3DX12_SHADER_BYTECODE pix(
        pixel_shader->byte_code_data(),
        pixel_shader->byte_code_size()
      );

      D3D12RootSignature* root_sig = 
        device_.shader_asset_manager().GetRootSignatureForShader(shader.GetGPUHandle());
      
      SetPipelineState(pipeline_state, root_sig->root_signature(), vert, pix);
    }

    //------------------------------------------------------------------------------------------------------
    D3D12_GRAPHICS_PIPELINE_STATE_DESC ConvertPipelineState(
      const PipelineState& pipeline_state, 
      ID3D12RootSignature* root_signature,
      const D3D12_SHADER_BYTECODE& default_vs,
      const D3D12_SHADER_BYTECODE& default_ps)
    {
      D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
      desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

      // TODO: Automate render target count
      desc.NumRenderTargets = 1;
      desc.BlendState = ConvertBlendState(pipeline_state.blend_state);
      
      for (size_t i = 0; i < desc.NumRenderTargets; ++i)
      {
        desc.RTVFormats[i] = DXGI_FORMAT_R8G8B8A8_UNORM;
      }

      for (size_t i = desc.NumRenderTargets; i < 8; ++i)
      {
        desc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
      }

      desc.RasterizerState = ConvertRasterizerState(pipeline_state.rasterizer_state);
      
      //----------------------------Set topology type--------------------------------
      desc.PrimitiveTopologyType = ConvertPrimitiveTopology(pipeline_state.topology_type);

      // TODO: Automate shader assignment
      desc.pRootSignature = root_signature;
      desc.VS = default_vs;
      desc.PS = default_ps;

      //-------------------------Create input layout-----------------------------
      // TODO: Create automated input layout (For now just have a default one)
      D3D12_INPUT_LAYOUT_DESC input_layout_desc = {};
      input_layout_desc.NumElements = _countof(element_descs);
      input_layout_desc.pInputElementDescs = element_descs;

      desc.InputLayout = input_layout_desc;

      // TODO: Automate sampler count
      desc.SampleDesc.Count = 1;
      desc.SampleMask = UINT_MAX;

      D3D12_DEPTH_STENCIL_DESC ds_desc = {};
      ds_desc.DepthEnable = pipeline_state.depth_stencil_state.depth_enable;
      ds_desc.DepthFunc = ConvertComparisonFunc(pipeline_state.depth_stencil_state.depth_func);
      ds_desc.DepthWriteMask = 
        pipeline_state.depth_stencil_state.depth_write ?
        D3D12_DEPTH_WRITE_MASK_ALL : 
        D3D12_DEPTH_WRITE_MASK_ZERO;

      ds_desc.StencilEnable = pipeline_state.depth_stencil_state.stencil_enable;
      ds_desc.StencilReadMask = pipeline_state.depth_stencil_state.stencil_read_mask;
      ds_desc.StencilWriteMask = pipeline_state.depth_stencil_state.stencil_write_mask;

      ds_desc.FrontFace.StencilDepthFailOp = 
        ConvertStencilOp(pipeline_state.depth_stencil_state.stencil_front_face.depth_fail);
      ds_desc.FrontFace.StencilFailOp = 
        ConvertStencilOp(pipeline_state.depth_stencil_state.stencil_front_face.fail);
      ds_desc.FrontFace.StencilPassOp = 
        ConvertStencilOp(pipeline_state.depth_stencil_state.stencil_front_face.pass);
      ds_desc.FrontFace.StencilFunc = 
        ConvertComparisonFunc(pipeline_state.depth_stencil_state.stencil_front_face.func);

      ds_desc.BackFace.StencilDepthFailOp = 
        ConvertStencilOp(pipeline_state.depth_stencil_state.stencil_back_face.depth_fail);
      ds_desc.BackFace.StencilFailOp = 
        ConvertStencilOp(pipeline_state.depth_stencil_state.stencil_back_face.fail);
      ds_desc.BackFace.StencilPassOp = 
        ConvertStencilOp(pipeline_state.depth_stencil_state.stencil_back_face.pass);
      ds_desc.BackFace.StencilFunc = 
        ConvertComparisonFunc(pipeline_state.depth_stencil_state.stencil_back_face.func);

      desc.RasterizerState.DepthClipEnable = pipeline_state.rasterizer_state.depth_clip_enable;
      desc.DepthStencilState = ds_desc;

      desc.DSVFormat = 
        pipeline_state.depth_stencil_state.stencil_enable ?
        DXGI_FORMAT_D24_UNORM_S8_UINT :
        DXGI_FORMAT_D32_FLOAT;

      return desc;
    }
  
    //------------------------------------------------------------------------------------------------------
    D3D12_BLEND_DESC ConvertBlendState(const BlendState& blend_state)
    {
      D3D12_BLEND_DESC blend_desc = {};
      blend_desc.IndependentBlendEnable = blend_state.independant_blend_enable;
      for (size_t i = 0; i < 8; ++i)
      {
        const RenderTargetBlendState& rtbs = blend_state.render_target_blend_states[i];

        blend_desc.RenderTarget[i].BlendEnable = 
          rtbs.blend_enable;
        blend_desc.RenderTarget[i].BlendOp = 
          static_cast<D3D12_BLEND_OP>(rtbs.blend_op);
        blend_desc.RenderTarget[i].BlendOpAlpha = 
          static_cast<D3D12_BLEND_OP>(rtbs.blend_op_alpha);
        blend_desc.RenderTarget[i].DestBlend = 
          static_cast<D3D12_BLEND>(rtbs.dst_blend_function);
        blend_desc.RenderTarget[i].DestBlendAlpha = 
          static_cast<D3D12_BLEND>(rtbs.dst_blend_alpha);
        blend_desc.RenderTarget[i].LogicOp = 
          static_cast<D3D12_LOGIC_OP>(rtbs.logic_op);
        blend_desc.RenderTarget[i].LogicOpEnable = 
          rtbs.logic_op_enable;
        blend_desc.RenderTarget[i].RenderTargetWriteMask = 
          static_cast<UINT8>(rtbs.color_write_mask);
        blend_desc.RenderTarget[i].SrcBlend = 
          static_cast<D3D12_BLEND>(rtbs.src_blend_function);
        blend_desc.RenderTarget[i].SrcBlendAlpha = 
          static_cast<D3D12_BLEND>(rtbs.src_blend_alpha);
      }

      return blend_desc;
    }

    //------------------------------------------------------------------------------------------------------
    D3D12_RASTERIZER_DESC ConvertRasterizerState(const RasterizerState& rasterizer_state)
    {
      D3D12_RASTERIZER_DESC rasterizer_desc = {};
      rasterizer_desc.AntialiasedLineEnable = rasterizer_state.antialiasing_line;
      //rasterizer_desc.ConservativeRaster = pipeline_state.rasterizer_state.;

      switch (rasterizer_state.cull_mode)
      {
      case CullMode::kBack:
        rasterizer_desc.CullMode = D3D12_CULL_MODE_BACK;
        break;
      case CullMode::kFront:
        rasterizer_desc.CullMode = D3D12_CULL_MODE_FRONT;
        break;
      case CullMode::kNone:
        rasterizer_desc.CullMode = D3D12_CULL_MODE_NONE;
        break;
      }

      rasterizer_desc.DepthBias = rasterizer_state.depth_bias;
      rasterizer_desc.DepthBiasClamp = rasterizer_state.depth_bias_clamp;
      rasterizer_desc.DepthClipEnable = rasterizer_state.depth_clip_enable;

      switch (rasterizer_state.fill_mode)
      {
      case FillMode::kSolid:
        rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID;
        break;
      case FillMode::kWireFrame:
        rasterizer_desc.FillMode = D3D12_FILL_MODE_WIREFRAME;
        break;
      }

      rasterizer_desc.ForcedSampleCount = rasterizer_state.force_sample_count;
      rasterizer_desc.FrontCounterClockwise = rasterizer_state.clockwise;
      rasterizer_desc.MultisampleEnable = rasterizer_state.msaa;
      rasterizer_desc.SlopeScaledDepthBias = rasterizer_state.sloped_scaled_depth_bias;

      return rasterizer_desc;
    }
  }
}