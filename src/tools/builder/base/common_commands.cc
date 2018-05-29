#include "tools/builder/base/common_commands.h"
#include "tools/builder/shared/application.h"
#include "tools/builder/base/flags.h"
#include "tools/builder/pipelines/model_pipeline.h"
#include "tools/builder/pipelines/shader_pipeline.h"
#include "tools/builder/pipelines/mesh_pipeline.h"
#include "tools/builder/pipelines/material_pipeline.h"
#include "tools/builder/pipelines/texture_pipeline.h"
#include "tools/builder/pipelines/skeleton_pipeline.h"
#include "tools/builder/pipelines/animation_pipeline.h"
#include "tools/builder/pipelines/script_pipeline.h"
#include "tools/builder/pipelines/scene_loader.h"
#include "tools/builder/pipelines/audio_pipeline.h"
namespace sulphur
{
  namespace builder
  {
    //--------------------------------------------------------------------------
    SetOutputDir::SetOutputDir(const char* key) :
      ICommand(key)
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
      Application::set_package_location(Application::package_relative_path());

      mesh_pipeline_->SetOutputLocation(Application::out_dir().path());
      model_pipeline_->SetOutputLocation(Application::out_dir().path());
      shader_pipeline_->SetOutputLocation(Application::out_dir().path());
      material_pipeline_->SetOutputLocation(Application::out_dir().path());
      texture_pipeline_->SetOutputLocation(Application::out_dir().path());
      skeleton_pipeline_->SetOutputLocation(Application::out_dir().path());
      animation_pipeline_->SetOutputLocation(Application::out_dir().path());
      script_pipeline_->SetOutputLocation(Application::out_dir().path());
      audio_pipeline_->SetOutputLocation(Application::out_dir().path());
    }

    //--------------------------------------------------------------------------
    SetPackagePath::SetPackagePath(const char* key) :
      ICommand(key)
    {
      SetValidFlags<DirFlag>();
      HasParameter<DirFlag>(true);
    }
    const char* SetPackagePath::GetDescription() const
    {
      return "set package output location. \n"
        "   -dir<path> relative path to current output location \n";
    }
    void SetPackagePath::Run(const CommandInput& input)
    {
      Application::set_package_location(input.GetFlagArg<DirFlag>());

      mesh_pipeline_->SetPackageOutputLocation(Application::package_relative_path());
      model_pipeline_->SetPackageOutputLocation(Application::package_relative_path());
      texture_pipeline_->SetPackageOutputLocation(Application::package_relative_path());
      shader_pipeline_->SetPackageOutputLocation(Application::package_relative_path());
      material_pipeline_->SetPackageOutputLocation(Application::package_relative_path());
      skeleton_pipeline_->SetPackageOutputLocation(Application::package_relative_path());
      animation_pipeline_->SetPackageOutputLocation(Application::package_relative_path());
      script_pipeline_->SetPackageOutputLocation(Application::package_relative_path());
      audio_pipeline_->SetPackageOutputLocation(Application::package_relative_path());
    }

    //--------------------------------------------------------------------------
    ClearOutputFolders::ClearOutputFolders(const char* key) :
      ICommand(key)
    {
    }

    //--------------------------------------------------------------------------
    const char* ClearOutputFolders::GetDescription() const
    {
      return "clear the output folder of all files and reintialize the pipelines. \n"
        "this will clear the cache files and regenerate the default assets";
    }

    //--------------------------------------------------------------------------
    void ClearOutputFolders::Run(const CommandInput&)
    {
      foundation::Vector<foundation::Path> files = Application::package_dir().GetFilesRecursive();
      for (foundation::Path& file : files)
      {
        std::remove(file.GetString().c_str());
      }

      std::remove((Application::out_dir().path() + "model_package.cache").GetString().c_str());
      std::remove((Application::out_dir().path() + "shader_package.cache").GetString().c_str());
      std::remove((Application::out_dir().path() + "texture_package.cache").GetString().c_str());
      std::remove((Application::out_dir().path() + "material_package.cache").GetString().c_str());
      std::remove((Application::out_dir().path() + "mesh_package.cache").GetString().c_str());
      std::remove((Application::out_dir().path() + "skeleton_package.cache").GetString().c_str());
      std::remove((Application::out_dir().path() + "animation_package.cache").GetString().c_str());
      std::remove((Application::out_dir().path() + "script_package.cache").GetString().c_str());
      std::remove((Application::out_dir().path() + "audio_package.cache").GetString().c_str());

      mesh_pipeline_->Initialize();
      model_pipeline_->Initialize();
      texture_pipeline_->Initialize();
      shader_pipeline_->Initialize();
      material_pipeline_->Initialize();
      skeleton_pipeline_->Initialize();
      animation_pipeline_->Initialize();
      script_pipeline_->Initialize();
      audio_pipeline_->Initialize();
    }

    //--------------------------------------------------------------------------
    RefreshCacheFiles::RefreshCacheFiles(const char* key) :
      ICommand(key)
    {
    }

    //--------------------------------------------------------------------------
    const char* RefreshCacheFiles::GetDescription() const
    {
      return "checks if the caches are up to date with the package output folder and updates the caches accordingly";
    }

    //--------------------------------------------------------------------------
    void RefreshCacheFiles::Run(const CommandInput&)
    {
      mesh_pipeline_->RefreshCache();
      model_pipeline_->RefreshCache();
      texture_pipeline_->RefreshCache();
      shader_pipeline_->RefreshCache();
      material_pipeline_->RefreshCache();
      skeleton_pipeline_->RefreshCache();
      animation_pipeline_->RefreshCache();
      script_pipeline_->RefreshCache();
      audio_pipeline_->RefreshCache();
    }
  }
}