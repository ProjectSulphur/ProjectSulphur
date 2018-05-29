#include "tools/builder/base/command_system.h"
#include "tools/builder/shared/Application.h"
#include "tools/builder/base/common_commands.h"
#include "tools/builder/base/convert_commands.h"

#include "tools/builder/pipelines/model_pipeline.h"
#include "tools/builder/pipelines/mesh_pipeline.h"
#include "tools/builder/pipelines/material_pipeline.h"
#include "tools/builder/pipelines/shader_pipeline.h"
#include "tools/builder/pipelines/texture_pipeline.h"
#include "tools/builder/pipelines/skeleton_pipeline.h"
#include "tools/builder/pipelines/animation_pipeline.h"
#include "tools/builder/pipelines/script_pipeline.h"
#include "tools/builder/pipelines/audio_pipeline.h"

#include <foundation/memory/memory.h>
using namespace sulphur::builder;

//-----------------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
  sulphur::foundation::Memory::Initialize(1024ul * 1024ul * 1024ul);
  Application app;
  {
    // create pipelines
    ModelPipeline model_pipeline;
    MeshPipeline mesh_pipeline;
    MaterialPipeline material_pipeline;
    TexturePipeline texture_pipeline;
    ShaderPipeline shader_pipeline;
    SkeletonPipeline skeleton_pipeline;
    AnimationPipeline animation_pipeline;
    ScriptPipeline script_pipeline;
    AudioPipeline audio_pipeline;
    SceneLoader scene_loader;

    // set output location
    model_pipeline.SetPackageOutputLocation(Application::package_relative_path());
    mesh_pipeline.SetPackageOutputLocation(Application::package_relative_path());
    material_pipeline.SetPackageOutputLocation(Application::package_relative_path());
    texture_pipeline.SetPackageOutputLocation(Application::package_relative_path());
    shader_pipeline.SetPackageOutputLocation(Application::package_relative_path());
    skeleton_pipeline.SetPackageOutputLocation(Application::package_relative_path());
    animation_pipeline.SetPackageOutputLocation(Application::package_relative_path());
    script_pipeline.SetPackageOutputLocation(Application::package_relative_path());
    audio_pipeline.SetPackageOutputLocation(Application::package_relative_path());

    // initislize pipelines
    model_pipeline.Initialize();
    mesh_pipeline.Initialize();
    material_pipeline.Initialize();
    texture_pipeline.Initialize();
    shader_pipeline.Initialize();
    skeleton_pipeline.Initialize();
    animation_pipeline.Initialize();
    script_pipeline.Initialize();
    audio_pipeline.Initialize();

    // initialize command system
    CommandSystem system(&model_pipeline, &mesh_pipeline, &material_pipeline,
      &texture_pipeline, &shader_pipeline, &skeleton_pipeline, &animation_pipeline,
      &script_pipeline, &audio_pipeline, &scene_loader);
    system.RegisterCommand<ConvertModels>("--convert_models");
    system.RegisterCommand<CompileShaders>("--convert_shaders");
    system.RegisterCommand<ConvertTextures>("--convert_textures");
    system.RegisterCommand<ConvertSkeletons>("--convert_skeletons");
    system.RegisterCommand<ConvertAnimations>("--convert_animations");
    system.RegisterCommand<ConvertScript>("--convert_scripts");
    system.RegisterCommand<ConvertAudioBank>("--convert_audio");
    system.RegisterCommand<Convert>("--convert");
    system.RegisterCommand<ClearOutputFolders>("--clear_output");
    system.RegisterCommand<RefreshCacheFiles>("--refresh_cache");

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
      system.RegisterCommand<SetOutputDir>("--set_output_dir");
      system.RegisterCommand<SetPackagePath>("--set_package_dir");

      sulphur::builder::CommandErr error = sulphur::builder::CommandErr::kNoError;
      while (error != sulphur::builder::CommandErr::kExitCommand)
      {
        error = system.ExecuteNext();
      }
    }
  }
  app.ShutDown();
  sulphur::foundation::Memory::Shutdown();
  return 0;
}