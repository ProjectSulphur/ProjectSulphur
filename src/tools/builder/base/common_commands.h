#pragma once
#include "tools/builder/base/commands_base.h"

namespace sulphur 
{ 
  namespace builder 
  {
    class ModelPipeline;
    class MaterialPipeline;
    class MeshPipeline;
    class ShaderPipeline;
    class TexturePipeline;
    enum struct Error;

    /**
    *@class sulphur::builder::SetOutputDir : sulphur::builder::Command
    *@brief command that sets the current directory to output the cache files to
    *@remark only available when running as .exe
    *@author Stan Pepels
    */
    class SetOutputDir : public ICommand
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
      SetOutputDir(const char* key, ModelPipeline* model_pipeline,
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

    private:
      ModelPipeline* model_pipeline_;       //!< a model pipeline passed by the constructor
      MeshPipeline* mesh_pipeline_;         //!< a mesh pipeline passed by the constructor
      MaterialPipeline* material_pipeline_; //!< a material pipeline passed by the constructor
      TexturePipeline* texture_pipeline_;   //!< a texture pipeline passed by the constructor
      ShaderPipeline* shader_pipeline_;     //!< a shader pipeline passed by the constructor
    };

    /**
    *@class sulphur::builder::SetPackagePath : sulphur::builder::Command
    *@brief command that sets the current directory to output the packages to relative to the cache files
    *@remark only available when running as .exe
    *@author Stan Pepels
    */
    class SetPackagePath : public ICommand
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
      SetPackagePath(const char* key, ModelPipeline* model_pipeline,
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

    private:
      ModelPipeline* model_pipeline_;       //!< a model pipeline passed by the constructor
      MeshPipeline* mesh_pipeline_;         //!< a mesh pipeline passed by the constructor
      MaterialPipeline* material_pipeline_; //!< a material pipeline passed by the constructor
      TexturePipeline* texture_pipeline_;   //!< a texture pipeline passed by the constructor
      ShaderPipeline* shader_pipeline_;     //!< a shader pipeline passed by the constructor
    };
  }
}