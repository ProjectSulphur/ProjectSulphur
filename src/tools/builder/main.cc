#include "tools/builder/base/command_system.h"
#include "tools/builder/shared/Application.h"
#include "tools/builder/base/common_commands.h"
#include "tools/builder/base/convert_commands.h"

#include "tools/builder/pipelines/model_pipeline.h"
#include "tools/builder/pipelines/mesh_pipeline.h"
#include "tools/builder/pipelines/material_pipeline.h"
#include "tools/builder/pipelines/shader_pipeline.h"
#include "tools/builder/pipelines/texture_pipeline.h"

#include <foundation/memory/memory.h>   

using namespace sulphur::builder;

//-----------------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
  sulphur::foundation::Memory::Initialize(1024ul * 1024ul * 1024ul);

   // initialize application
  Application::set_out_dir("./");
  Application::set_package_location("sulphur-builder-output");

  if (Application::out_dir().Exists() == false)
  {
    Application::out_dir().Create();
  }

  if(Application::package_dir().Exists() == false)
  {
    Application::package_dir().Create();
  }

  // create pipelines
  ModelPipeline model_pipeline;
  MeshPipeline mesh_pipeline;
  MaterialPipeline material_pipeline;
  TexturePipeline texture_pipeline;
  ShaderPipeline shader_pipeline;


  // set output location
  model_pipeline.SetPackageOutputLocation(Application::package_relative_path());
  mesh_pipeline.SetPackageOutputLocation(Application::package_relative_path());
  material_pipeline.SetPackageOutputLocation(Application::package_relative_path());
  texture_pipeline.SetPackageOutputLocation(Application::package_relative_path());
  shader_pipeline.SetPackageOutputLocation(Application::package_relative_path());

  // initislize pipelines
  model_pipeline.Initialize();
  mesh_pipeline.Initialize();
  material_pipeline.Initialize();
  texture_pipeline.Initialize();
  shader_pipeline.Initialize();

  // initialize command system
  CommandSystem system;
  system.RegisterCommand<ConvertModels>("--convert_models", &model_pipeline, &mesh_pipeline, 
    &material_pipeline, &texture_pipeline, &shader_pipeline);
  system.RegisterCommand<CompileShaders>("--convert_shaders", &model_pipeline, &mesh_pipeline,
    &material_pipeline, &texture_pipeline, &shader_pipeline);
  system.RegisterCommand <ConvertTextures>("--convert_textures", &model_pipeline, &mesh_pipeline,
    &material_pipeline, &texture_pipeline, &shader_pipeline);
  system.RegisterCommand<Convert>("--convert", &model_pipeline, &mesh_pipeline,
    &material_pipeline, &texture_pipeline, &shader_pipeline);

  if (argc > 1)
  {
    sulphur::foundation::String command_line = "";
    for (int i = 1; i < argc; ++i)
    {
      command_line += argv[i];
      if (i != argc - 1)
      {
        command_line += " ";
      }
    }

    system.ExecuteCommandLine(command_line.c_str());
  }
  else
  {
    system.RegisterCommand<SetOutputDir>("--set_output_dir", &model_pipeline, &mesh_pipeline,
      &material_pipeline, &texture_pipeline, &shader_pipeline);
    system.RegisterCommand<SetPackagePath>("--set_package_dir", &model_pipeline, &mesh_pipeline,
      &material_pipeline, &texture_pipeline, &shader_pipeline);

    sulphur::builder::CommandErr error = sulphur::builder::CommandErr::kNoError;
    while (error != sulphur::builder::CommandErr::kExitCommand)
    {
      error = system.ExecuteNext();
    }
  }

  sulphur::foundation::Memory::Shutdown();
  return 0;
}