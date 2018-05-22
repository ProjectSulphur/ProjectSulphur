#pragma once
#include "tools/builder/base/commands_base.h"
#include <foundation/utils/asset_definitions.h>

namespace sulphur 
{
  namespace builder 
  {
    class ModelPipeline;
    class MaterialPipeline;
    class MeshPipeline;
    class ShaderPipeline;
    class TexturePipeline;
    class Directory;
    enum struct Error;

    /**
    *@class sulphur::builder::Convert : sulphur::builder::ICommand
    *@brief command that convertes all assets in a folder.
    *@author Stan Pepels
    */
    class Convert : public ICommand
    {
    public:
      /**
      *@brief constructor
      *@param[in] key (const char*) key to identify this command
      *@param[in] model_pipeline (sulphur::builder::ModelPipeline*) a model pipeline
      *@param[in] mesh_pipeline (sulphur::builder::MeshPipeline*) a mesh pipeline
      *@param[in] material_pipeline (sulphur::builder::MaterialPipeline*) a material pipeline
      *@param[in] texture_pipeline (sulphur::builder::TexturePipeline*) a texture pipeline
      *@param[in] shader_pipeline (sulphur::builder::ShaderPipeline*) a shader pipeline
      */
      Convert(const char* key, ModelPipeline* model_pipeline,
        MeshPipeline* mesh_pipeline, MaterialPipeline* material_pipeline,
        TexturePipeline* texture_pipeline, ShaderPipeline* shader_pipeline);

      /**
      *@see sulphur::builder::Command::GetDescription
      */
      const char* GetDescription() const override;

      /**
      *@see sulphur::builder::Command::Run
      */
      void Run(const CommandInput& input) override;

    protected:
      /**
      *@brief process all files inside folders passed via he command line
      *@param[in] count (const unsigned int) number of times the flag specifying the directory is in the sulphur::builder::CommandInput structure
      *@param[in] input (const sulphur::builder::CommandInput&) parsed command line input containing the folders to be searched
      *@param[in] func (const eastl::function<bool(const foundation::String&)>&) function to use for converting the files
      */
      void ProcessFolders(const CommandInput& input,
                          const eastl::function<bool(const foundation::String&)>& func);

      /**
      *@brief process all files specified in a string delimited with a ','
      *@param[in] file_names (const foundation::string&) string containing the file names delimited with a ','
      *@param[in] input (const sulphur::builder::CommandInput&) the parsed command input
      *@param[in] func (const eastl::function<bool(const foundation::String&)>&) function to use for converting the files
      */
      void ProcessFiles(const foundation::String& file_names,
                        const CommandInput& input,
                        const eastl::function<bool(const foundation::String&)>& func);
 
      /**
      *@brief set the output location for the pipelines cache files
      *@param[in] location (const sulphur::builder::Directory&) the new output location
      *@remark this also changes the package output location
      */
      void SetOutputLocation(const Directory& location);

      /**
      *@brief reset the pipelines output location to the current working directory
      *@remark this also changes the package output location
      */
      void ResetOutputLocation();

      ModelPipeline* model_pipeline_;       //!< a model pipeline passed by the constructor
      MeshPipeline* mesh_pipeline_;         //!< a mesh pipeline passed by the constructor
      MaterialPipeline* material_pipeline_; //!< a material pipeline passed by the constructor
      TexturePipeline* texture_pipeline_;   //!< a texture pipeline passed by the constructor
      ShaderPipeline* shader_pipeline_;     //!< a shader pipeline passed by the constructor
    };

    /**
    *@class sulphur::builder::ConvertModels : sulphur::builder::Convert
    *@brief command converts .obj, .fbx, .gltf into an engine readable formats and
    * outputs them to the directory specified in Application::out_dir
    *@author Stan Pepels
    */
    class ConvertModels : public Convert
    {
    public:
      /**
      *@brief constructor
      *@param[in] key (const char*) key to identify this command
      *@param[in] model_pipeline (sulphur::builder::ModelPipeline*) a model pipeline
      *@param[in] mesh_pipeline (sulphur::builder::MeshPipeline*) a mesh pipeline
      *@param[in] material_pipeline (sulphur::builder::MaterialPipeline*) a material pipeline
      *@param[in] texture_pipeline (sulphur::builder::TexturePipeline*) a texture pipeline
      *@param[in] shader_pipeline (sulphur::builder::ShaderPipeline*) a shader pipeline
      */
      ConvertModels(const char* key, ModelPipeline* model_pipeline,
        MeshPipeline* mesh_pipeline, MaterialPipeline* material_pipeline,
        TexturePipeline* texture_pipeline, ShaderPipeline* shader_pipeline);

      /**
      *@see sulphur::builder::Command::GetDescription
      */
      void Run(const CommandInput& input) final;

      /**
      *@see sulphur::builder::Command::Run
      */
      const char* GetDescription() const final;
    };

    /**
    *@class sulphur::builder::CompileShaders : sulphur::builder::Convert
    *@brief command to convert shader files into bytecode for the following platforms: Vulkan, Gnm and DX12
    *@author Stan Pepels
    */
    class CompileShaders : public Convert
    {
    public:
      /**
      *@brief constructor
      *@param[in] key (const char*) key to identify this command
      *@param[in] model_pipeline (sulphur::builder::ModelPipeline*) a model pipeline
      *@param[in] mesh_pipeline (sulphur::builder::MeshPipeline*) a mesh pipeline
      *@param[in] material_pipeline (sulphur::builder::MaterialPipeline*) a material pipeline
      *@param[in] texture_pipeline (sulphur::builder::TexturePipeline*) a texture pipeline
      *@param[in] shader_pipeline (sulphur::builder::ShaderPipeline*) a shader pipeline
      */
      CompileShaders(const char* key, ModelPipeline* model_pipeline,
        MeshPipeline* mesh_pipeline, MaterialPipeline* material_pipeline,
        TexturePipeline* texture_pipeline, ShaderPipeline* shader_pipeline);

      /**
      *@see sulphur::builder::Command::Run
      */
      void Run(const CommandInput& input) final;

      /**
      *@see sulphur::builder::Command::GetDescription
      */
      const char* GetDescription() const final;
    };
    
    /**
    *@class sulphur::builder::ConvertTextures : sulphur::builder::Convert
    *@brief command to textures from raw format such as jpeg or png to an engine readable binary format
    *@author Stan Pepels
    */
    class ConvertTextures : public Convert
    {
    public:
      /**
      *@brief constructor
      *@param[in] key (const char*) key to identify this command
      *@param[in] model_pipeline (sulphur::builder::ModelPipeline*) a model pipeline
      *@param[in] mesh_pipeline (sulphur::builder::MeshPipeline*) a mesh pipeline
      *@param[in] material_pipeline (sulphur::builder::MaterialPipeline*) a material pipeline
      *@param[in] texture_pipeline (sulphur::builder::TexturePipeline*) a texture pipeline
      *@param[in] shader_pipeline (sulphur::builder::ShaderPipeline*) a shader pipeline
      */
      ConvertTextures(const char* key, ModelPipeline* model_pipeline,
        MeshPipeline* mesh_pipeline, MaterialPipeline* material_pipeline,
        TexturePipeline* texture_pipeline, ShaderPipeline* shader_pipeline);

      /**
      *@see sulphur::builder::Command::Run
      */
      void Run(const CommandInput& input) final;

      /**
      *@see sulphur::builder::Command::GetDescription
      */
      const char* GetDescription() const final;
    };
  }
}
