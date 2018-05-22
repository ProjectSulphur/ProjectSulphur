#include "tools/builder/base/common_commands.h"
#include "tools/builder/shared/application.h"
#include "tools/builder/base/flags.h"
#include "tools/builder/pipelines/model_pipeline.h"
#include "tools/builder/pipelines/shader_pipeline.h"
#include "tools/builder/pipelines/mesh_pipeline.h"
#include "tools/builder/pipelines/material_pipeline.h"
#include "tools/builder/pipelines/texture_pipeline.h"

namespace sulphur 
{
  namespace builder 
  {
    //--------------------------------------------------------------------------
    SetOutputDir::SetOutputDir(const char* key, ModelPipeline* model_pipeline,
      MeshPipeline* mesh_pipeline, MaterialPipeline* material_pipeline,
      TexturePipeline* texture_pipeline, ShaderPipeline* shader_pipeline) :
      ICommand(key),
      model_pipeline_(model_pipeline),
      mesh_pipeline_(mesh_pipeline),
      material_pipeline_(material_pipeline),
      texture_pipeline_(texture_pipeline),
      shader_pipeline_(shader_pipeline)
    {
      SetValidFlags<DirFlag>();
      HasParameter<DirFlag>(true);
    }

    //--------------------------------------------------------------------------
    const char * SetOutputDir::GetDescription() const
    {
      return "set the output directory \n"
        "   -dir<path> path to set the output location to. \n";
    }

    //--------------------------------------------------------------------------
    void SetOutputDir::Run(const CommandInput& input)
    {
      Application::set_out_dir(input.GetFlagArg<DirFlag>());
      Application::set_package_location(Application::out_dir().path() + Application::package_relative_path());

      if (Application::out_dir().Exists() == false)
      {
        Application::out_dir().Create();
      }
      
      if (Application::package_dir().Exists() == false)
      {
        Application::package_dir().Create();
      }

      mesh_pipeline_->SetOutputLocation(Application::out_dir().path());
      model_pipeline_->SetOutputLocation(Application::out_dir().path());
      shader_pipeline_->SetOutputLocation(Application::out_dir().path());
      material_pipeline_->SetOutputLocation(Application::out_dir().path());
      texture_pipeline_->SetOutputLocation(Application::out_dir().path());
    }

    SetPackagePath::SetPackagePath(const char* key, ModelPipeline* model_pipeline,
      MeshPipeline* mesh_pipeline, MaterialPipeline* material_pipeline,
      TexturePipeline* texture_pipeline, ShaderPipeline* shader_pipeline) :
      ICommand(key),
      model_pipeline_(model_pipeline),
      mesh_pipeline_(mesh_pipeline),
      material_pipeline_(material_pipeline),
      texture_pipeline_(texture_pipeline),
      shader_pipeline_(shader_pipeline)
    {
      SetValidFlags<DirFlag>();
      HasParameter<DirFlag>(true);
    }
    const char * SetPackagePath::GetDescription() const
    {
      return "set package output location. \n"
        "   -dir<path> relative path to current output location \n";
    }
    void SetPackagePath::Run(const CommandInput& input)
    {
      Application::set_package_location(input.GetFlagArg<DirFlag>());

      if (Application::package_dir().Exists() == false)
      {
        Application::package_dir().Create();
      }

      mesh_pipeline_->SetPackageOutputLocation(Application::package_relative_path());
      model_pipeline_->SetPackageOutputLocation(Application::package_relative_path());
      texture_pipeline_->SetPackageOutputLocation(Application::package_relative_path());
      shader_pipeline_->SetPackageOutputLocation(Application::package_relative_path());
      material_pipeline_->SetPackageOutputLocation(Application::package_relative_path());
    }
  }
}