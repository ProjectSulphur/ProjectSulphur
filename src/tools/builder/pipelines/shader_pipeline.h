#pragma once
#include "tools/builder/pipelines/pipeline_base.h"
#include <foundation/utils/template_util.h>
#include <foundation/pipeline-assets/shader.h>

namespace glslang
{
  class TShader;
  class TProgram;
}

namespace spirv_cross
{
  struct Resource;
  struct SPIRType;
  class Compiler;
}

namespace sulphur 
{

  namespace builder 
  {
    struct ShaderPipelineOptions;
    class ShaderCompilerBase;

    /**
    * @class sulphur::builder::ShaderPipeline : shader::builder::PipelineBase
    * @brief coverts hlsl shaders to bytecode for Vulkan, Gnm and DirectX12
    * @author Stan Pepels, Timo van Hees
    */
    class ShaderPipeline : public PipelineBase
    {
    public:
      ShaderPipeline() = default;
      ~ShaderPipeline() = default;
      
      /**
       * @brief Loads, compiles and reflects on a shader loaded from disk and returns it as an asset.
       * @param[in] shader_file (const sulphur::foundation::String&) File containing the shader source.
       * @param[in] options (const sulphur::builder::ShaderPipelineOptions&) Options for shader compilation.
       * @param[out] shader (sulphur::foundation::ShaderAsset&) Shader asset created from shader file.
       * @return (bool) False when there was an error that couldn't be recovered from. 
       * @remark If the function returned false, the shader should be discarded.
       */
      bool Create(const foundation::String& shader_file, const ShaderPipelineOptions& options, 
        foundation::ShaderAsset& shader);

      /**
      * @brief Adds a texture to the package.
      * @param[in] asset_origin (const sulphur::foundation::String&) The file the asset was
      * created from. Should be ASSET_ORIGIN_USER when the asset is created by the user.
      * @param[in] shader (sulphur::foundation::ShaderAsset&) The shader to add to the package.
      * @return (bool) True if the shader was added to the package succesfully.
      */
      bool PackageShader(const foundation::String& asset_origin, foundation::ShaderAsset& shader);

      /**
      * @see sulphur::builder::PipelineBase
      */
      foundation::String GetCacheName() const override;

      /**
      * @see sulphur::builder::PipelineBase
      */
      foundation::String GetPackageExtension() const override;
    private:
      /**
      *@brief construct the platform specific shader compilers
      *@param[in] Args types of compiler to put the loaded shader through
      */
      template<typename... Args>
      void ConstructCompilers(const ShaderPipelineOptions& options);

      /**
      *@brief free the memory allocated by the sulphur::builder::ShaderPipeline::Contruct method
      */
      void DeconstructCompilers();

      /**
      *@brief reflect on shader using spirv cross
      *@param[in] compiled (sulphur::foundation::Vector<unsigned int>&) compiled spirv data
      *@param[out] shader (sulphur::builder::Shader&) shader struct now complete with its resouces
      */
      void Reflect(const foundation::Vector<uint8_t>& compiled, foundation::ShaderData& shader);

      /**
      *@brief find the shader name and store it in the shader struct
      *@param[in] path (const sulphur::builder::String&) path to the shader on disk
      *@param[out] shader (sulphur::foundation::ShaderAsset&) shader struct to fill in
      */
      bool GetShaderName(const foundation::String& path, foundation::ShaderAsset& shader);

      /**
      *@brief find the shader stage and store it in the shader struct
      *@param[in] path (const sulphur::builder::String&) path to the shader on disk
      *@param[out] shader (sulphur::foundation::ShaderData&) shader struct to fill in
      */
      bool GetShaderStage(const foundation::String& path, foundation::ShaderData& shader);

      /**
      *@brief use sulphur::builder::SpvShaderCompiler to get spirv compiled output 
      *@param[in] shader_source (sulphur::foundation::String& shader_source) source data to validate and compile
      *@param[in|out] shader (sulphur::foundation::ShaderAsset&) shader struct for reflection data
      *@param[in] path (const sulphur::foundation::String&) path to the source file
      *@param[in] options (const sulphur::builder::ShaderPipelineOptions&) options to process the shader with
      *@param[out] out_compiled (sulphur::foundation::Vector<unsigned int>&) compiled data in spirv format ready for reflection
      *@return (bool) boolean indicating whether the passed source data is valid hlsl
      */
      bool ValidateSource(const foundation::String& shader_source,
                          foundation::ShaderAsset& shader,
                          const foundation::String& path,
                          const ShaderPipelineOptions& options,
                          foundation::Vector<uint8_t>& out_compiled);


      /**
      *@brief fills in the resource with the reflection data from spirv_cross
      *@param[in|out] resource (sulphur::foundation::Resource&)the resource to fill
      *@param[in] spv_type (const spirv_cross::SPIRType&) the type of resource retrieved from spirv cross
      *@param[in] compiler (const spirv_cross::Compiler&) the spirv_cross compiler instatiated with the shader data
      */
      void ProcessShaderResource(foundation::ShaderResource& resource,
                                 const spirv_cross::SPIRType& spv_type,
                                 const spirv_cross::Compiler& compiler);


    protected:
      /*
       * @see sulphur::builder::PipelineBase::PackageDefaultAssets
       */
      bool PackageDefaultAssets() override;

    private:
      bool CreateFromSource(const foundation::String& source,
        const foundation::String& shader_file,
        const foundation::String& name,
        foundation::ShaderData::ShaderStage shader_stage,
        const ShaderPipelineOptions& options,
        foundation::ShaderAsset& shader);

      static foundation::String GetShaderDefines();

      template<typename... Args>
      using  derived_from_comiler_base =
        foundation::and_cond<eastl::is_base_of<ShaderCompilerBase, Args>...>; //<! check if all types given by Args, are derived from ShaderCompilerBase on compile time

      foundation::Vector<ShaderCompilerBase*> compilers_; //<! list of compiler to put the shader through
    };

    template<typename... Args>
    inline void ShaderPipeline::ConstructCompilers(const ShaderPipelineOptions& options)
    {
      static_assert(derived_from_comiler_base<Args...>::value,
        "ShaderCompiler::Construct template argument not derived from ShaderCompilerBase");

      compilers_ = { new Args(options)... };
    }
  }
}