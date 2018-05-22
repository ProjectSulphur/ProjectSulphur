#pragma once
#include "tools/builder/shared/shader_compiler_base.h"

namespace sulphur 
{
  namespace builder 
  {
    /**
    *@class sulphur::builder::Win32HlslCompiler : sulphur::builder::ShaderCompilerBase
    *@brief compiles hlsl files into DX12 compatible bytecode
    *@author Stan Pepels
    */
    class Win32HlslCompiler : public ShaderCompilerBase
    {
    public:
      /**
      *@brief constructor
      *@param[in] options (const sulphur::builder::CompilerOptions&) options to compile the shader with
      */
      Win32HlslCompiler(const ShaderPipelineOptions& options);

      /**
      *@brief destructor
      */
      ~Win32HlslCompiler() final;

      /**
      *@see sulphur::builder::ShaderCompilerBase::CompileShader
      */
      bool CompileShader(const foundation::String& shader_source,
                         const foundation::ShaderAsset& shader,
                         const foundation::String& /*path*/,
                         foundation::Vector<uint8_t>& out_compiled) final;
    };
  }
}