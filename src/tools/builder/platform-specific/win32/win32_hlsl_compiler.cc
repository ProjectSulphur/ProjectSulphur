#include "tools/builder/platform-specific/win32/win32_hlsl_compiler.h"
#include "tools/builder/shared/shader_compiler_includer.h"
#include "tools/builder/shared/file_system.h"
#include "tools/builder/pipelines/shader_pipeline_options.h"
#include <foundation/pipeline-assets/shader.h>
#include <foundation/logging/logger.h>
#include <d3dcompiler.h>

namespace sulphur 
{
  namespace builder 
  {
    //-----------------------------------------------------------------------------------------------
    Win32HlslCompiler::Win32HlslCompiler(const ShaderPipelineOptions& options) :
      ShaderCompilerBase(Target::kHlsl, options)
    {
    }

    //-----------------------------------------------------------------------------------------------
    Win32HlslCompiler::~Win32HlslCompiler()
    {
    }

    //-----------------------------------------------------------------------------------------------
    bool Win32HlslCompiler::CompileShader(
      const foundation::String& shader_source,
      const foundation::ShaderAsset& shader,
      const foundation::String& /*path*/,
      foundation::Vector<uint8_t>& out_compiled)
    {
      D3DIncluder includer;
      for (size_t i = 0; i < options().additional_include_dirs.size(); ++i)
      {
        includer.AddIncludeDirectory(options().additional_include_dirs[i].path().c_str());
      }


      uint32_t compile_options = 0;
      compile_options |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
      compile_options |= D3DCOMPILE_ENABLE_STRICTNESS;

#ifndef _DEBUG
      compile_options |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif // 

      ID3DBlob* code;
      ID3DBlob* error_messages = nullptr;
      const char* target = "";

      switch (shader.data.stage)
      {
      case foundation::ShaderData::ShaderStage::kCompute:
        target = "cs_5_0";
        break;
      case foundation::ShaderData::ShaderStage::kDomain:
        target = "ds_5_0";
        break;
      case foundation::ShaderData::ShaderStage::kGeometry:
        target = "gs_5_0";
        break;
      case foundation::ShaderData::ShaderStage::kHull:
        target = "hs_5_0";
        break;
      case foundation::ShaderData::ShaderStage::kPixel:
        target = "ps_5_0";
        break;
      case foundation::ShaderData::ShaderStage::kVertex:
        target = "vs_5_0";
        break;
      }

      D3DCompile(shader_source.data(),
        shader_source.length(),
        NULL,
        NULL,
        &includer,
        "main",
        target,
        compile_options,
        NULL,
        &code,
        &error_messages);

      bool result = true;
      if (error_messages != nullptr)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error, 
          "%s\n", static_cast<const char*>(error_messages->GetBufferPointer()));
        result = false;
        error_messages->Release();
        out_compiled.clear();
      }
      else if(code != nullptr)
      { 
        out_compiled.resize(code->GetBufferSize());
        memcpy_s(out_compiled.data(), code->GetBufferSize(), code->GetBufferPointer(), 
          code->GetBufferSize());
        code->Release();
      }

      return result;
    }
  }
}
