#include "tools/builder/shared/spv_shader_compiler.h"
#include "tools/builder/shared/shader_compiler_includer.h"
#include "tools/builder/pipelines/shader_pipeline_options.h"
#include "tools/builder/shared/file_system.h"
#include <foundation/logging/logger.h>
#include <foundation/pipeline-assets/shader.h>
#include "../glslang/SPIRV/GlslangToSpv.h"
#include <Public/ShaderLang.h>

#define PS_SPIRV_VERSION 0x00001000
#define PS_VULKAN_VERSION 100

namespace sulphur 
{
  namespace builder 
  {
    /**
     * @brief Default limits for GPU resources.
     */
    const TBuiltInResource default_built_in_resources = {
      /* .MaxLights = */ 32,
      /* .MaxClipPlanes = */ 6,
      /* .MaxTextureUnits = */ 32,
      /* .MaxTextureCoords = */ 32,
      /* .MaxVertexAttribs = */ 64,
      /* .MaxVertexUniformComponents = */ 4096,
      /* .MaxVaryingFloats = */ 64,
      /* .MaxVertexTextureImageUnits = */ 32,
      /* .MaxCombinedTextureImageUnits = */ 80,
      /* .MaxTextureImageUnits = */ 32,
      /* .MaxFragmentUniformComponents = */ 4096,
      /* .MaxDrawBuffers = */ 32,
      /* .MaxVertexUniformVectors = */ 128,
      /* .MaxVaryingVectors = */ 8,
      /* .MaxFragmentUniformVectors = */ 16,
      /* .MaxVertexOutputVectors = */ 16,
      /* .MaxFragmentInputVectors = */ 15,
      /* .MinProgramTexelOffset = */ -8,
      /* .MaxProgramTexelOffset = */ 7,
      /* .MaxClipDistances = */ 8,
      /* .MaxComputeWorkGroupCountX = */ 65535,
      /* .MaxComputeWorkGroupCountY = */ 65535,
      /* .MaxComputeWorkGroupCountZ = */ 65535,
      /* .MaxComputeWorkGroupSizeX = */ 1024,
      /* .MaxComputeWorkGroupSizeY = */ 1024,
      /* .MaxComputeWorkGroupSizeZ = */ 64,
      /* .MaxComputeUniformComponents = */ 1024,
      /* .MaxComputeTextureImageUnits = */ 16,
      /* .MaxComputeImageUniforms = */ 8,
      /* .MaxComputeAtomicCounters = */ 8,
      /* .MaxComputeAtomicCounterBuffers = */ 1,
      /* .MaxVaryingComponents = */ 60,
      /* .MaxVertexOutputComponents = */ 64,
      /* .MaxGeometryInputComponents = */ 64,
      /* .MaxGeometryOutputComponents = */ 128,
      /* .MaxFragmentInputComponents = */ 128,
      /* .MaxImageUnits = */ 8,
      /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
      /* .MaxCombinedShaderOutputResources = */ 8,
      /* .MaxImageSamples = */ 0,
      /* .MaxVertexImageUniforms = */ 0,
      /* .MaxTessControlImageUniforms = */ 0,
      /* .MaxTessEvaluationImageUniforms = */ 0,
      /* .MaxGeometryImageUniforms = */ 0,
      /* .MaxFragmentImageUniforms = */ 8,
      /* .MaxCombinedImageUniforms = */ 8,
      /* .MaxGeometryTextureImageUnits = */ 16,
      /* .MaxGeometryOutputVertices = */ 256,
      /* .MaxGeometryTotalOutputComponents = */ 1024,
      /* .MaxGeometryUniformComponents = */ 1024,
      /* .MaxGeometryVaryingComponents = */ 64,
      /* .MaxTessControlInputComponents = */ 128,
      /* .MaxTessControlOutputComponents = */ 128,
      /* .MaxTessControlTextureImageUnits = */ 16,
      /* .MaxTessControlUniformComponents = */ 1024,
      /* .MaxTessControlTotalOutputComponents = */ 4096,
      /* .MaxTessEvaluationInputComponents = */ 128,
      /* .MaxTessEvaluationOutputComponents = */ 128,
      /* .MaxTessEvaluationTextureImageUnits = */ 16,
      /* .MaxTessEvaluationUniformComponents = */ 1024,
      /* .MaxTessPatchComponents = */ 120,
      /* .MaxPatchVertices = */ 32,
      /* .MaxTessGenLevel = */ 64,
      /* .MaxViewports = */ 16,
      /* .MaxVertexAtomicCounters = */ 0,
      /* .MaxTessControlAtomicCounters = */ 0,
      /* .MaxTessEvaluationAtomicCounters = */ 0,
      /* .MaxGeometryAtomicCounters = */ 0,
      /* .MaxFragmentAtomicCounters = */ 8,
      /* .MaxCombinedAtomicCounters = */ 8,
      /* .MaxAtomicCounterBindings = */ 1,
      /* .MaxVertexAtomicCounterBuffers = */ 0,
      /* .MaxTessControlAtomicCounterBuffers = */ 0,
      /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
      /* .MaxGeometryAtomicCounterBuffers = */ 0,
      /* .MaxFragmentAtomicCounterBuffers = */ 1,
      /* .MaxCombinedAtomicCounterBuffers = */ 1,
      /* .MaxAtomicCounterBufferSize = */ 16384,
      /* .MaxTransformFeedbackBuffers = */ 4,
      /* .MaxTransformFeedbackInterleavedComponents = */ 64,
      /* .MaxCullDistances = */ 8,
      /* .MaxCombinedClipAndCullDistances = */ 8,
      /* .MaxSamples = */ 4,
      /* .limits = */{
        /* .nonInductiveForLoops = */ 1,
        /* .whileLoops = */ 1,
        /* .doWhileLoops = */ 1,
        /* .generalUniformIndexing = */ 1,
        /* .generalAttributeMatrixVectorIndexing = */ 1,
        /* .generalVaryingIndexing = */ 1,
        /* .generalSamplerIndexing = */ 1,
        /* .generalVariableIndexing = */ 1,
        /* .generalConstantMatrixVectorIndexing = */ 1,
      } };

    //-----------------------------------------------------------------------------------------------
    SpvShaderCompiler::SpvShaderCompiler(const ShaderPipelineOptions& options ) :
      ShaderCompilerBase(Target::kSpirv, options)
    {

    }

    //-----------------------------------------------------------------------------------------------
    SpvShaderCompiler::~SpvShaderCompiler()
    {
    }

    
    //-----------------------------------------------------------------------------------------------
    bool SpvShaderCompiler::CompileShader(
      const foundation::String& shader_source,
      const foundation::ShaderAsset& shader,
      const foundation::String& /*path*/,
      foundation::Vector<uint8_t>& out_compiled)
    {
      glslang::InitializeProcess();
      bool result = false;
      EShLanguage language = EShLanguage::EShLangVertex;
      switch (shader.data.stage)
      {
      case foundation::ShaderData::ShaderStage::kCompute:
        language = EShLanguage::EShLangCompute;
        break;
      case foundation::ShaderData::ShaderStage::kDomain:
        language = EShLanguage::EShLangTessEvaluation;
        break;
      case foundation::ShaderData::ShaderStage::kGeometry:
        language = EShLanguage::EShLangGeometry;
        break;
      case foundation::ShaderData::ShaderStage::kHull:
        language = EShLanguage::EShLangTessControl;
        break;
      case foundation::ShaderData::ShaderStage::kPixel:
        language = EShLanguage::EShLangFragment;
        break;
      case foundation::ShaderData::ShaderStage::kVertex:
        language = EShLanguage::EShLangVertex;
        break;
      }

      glslang::TProgram* tprogram = new glslang::TProgram();
      glslang::TShader* tshader = new glslang::TShader(language);

      const char* strings[1] = { shader_source.c_str() };
      const char* names[1] = { shader.name.GetCString() };
      TBuiltInResource resources = default_built_in_resources;

      tshader->setStringsWithLengthsAndNames(strings, NULL, names, 1);
      tshader->setEntryPoint("main");

      tshader->setEnvInput(glslang::EShSourceHlsl,
        language,
        glslang::EShClient::EShClientVulkan,
        PS_VULKAN_VERSION);

      tshader->setEnvClient(glslang::EShClient::EShClientVulkan, PS_VULKAN_VERSION);
      tshader->setEnvTarget(glslang::EShTargetLanguage::EshTargetSpv, PS_SPIRV_VERSION);

      GLSLangIncluder includer;
      for (size_t i = 0; i < options().additional_include_dirs.size(); ++i)
      {
        includer.AddIncludeDirectory(options().additional_include_dirs[i].path().c_str());
      }

      EShMessages messages = EShMessages::EShMsgDefault;
      messages = (EShMessages)(messages | EShMsgSpvRules);
      messages = (EShMessages)(messages | EShMsgVulkanRules);
      messages = (EShMessages)(messages | EShMsgReadHlsl);

      std::string str;

      if (tshader->preprocess(&resources,
        110,
        EProfile::ENoProfile,
        false,
        false,
        messages,
        &str,
        includer) == false)
      {
        PrintErrors(tshader->getInfoLog());
        PrintErrors(tshader->getInfoDebugLog());
        return result;
      }

      if (tshader->parse(&resources,
        110,
        false,
        messages,
        includer) == false)
      {
        PrintErrors(tshader->getInfoLog());
        PrintErrors(tshader->getInfoDebugLog());
        return result;
      }

      tprogram->addShader(tshader);
      PrintErrors(tprogram->getInfoLog());
      PrintErrors(tprogram->getInfoDebugLog());

      tprogram->link(messages);
      PrintErrors(tprogram->getInfoLog());
      PrintErrors(tprogram->getInfoDebugLog());

      std::vector<unsigned int> tmp;
      for (int stage = 0; stage < EShLanguage::EShLangCount; ++stage)
      {
        if (tprogram->getIntermediate(static_cast<EShLanguage>(stage)) != nullptr)
        {
          foundation::String warnings;
          spv::SpvBuildLogger logger;
          glslang::SpvOptions spv_options;
          spv_options.disableOptimizer = true;
          spv_options.optimizeSize = false;
          spv_options.generateDebugInfo = true;
          glslang::GlslangToSpv(*tprogram->getIntermediate(static_cast<EShLanguage>(stage)),
            tmp,
            &logger,
            &spv_options);
          out_compiled.resize(tmp.size() * sizeof(unsigned int));
          memcpy_s(out_compiled.data(), tmp.size() * sizeof(unsigned int), tmp.data(), tmp.size() * sizeof(unsigned int));
          result = true;
        }
      }

      delete tshader;
      delete tprogram;
      glslang::FinalizeProcess();
      return result;
    }

    void SpvShaderCompiler::PrintErrors(const char * msg)
    {
      if (msg != nullptr && *msg != '\0')
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error, msg);
      }
    }
  }
}