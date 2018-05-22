#pragma once 
#include <foundation/containers/string.h>
#include <foundation/containers/vector.h>

namespace sulphur 
{
  namespace foundation 
  {
    struct ShaderAsset;
  }

  namespace builder 
  {
    struct ShaderPipelineOptions;
    class Directory;

    /**
    *@class sulphur::builder::ShaderCompilerBase
    *@brief base class for compiling shaders to bytecode there should be a derived version for each possible taret platform
    *@author Stan Pepels
    */
    class ShaderCompilerBase
    {
    public:
      /**
      *@brief specific shader targets
      */
      enum struct Target : uint8_t
      {
        kHlsl = 1 << 0,
        kPssl = 1 << 1,
        kSpirv = 1 << 2,
        kAll = 1 << 3,
      };

      /**
      *@brief constructor
      *@param[in] target (sulphur::builder::ShaderCompilerBase::Target) target the derived compiler compiles for
      *@param[in] options (const sulphur::builder::ShaderPipelineOptions&) options to compile the shader with
      */
      ShaderCompilerBase(Target target, const ShaderPipelineOptions& options);

      /**
      *@brief destructor
      */
      virtual ~ShaderCompilerBase();

      /**
      *@brief compiles shader to bytecode for the specified target platform
      *@param[in] shader_source (const sulphur::foundation::String&) shader source data as read from disk
      *@param[in] shader (const sulphur::foundation::ShaderAsset&) shader to convert to bytecode
      *@param[in] path (const sulphur::foundation::String&) path to the original file
      *@param[out] compiled (sulphur::builder::Vector<unsigned int>&) compiled shader data
      *@return (bool) boolean indicating if the compilation was successfull
      */
      virtual bool CompileShader(const foundation::String& shader_source,
                                 const foundation::ShaderAsset& shader,
                                 const foundation::String& path,
                                 foundation::Vector<uint8_t>& compiled) = 0;
      /**
      *@brief get the target this compiler compiles for
      *@brief (sulphur::builder::ShaderCompilerBase::Target) target platform to compile the source to
      */
      Target target();
    protected:

      /**
      *@brief get the current compile options
      */
      const ShaderPipelineOptions& options() const;

    private:
      const ShaderPipelineOptions& options_; //<! compile options shaders are compiled with
      Target target_; //<! target to convert shaders to
    };
  }
}