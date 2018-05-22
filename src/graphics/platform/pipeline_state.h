#pragma once

#include <foundation/utils/type_definitions.h>

#include <glm/glm.hpp>

namespace sulphur 
{
  namespace graphics 
  {
    enum struct TopologyType
    {
      kPoint,
      kLine,
      kTriangle
    };

    enum struct ComputeQueueType
    {
      kPostProcessing,
      kGpGpu,
    };

    enum struct FillMode
    {
      kWireFrame,
      kSolid
    };

    enum struct CullMode
    {
      kNone,
      kFront,
      kBack
    };

    struct RasterizerState
    {
      FillMode fill_mode = FillMode::kSolid;
      CullMode cull_mode = CullMode::kBack;
      bool clockwise = false;
      int depth_bias = 0;
      float depth_bias_clamp = 0.0f;
      float sloped_scaled_depth_bias = 0.0f;
      bool depth_clip_enable = true;
      bool msaa = false;
      bool antialiasing_line = false;
      uint force_sample_count = 0;
    };

    enum struct BlendFunc
    {
      kZero,
      kOne,
      kSrcColor,
      kInvSrcColor,
      kSrcAlpha,
      kInvSrcAlpha,
      kDestAlpha,
      kInvDestAlpha,
      kDestColor,
      kInvDestColor,
      kSrcAlphaSat,
      kBlendFactor,
      kInvBlendFactor,
      kSrc1Color,
      kInvSrc1Color,
      kSrc1Alpha,
      kInvSrc1Alpha
    };

    enum struct BlendOp
    {
      kAdd,
      kSubtract,
      kRevSubtract,
      kMin,
      kMax
    };

    enum struct LogicOp
    {
      kClear,
      kSet,
      kCopy,
      kCopyInverted,
      kNoop,
      kInvert,
      kAdd,
      kNand,
      kOr,
      kNor,
      kXor,
      kEquiv,
      kAndReverse,
      kAndInverted,
      kOrReverse,
      kOrInverted
    };

    enum struct ColorWriteEnable : byte
    {
      kEnableRed = 1,
      kEnableGreen = 2,
      kEnableBlue = 4,
      kEnableAlpha = 8,
      kEnableAll = (((kEnableRed | kEnableGreen) | kEnableBlue) | kEnableAlpha)
    };

    struct RenderTargetBlendState
    {
      bool blend_enable = false;
      bool logic_op_enable = false;
      BlendFunc src_blend_function = BlendFunc::kOne;
      BlendFunc dst_blend_function = BlendFunc::kZero;
      BlendOp blend_op = BlendOp::kAdd;
      BlendFunc src_blend_alpha = BlendFunc::kOne;
      BlendFunc dst_blend_alpha = BlendFunc::kZero;
      BlendOp blend_op_alpha = BlendOp::kAdd;
      LogicOp logic_op = LogicOp::kNoop;
      ColorWriteEnable color_write_mask = ColorWriteEnable::kEnableAll;
    };

    struct BlendState
    {
      RenderTargetBlendState render_target_blend_states[8];
      bool independant_blend_enable = false;
    };

    enum struct ComparisonFunc
    {
      kNever,
      kLess,
      kEqual,
      kLessEqual,
      kGreater,
      kNotEqual,
      kGreaterEqual,
      kAlways
    };

    enum struct StencilOp
    {
      kKeep,
      kZero,
      kReplace,
      kIncrementSat,
      kDecrementSat,
      kInvert,
      kIncrement,
      kDecrement
    };

    struct StencilOpDesc
    {
      StencilOpDesc(StencilOp a_fail, StencilOp a_depth_fail, StencilOp a_pass, ComparisonFunc a_func) :
        fail(a_fail),
        depth_fail(a_depth_fail),
        pass(a_pass),
        func(a_func)
      {}

      StencilOp fail = StencilOp::kKeep;
      StencilOp depth_fail = StencilOp::kKeep;
      StencilOp pass = StencilOp::kKeep;
      ComparisonFunc func = ComparisonFunc::kAlways;
    };

    struct DepthStencilState
    {
      bool depth_enable = true;
      bool stencil_enable = true;
      byte stencil_read_mask = 0xff;
      byte stencil_write_mask = 0xff;
      bool depth_write = true;
      ComparisonFunc depth_func = ComparisonFunc::kLess;
      StencilOpDesc stencil_front_face = { StencilOp::kKeep, StencilOp::kKeep, StencilOp::kReplace, ComparisonFunc::kGreaterEqual };
      StencilOpDesc stencil_back_face = { StencilOp::kKeep, StencilOp::kKeep, StencilOp::kKeep, ComparisonFunc::kNever };
    };

    struct PipelineState
    {
      RasterizerState rasterizer_state;
      BlendState blend_state;
      DepthStencilState depth_stencil_state;
      TopologyType topology_type = TopologyType::kTriangle;
      glm::vec4 scissor_rect;
    };
  }
}