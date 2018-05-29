#pragma once
#include "tools/builder/shared/shader_compiler_base.h"

#include <foundation/containers/vector.h>

namespace sulphur 
{
  namespace builder 
  {
    /**
    *@class sulphur::builder::SpvShaderCompiler : sulphur::builder::ShaderCompilerBase 
    *@brief this class compiles hlsl files into spirv binary format,
    *       reflects on shaders, validates shaders and invokes the platform
    *       specific compilers specified by compiler flags 
    *@author Stan Pepels
    */
    class SpvShaderCompiler : public ShaderCompilerBase
    {
    public:
      /**
      *@brief constructor
      *@param[in] options (const sulphur::builder::CompilerOptions&) options to compile the shader with
      */
      SpvShaderCompiler(const ShaderPipelineOptions& options);

      /**
      *@brief destructor
      */
      ~SpvShaderCompiler() final;

      /**
      *@see sulphur::builder::ShaderCompilerBase::CompileShader
      */
      bool CompileShader(const foundation::String& shader_source,
                         const foundation::ShaderAsset& shader,
                         const foundation::Path& /*path*/,
                         foundation::Vector<uint8_t>& out_compiled) final;

      /**
      *@brief print errors to output console
      *@param[in] msg (const char*) message to be printed
      */
      void PrintErrors(const char* msg);
    };
  }
}

