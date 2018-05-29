#include "tools/builder/base/convert_commands.h"
#include "tools/builder/base/flags.h"
#include "tools/builder/shared/application.h"
#include "tools/builder/pipelines/shader_pipeline_options.h"
#include "tools/builder/shared/file_system.h"
#include "tools/builder/pipelines/model_pipeline.h"
#include "tools/builder/pipelines/shader_pipeline.h"
#include "tools/builder/pipelines/mesh_pipeline.h"
#include "tools/builder/pipelines/material_pipeline.h"
#include "tools/builder/pipelines/texture_pipeline.h"
#include "tools/builder/pipelines/skeleton_pipeline.h"
#include "tools/builder/pipelines/animation_pipeline.h"
#include "tools/builder/pipelines/script_pipeline.h"
#include "tools/builder/pipelines/audio_pipeline.h"
#include "tools/builder/shared/shader_compiler_base.h"

#include <foundation/pipeline-assets/model_info.h>
#include <foundation/pipeline-assets/shader.h>
#include <foundation/pipeline-assets/texture.h>
#include <foundation/pipeline-assets/skeleton.h>
#include <foundation/pipeline-assets/animation.h>
#include <foundation/pipeline-assets/script.h>
#include <foundation/pipeline-assets/audio.h>

#include <fstream>

namespace sulphur
{
  namespace builder
  {
    //--------------------------------------------------------------------------
    ConvertModels::ConvertModels(const char* key) :
      Convert(key)
    {
      SetValidFlags<DirFlag,
        FileFlag,
        RecursiveFlag,
        VertexShaderFlag,
        PixelShaderFlag,
        SingleFlag,
        OutputLocationFlag>();
      HasParameter<DirFlag>(true);
      HasParameter<FileFlag>(true);
      HasParameter<VertexShaderFlag>(true);
      HasParameter<PixelShaderFlag>(true);
      HasParameter<OutputLocationFlag>(true);

      AllowMultipleOccurances<DirFlag>(true);
      IsOptional<DirFlag>(true);
      IsOptional<RecursiveFlag>(true);
      IsOptional<SingleFlag>(true);
      IsOptional<OutputLocationFlag>(true);
      IsOptional<FileFlag>(true);
    }

    //--------------------------------------------------------------------------
    void ConvertModels::Run(const CommandInput& input)
    {
      if (input.HasFlag<OutputLocationFlag>() == true)
      {
        Directory location = input.GetFlagArg<OutputLocationFlag>();
        SetOutputLocation(location);
      }

      model_pipeline_->PackageDefaultAssets();
      mesh_pipeline_->PackageDefaultAssets();
      skeleton_pipeline_->PackageDefaultAssets();
      material_pipeline_->PackageDefaultAssets();
      texture_pipeline_->PackageDefaultAssets();
      shader_pipeline_->PackageDefaultAssets();

      eastl::function<bool(const foundation::String&)> func =
        [this, &input](const foundation::Path& file)
      {
        foundation::String extension = file.GetFileExtension();
        if (extension != "obj" && extension != "fbx" && extension != "gltf")
        {
            return false;
        }

        // Convert models
        foundation::ModelInfo info =
          model_pipeline_->GetModelInfo(*scene_loader_, file,
            input.HasFlag<SingleFlag>());

        foundation::Vector<foundation::ModelAsset> models;
        if (model_pipeline_->Create(*scene_loader_, file,
          input.HasFlag<SingleFlag>(),
          info, *mesh_pipeline_,
          *skeleton_pipeline_,
          *material_pipeline_,
          *texture_pipeline_,
          *shader_pipeline_,
          input.GetFlagArg<VertexShaderFlag>(),
          input.GetFlagArg<PixelShaderFlag>(),
          models) == false)
        {
          PS_LOG_BUILDER(Error, "Failed to create models from %s", file.GetString().c_str());
          return false;
        }

        // Convert animations
        foundation::Vector<foundation::AnimationAsset> animations;
        if(animation_pipeline_->Create(file, *scene_loader_, animations) == false)
        {
          PS_LOG_BUILDER(Error, "Failed to create animations from %s", file.GetString().c_str());
          return false;
        }

        // Convert skeletons
        foundation::Vector<foundation::SkeletonAsset> skeletons;
        if (skeleton_pipeline_->Create(file, *scene_loader_, skeletons) == false)
        {
          PS_LOG_BUILDER(Error, "Failed to create skeletons from %s", file.GetString().c_str());
          return false;
        }

        // Package models
        for (foundation::ModelAsset& model : models)
        {
          if (model_pipeline_->PackageModel(file,
            model,
            *mesh_pipeline_,
            *skeleton_pipeline_,
            *material_pipeline_,
            *texture_pipeline_) == false)
          {
            PS_LOG_BUILDER(Error, "Failed to package model %s", model.name.GetCString());
            continue;
          }

          PS_LOG_BUILDER(Info, "Succesfully packaged model %s", model.name.GetCString());
        }

        // Package animations
        for (foundation::AnimationAsset& animation : animations)
        {
          if (animation_pipeline_->PackageAnimation(file, animation) == false)
          {
            PS_LOG_BUILDER(Error, "Failed to package animation %s", animation.name.GetCString());
            continue;
          }

          PS_LOG_BUILDER(Info, "Succesfully packaged animation %s", animation.name.GetCString());
        }

        // Package skeletons
        for (foundation::SkeletonAsset& skeleton : skeletons)
        {
          if (skeleton_pipeline_->PackageSkeleton(file, skeleton) == false)
          {
            PS_LOG_BUILDER(Error, "Failed to package skeleton %s", skeleton.name.GetCString());
            continue;
          }
          PS_LOG_BUILDER(Info, "Successfully packaged skeleton %s", skeleton.name.GetCString());
        }

        return true;
      };

      if (input.HasFlag<FileFlag>() == true)
      {
        foundation::String files = input.GetFlagArg<FileFlag>();
        ProcessFiles(files, input, func);
      }
      else
      {
        ProcessFolders(input, func);
      }

      if (input.HasFlag<OutputLocationFlag>() == true)
      {
        ResetOutputLocation();
      }
    }

    //--------------------------------------------------------------------------
    const char* ConvertModels::GetDescription() const
    {
      return "convert models from *.obj, *.fbx or *.gtlf to engine readable format \n"
        "can only be used in combination with -dir flag \n"
        "   -vertex<name>                vertex shader name. Must already be packaged \n"
        "   -pixel<name>                 pixel shader name. Must already be packaged \n"
        "   [opt]-dir<path>              convert all models at the specified path. multiple flags can be specified. \n"
        "   [opt]-file<name>,<name>...   convert single files located in the directory specified with -dir every file must be delimitied with a ',' \n"
        "   [opt]-single                 forces the model to be interpreted as a single mesh \n"
        "                                if not specified -file must be specified. cannot be combined with -file flag \n"
        "   [opt]-r                      search the directory specified with -dir flag recursivly i.e. also go through subfolders \n"
        "   [opt]-output <path>          path where to put the generated cache file and the folder containing the processed assets \n"
        "                                if not specified working directory will be used \n"
        "                                if specified it is assumed that the vertex and pixel shader specified with the -vertex and -pixel flag are compiled to caches allready located at the given output path \n";
    }

    //--------------------------------------------------------------------------
    CompileShaders::CompileShaders(const char* key) :
      Convert(key)
    {
      SetValidFlags<PSSLFlag,
        HLSLFlag,
        SpirvFlag,
        DirFlag,
        FileFlag,
        RecursiveFlag,
        IncludeDirFlag,
        OutputLocationFlag>();

      AllowMultipleOccurances<DirFlag>(true);
      AllowMultipleOccurances<IncludeDirFlag>(true);

      HasParameter<IncludeDirFlag>(true);
      HasParameter<FileFlag>(true);
      HasParameter<DirFlag>(true);
      HasParameter<OutputLocationFlag>(true);

      IsOptional<DirFlag>(true);
      IsOptional<PSSLFlag>(true);
      IsOptional<HLSLFlag>(true);
      IsOptional<SpirvFlag>(true);
      IsOptional<FileFlag>(true);
      IsOptional<IncludeDirFlag>(true);
      IsOptional<OutputLocationFlag>(true);
      IsOptional<RecursiveFlag>(true);
    }

    //--------------------------------------------------------------------------
    void CompileShaders::Run(const CommandInput& input)
    {
      unsigned int count = 0;

      ShaderPipelineOptions options;
      foundation::String err_msg;
      options.targets = 0;
      if (Directory("./include/").Exists() == true)
      {
        options.additional_include_dirs = { "./include/" };
      }

      if (input.HasFlag<IncludeDirFlag>(count))
      {
        for (unsigned int i = 0; i < count; ++i)
        {
          Directory dir(input.GetFlagArg<IncludeDirFlag>(i));
          options.additional_include_dirs.push_back(dir);
        }
      }

      if (input.HasFlag<PSSLFlag>(count) == true)
      {
        options.targets |= static_cast<uint8_t>(ShaderCompilerBase::Target::kPssl);
      }

      if (input.HasFlag<HLSLFlag>(count) == true)
      {
        options.targets |= static_cast<uint8_t>(ShaderCompilerBase::Target::kHlsl);
      }

      if (input.HasFlag<SpirvFlag>(count) == true)
      {
        options.targets |= static_cast<uint8_t>(ShaderCompilerBase::Target::kSpirv);
      }

      if (options.targets == 0)
      {
        options.targets |= static_cast<uint8_t>(ShaderCompilerBase::Target::kAll);
      }

      if (input.HasFlag<OutputLocationFlag>() == true)
      {
        Directory location = input.GetFlagArg<OutputLocationFlag>();
        SetOutputLocation(location);
      }

      shader_pipeline_->PackageDefaultAssets();

      eastl::function<bool(const foundation::String&)> func =
        [this, &options](const foundation::Path& file)
      {
        foundation::String extension = file.GetFileExtension();
        if(extension != "vert" && extension != "pixe" && extension != "geom" && 
          extension != "comp" && extension != "doma" && extension != "hull")
        {
          return false;
        }

        foundation::ShaderAsset shader = {};
        if (shader_pipeline_->Create(file, options, shader) == false)
        {
          PS_LOG_BUILDER(Error, "Failed to create shader from %s", file.GetString().c_str());
          return false;
        }

        if (shader_pipeline_->PackageShader(file, shader) == false)
        {
          PS_LOG_BUILDER(Error, "Failed to package shader %s", shader.name.GetCString());
          return false;
        }

        PS_LOG_BUILDER(Info, "Succesfully packaged shader %s", shader.name.GetCString());
        return true;
      };

      if (input.HasFlag<FileFlag>() == true)
      {
        foundation::String files = input.GetFlagArg<FileFlag>();
        ProcessFiles(files, input, func);
      }
      else
      {
        ProcessFolders(input, func);
      }

      if (input.HasFlag<OutputLocationFlag>() == true)
      {
        ResetOutputLocation();
      }
    }

    //--------------------------------------------------------------------------
    const char* CompileShaders::GetDescription() const
    {
      return "compile shaders to bytecode \n"
        "   [opt]-dir<path>              convert all shaders located in working directory \n"
        "   [opt]-file<name>,<name>...   convert single files located in the directory specified with -dir every file must be delimitied with a ',' \n"
        "   [opt]-r                      can only be used in combination with -dir flag \n"
        "   [opt]-incl_dir<path>         specify additional include directories. ./include/ folder is searched by default if it exists \n"
        "   [opt]-pssl                   convert to pssl bytecode \n"
        "   [opt]-hlsl                   convert to hlsl bytecode \n"
        "   [opt]-spirv                  convert to spirv bytecode \n"
        "   [opt]-output <path>          path where to put the generated cache file and the folder containing the processed assets \n"
        "                                if not specified working directory will be used";
    }

    //--------------------------------------------------------------------------
    ConvertTextures::ConvertTextures(const char* key) :
      Convert(key)
    {
      SetValidFlags<DirFlag,
        FileFlag,
        CompressionTypeFlag,
        MipMapFlag,
        RecursiveFlag,
        OutputLocationFlag>();
      AllowMultipleOccurances<DirFlag>(true);

      HasParameter<CompressionTypeFlag>(true);
      HasParameter<FileFlag>(true);
      HasParameter<DirFlag>(true);
      HasParameter<OutputLocationFlag>(true);

      IsOptional<DirFlag>(true);
      IsOptional<FileFlag>(true);
      IsOptional<CompressionTypeFlag>(true);
      IsOptional<MipMapFlag>(true);
      IsOptional<RecursiveFlag>(true);
      IsOptional<OutputLocationFlag>(true);
    }

    //--------------------------------------------------------------------------
    void ConvertTextures::Run(const CommandInput& input)
    {
      if (input.HasFlag<OutputLocationFlag>() == true)
      {
        Directory location = input.GetFlagArg<OutputLocationFlag>();
        SetOutputLocation(location);
      }

      texture_pipeline_->PackageDefaultAssets();

      foundation::TextureAsset out;
      eastl::function<bool(const foundation::String&)> func =
        [this, &out](const foundation::Path& file)
      {
        foundation::String extension = file.GetFileExtension();
        if (extension != "png" &&
          extension != "jpeg" &&
          extension != "tga" &&
          extension != "bmp" &&
          extension != "dds" &&
          extension != "jpg")
        {
          return false;
        }

        if (texture_pipeline_->Create(file, out) == false)
        {
          PS_LOG_BUILDER(Error, "Failed to create texture from %s", file.GetString().c_str());
          return false;
        }

        if (texture_pipeline_->PackageTexture(file, out) == false)
        {
          PS_LOG_BUILDER(Error, "Failed to package texture %s", out.name.GetCString());
          return false;
        }

        PS_LOG_BUILDER(Info, "Succesfully packaged texture %s", out.name.GetCString());
        return true;
      };

      if (input.HasFlag<FileFlag>() == true)
      {
        foundation::String files = input.GetFlagArg<FileFlag>();
        ProcessFiles(files, input, func);
      }
      else
      {
        ProcessFolders(input, func);
      }

      if (input.HasFlag<OutputLocationFlag>() == true)
      {
        ResetOutputLocation();
      }
    }

    //--------------------------------------------------------------------------
    const char* ConvertTextures::GetDescription() const
    {
      return "convert textures from *.png, *.jpeg, *.tga, *.bmp \n"
        "to an engine readable format \n"
        "   [opt]-dir<path>              convert all textures located in working directory \n"
        "   [opt]-r                      search the working directory recursivly i.e. also go through subfolders \n"
        "                                can only be used in combination with -dir flag \n"
        "   [opt]-file<name>,<name>...   convert single files located in the directory specified with -dir every file must be delimitied with a ',' \n"
        "   [opt]-mipmap                 enable mipmaps \n"
        "   [opt]-compression<type>      set compression type. types: Block, NormalMap \n"
        "   [opt]-output <path>          path where to put the generated cache file and the folder containing the processed assets \n"
        "                                if not specified working directory will be used";
    }

    //--------------------------------------------------------------------------
    ConvertSkeletons::ConvertSkeletons(const char* key) :
      Convert(key)
    {
      SetValidFlags<DirFlag,
        FileFlag,
        RecursiveFlag,
        OutputLocationFlag>();
      AllowMultipleOccurances<DirFlag>(true);

      HasParameter<FileFlag>(true);
      HasParameter<DirFlag>(true);
      HasParameter<OutputLocationFlag>(true);

      IsOptional<DirFlag>(true);
      IsOptional<FileFlag>(true);
      IsOptional<RecursiveFlag>(true);
      IsOptional<OutputLocationFlag>(true);
    }

    //--------------------------------------------------------------------------
    const char* ConvertSkeletons::GetDescription() const
    {
      return "convert skeletons from *.fbx, *.gltf \n"
        "to an engine readable format \n"
        "   [opt]-dir<path>              convert all skeletons located in working directory \n"
        "   [opt]-r                      search the working directory recursivly i.e. also go through subfolders \n"
        "                                can only be used in combination with -dir flag \n"
        "   [opt]-file<name>,<name>...   convert single files located in the directory specified with -dir every file must be delimitied with a ',' \n"
        "   [opt]-output <path>          path where to put the generated cache file and the folder containing the processed assets \n"
        "                                if not specified working directory will be used";
    }

    //--------------------------------------------------------------------------
    void ConvertSkeletons::Run(const CommandInput& input)
    {
      if (input.HasFlag<OutputLocationFlag>() == true)
      {
        Directory location = input.GetFlagArg<OutputLocationFlag>();
        SetOutputLocation(location);
      }

      skeleton_pipeline_->PackageDefaultAssets();

      foundation::Vector<foundation::SkeletonAsset> out;
      eastl::function<bool(const foundation::String&)> func =
        [this, &out](const foundation::Path& file)
      {
        foundation::String extension = file.GetFileExtension();
        if(extension != "fbx" && extension != "gltf")
        {
          return false;
        }

        if (skeleton_pipeline_->Create(file, *scene_loader_, out) == false)
        {
          PS_LOG_BUILDER(Error, "Failed to create skeletons from %s", file.GetString().c_str());
          return false;
        }

        for (int i = 0; i < out.size(); ++i)
        {
          if (skeleton_pipeline_->PackageSkeleton(file, out[i]) == false)
          {
            PS_LOG_BUILDER(Error, "Failed to package skeleton %s", out[i].name.GetCString());
            return false;
          }

          PS_LOG_BUILDER(Info, "Succesfully packaged skeleton %s", out[i].name.GetCString());
        }
        return true;
      };

      if (input.HasFlag<FileFlag>() == true)
      {
        foundation::String files = input.GetFlagArg<FileFlag>();
        ProcessFiles(files, input, func);
      }
      else
      {
        ProcessFolders(input, func);
      }

      if (input.HasFlag<OutputLocationFlag>() == true)
      {
        ResetOutputLocation();
      }
    }



    //--------------------------------------------------------------------------
    ConvertAnimations::ConvertAnimations(const char* key) :
      Convert(key)
    {
      SetValidFlags<DirFlag,
        FileFlag,
        RecursiveFlag,
        OutputLocationFlag>();
      AllowMultipleOccurances<DirFlag>(true);

      HasParameter<FileFlag>(true);
      HasParameter<DirFlag>(true);
      HasParameter<OutputLocationFlag>(true);

      IsOptional<DirFlag>(true);
      IsOptional<FileFlag>(true);
      IsOptional<RecursiveFlag>(true);
      IsOptional<OutputLocationFlag>(true);
    }

    //--------------------------------------------------------------------------
    const char* ConvertAnimations::GetDescription() const
    {
      return "convert animations from *.fbx, *.gltf \n"
        "to an engine readable format \n"
        "   [opt]-dir<path>              convert all animations located in working directory \n"
        "   [opt]-r                      search the working directory recursivly i.e. also go through subfolders \n"
        "                                can only be used in combination with -dir flag \n"
        "   [opt]-file<name>,<name>...   convert single files located in the directory specified with -dir every file must be delimitied with a ',' \n"
        "   [opt]-output <path>          path where to put the generated cache file and the folder containing the processed assets \n"
        "                                if not specified working directory will be used";
    }

    //--------------------------------------------------------------------------
    void ConvertAnimations::Run(const CommandInput& input)
    {
      if (input.HasFlag<OutputLocationFlag>() == true)
      {
        Directory location = input.GetFlagArg<OutputLocationFlag>();
        SetOutputLocation(location);
      }

      animation_pipeline_->PackageDefaultAssets();

      foundation::Vector<foundation::AnimationAsset> out;
      eastl::function<bool(const foundation::String&)> func =
        [this, &out](const foundation::Path& file)
      {
        foundation::String extension = file.GetFileExtension();
        if (extension != "fbx" && extension != "gltf")
        {
          return false;
        }

        if (animation_pipeline_->Create(file, *scene_loader_, out) == false)
        {
          PS_LOG_BUILDER(Error, "Failed to create animations from %s", file.GetString().c_str());
          return false;
        }

        for (int i = 0; i < out.size(); ++i)
        {
          if (animation_pipeline_->PackageAnimation(file, out[i]) == false)
          {
            PS_LOG_BUILDER(Error, "Failed to package animation %s", out[i].name.GetCString());
            return false;
          }

          PS_LOG_BUILDER(Info, "Succesfully packaged animation %s", out[i].name.GetCString());
        }
        return true;
      };

      if (input.HasFlag<FileFlag>() == true)
      {
        foundation::String files = input.GetFlagArg<FileFlag>();
        ProcessFiles(files, input, func);
      }
      else
      {
        ProcessFolders(input, func);
      }

      if (input.HasFlag<OutputLocationFlag>() == true)
      {
        ResetOutputLocation();
      }
    }



    //--------------------------------------------------------------------------
    ConvertScript::ConvertScript(const char* key) :
      Convert(key)
    {
      SetValidFlags<DirFlag,
        FileFlag,
        RecursiveFlag,
        OutputLocationFlag>();
      AllowMultipleOccurances<DirFlag>(true);

      HasParameter<FileFlag>(true);
      HasParameter<DirFlag>(true);
      HasParameter<OutputLocationFlag>(true);

      IsOptional<DirFlag>(true);
      IsOptional<FileFlag>(true);
      IsOptional<RecursiveFlag>(true);
      IsOptional<OutputLocationFlag>(true);
    }

    //--------------------------------------------------------------------------
    const char* ConvertScript::GetDescription() const
    {
      return "convert lua script from *.lua file \n"
        "to an engine readable format \n"
        "   [opt]-dir<path>              convert all scripts located in working directory \n"
        "   [opt]-r                      search the working directory recursivly i.e. also go through subfolders \n"
        "                                can only be used in combination with -dir flag \n"
        "   [opt]-file<name>,<name>...   convert single files located in the directory specified with -dir every file must be delimitied with a ',' \n"
        "   [opt]-output <path>          path where to put the generated cache file and the folder containing the processed assets \n"
        "                                if not specified working directory will be used";
    }

    //--------------------------------------------------------------------------
    void ConvertScript::Run(const CommandInput& input)
    {
      if (input.HasFlag<OutputLocationFlag>() == true)
      {
        Directory location = input.GetFlagArg<OutputLocationFlag>();
        SetOutputLocation(location);
      }

      script_pipeline_->PackageDefaultAssets();

      foundation::ScriptAsset out = {};
      eastl::function<bool(const foundation::String&)> func =
        [this, &out](const foundation::Path& file)
      {
        foundation::String extension = file.GetFileExtension();
        if (extension != "lua")
        {
          return false;
        }

        if (script_pipeline_->Create(file, out) == false)
        {
          PS_LOG_BUILDER(Error, "Failed to create script from %s", file.GetString().c_str());
          return false;
        }

        if (script_pipeline_->PackageScript(file, out) == false)
        {
          PS_LOG_BUILDER(Error, "Failed to package script %s", out.name.GetCString());
          return false;
        }

        PS_LOG_BUILDER(Info, "Succesfully packaged script %s", out.name.GetCString());

        return true;
      };

      if (input.HasFlag<FileFlag>() == true)
      {
        foundation::String files = input.GetFlagArg<FileFlag>();
        ProcessFiles(files, input, func);
      }
      else
      {
        ProcessFolders(input, func);
      }

      if (input.HasFlag<OutputLocationFlag>() == true)
      {
        ResetOutputLocation();
      }
    }



    //--------------------------------------------------------------------------
    ConvertAudioBank::ConvertAudioBank(const char* key) :
      Convert(key)
    {
      SetValidFlags<DirFlag,
        FileFlag,
        RecursiveFlag,
        OutputLocationFlag>();
      AllowMultipleOccurances<DirFlag>(true);

      HasParameter<FileFlag>(true);
      HasParameter<DirFlag>(true);
      HasParameter<OutputLocationFlag>(true);

      IsOptional<DirFlag>(true);
      IsOptional<FileFlag>(true);
      IsOptional<RecursiveFlag>(true);
      IsOptional<OutputLocationFlag>(true);
    }

    //--------------------------------------------------------------------------
    const char* ConvertAudioBank::GetDescription() const
    {
      return "convert audio bank from *.bank file \n"
        "to an engine readable format \n"
        "   [opt]-dir<path>              convert all audio banks located in working directory \n"
        "   [opt]-r                      search the working directory recursivly i.e. also go through subfolders \n"
        "                                can only be used in combination with -dir flag \n"
        "   [opt]-file<name>,<name>...   convert single files located in the directory specified with -dir every file must be delimitied with a ',' \n"
        "   [opt]-output <path>          path where to put the generated cache file and the folder containing the processed assets \n"
        "                                if not specified working directory will be used";
    }

    //--------------------------------------------------------------------------
    void ConvertAudioBank::Run(const CommandInput& input)
    {
      if (input.HasFlag<OutputLocationFlag>() == true)
      {
        Directory location = input.GetFlagArg<OutputLocationFlag>();
        SetOutputLocation(location);
      }

      audio_pipeline_->PackageDefaultAssets();

      foundation::AudioBankAsset out = {};
      eastl::function<bool(const foundation::String&)> func =
        [this, &out](const foundation::Path& file)
      {
        out = {};
        foundation::String extension = file.GetFileExtension();
        if (extension != "bank")
        {
          return false;
        }

        if (audio_pipeline_->Create(file, out) == false)
        {
          PS_LOG_BUILDER(Error, "Failed to create audio bank from %s", file.GetString().c_str());
          return false;
        }

        if (audio_pipeline_->PackageAudioBank(file, out) == false)
        {
          PS_LOG_BUILDER(Error, "Failed to package audio bank %s", out.name.GetCString());
          return false;
        }

        PS_LOG_BUILDER(Info, "Succesfully packaged audio bank %s", out.name.GetCString());

        return true;
      };

      if (input.HasFlag<FileFlag>() == true)
      {
        foundation::String files = input.GetFlagArg<FileFlag>();
        ProcessFiles(files, input, func);
      }
      else
      {
        ProcessFolders(input, func);
      }

      if (input.HasFlag<OutputLocationFlag>() == true)
      {
        ResetOutputLocation();
      }
    }

    //--------------------------------------------------------------------------
    Convert::Convert(const char* key) :
      ICommand(key)
    {
      SetValidFlags<DirFlag, VertexShaderFlag, PixelShaderFlag, OutputLocationFlag, FileFlag, RecursiveFlag>();
      HasParameter<DirFlag>(true);
      HasParameter<VertexShaderFlag>(true);
      HasParameter<PixelShaderFlag>(true);
      HasParameter<OutputLocationFlag>(true);
      HasParameter<FileFlag>(true);
      AllowMultipleOccurances<DirFlag>(true);
      IsOptional<RecursiveFlag>(true);
      IsOptional<FileFlag>(true);
      IsOptional<OutputLocationFlag>(true);
      IsOptional<DirFlag>(true);
    }

    //--------------------------------------------------------------------------
    void Convert::Run(const CommandInput& input)
    {
      if (input.HasFlag<OutputLocationFlag>() == true)
      {
        Directory location = input.GetFlagArg<OutputLocationFlag>();
        SetOutputLocation(location);
      }

      model_pipeline_->PackageDefaultAssets();
      mesh_pipeline_->PackageDefaultAssets();
      skeleton_pipeline_->PackageDefaultAssets();
      material_pipeline_->PackageDefaultAssets();
      texture_pipeline_->PackageDefaultAssets();
      shader_pipeline_->PackageDefaultAssets();
      animation_pipeline_->PackageDefaultAssets();
      script_pipeline_->PackageDefaultAssets();
      audio_pipeline_->PackageDefaultAssets();

      Directory asset_dir(input.GetFlagArg<DirFlag>());
      foundation::Vector<foundation::Path> files;
      if (input.HasFlag<RecursiveFlag>() == true)
      {
        files = asset_dir.GetFilesRecursive();
      }
      else
      {
        files = asset_dir.GetFiles();
      }
      foundation::Vector<foundation::String> shaders;
      foundation::Vector<foundation::String> rest;

      for (size_t i = 0; i < files.size(); ++i)
      {
        foundation::String extension = files[i].GetFileExtension();
        if (extension == "pixe" ||
          extension == "geom" ||
          extension == "vert" ||
          extension == "comp" ||
          extension == "doma" ||
          extension == "hull")
        {
          shaders.push_back(files[i].GetString());
        }
        else
        {
          rest.push_back(files[i].GetString());
        }
      }

      // compile shaders first because they must be compiled before loading a model
      ShaderPipelineOptions options;
      options.targets = static_cast<uint8_t>(ShaderCompilerBase::Target::kAll);
      options.additional_include_dirs = { "./include/" };
      foundation::ShaderAsset shader = {};
      for (size_t i = 0; i < shaders.size(); ++i)
      {
        if (shader_pipeline_->Create(shaders[i], options, shader) == false)
        {
          PS_LOG_BUILDER(Error, "Failed to create shader from %s", shaders[i].c_str());
          return;
        }

        if (shader_pipeline_->PackageShader(shaders[i], shader) == false)
        {
          PS_LOG_BUILDER(Error, "Failed to package shader %s", shader.name.GetCString());
          return;
        }

        PS_LOG_BUILDER(Info, "Successfully packaged shader %s", shader.name.GetCString());
      }

      // handle all other assets
      for (int i = 0; i < rest.size(); ++i)
      {
        foundation::String extension = files[i].GetFileExtension();

        if (extension == "obj" ||
          extension == "fbx" ||
          extension == "gtlf")
        {
          // Convert models
          foundation::ModelInfo info = model_pipeline_->GetModelInfo(*scene_loader_, rest[i], true);
          foundation::Vector<foundation::ModelAsset> models;

          if (model_pipeline_->Create(*scene_loader_, rest[i],
            true,
            info,
            *mesh_pipeline_,
            *skeleton_pipeline_,
            *material_pipeline_,
            *texture_pipeline_,
            *shader_pipeline_,
            input.GetFlagArg<VertexShaderFlag>(),
            input.GetFlagArg<PixelShaderFlag>(), models) == false)
          {
            PS_LOG_BUILDER(Error, "Failed to create models from %s", rest[i].c_str());
            return;
          }

          // Convert animations
          foundation::Vector<foundation::AnimationAsset> animations;
          if (animation_pipeline_->Create(rest[i], *scene_loader_, animations) == false)
          {
            PS_LOG_BUILDER(Error, "Failed to create animations from %s", rest[i].c_str());
            return;
          }

          // Convert skeletons
          foundation::Vector<foundation::SkeletonAsset> skeletons;
          if (skeleton_pipeline_->Create(rest[i], *scene_loader_, skeletons) == false)
          {
            PS_LOG_BUILDER(Error, "Failed to create skeletons from %s", rest[i].c_str());
            return;
          }

          // Package models
          for (foundation::ModelAsset& model : models)
          {
            if (model_pipeline_->PackageModel(rest[i],
              model, *mesh_pipeline_,
              *skeleton_pipeline_,
              *material_pipeline_,
              *texture_pipeline_) == false)
            {
              PS_LOG_BUILDER(Error, "Failed to package model %s", model.name.GetCString());
              return;
            }
            PS_LOG_BUILDER(Info, "Successfully packaged model %s", model.name.GetCString());
          }

          // Package animations
          for (foundation::AnimationAsset& animation : animations)
          {
            if (animation_pipeline_->PackageAnimation(rest[i], animation) == false)
            {
              PS_LOG_BUILDER(Error, "Failed to package animation %s", animation.name.GetCString());
              continue;
            }

            PS_LOG_BUILDER(Info, "Succesfully packaged animation %s", animation.name.GetCString());
          }

          // Package skeletons
          for (foundation::SkeletonAsset& skeleton : skeletons)
          {
            if (skeleton_pipeline_->PackageSkeleton(rest[i], skeleton) == false)
            {
              PS_LOG_BUILDER(Error, "Failed to package skeleton %s", skeleton.name.GetCString());
              continue;
            }
            PS_LOG_BUILDER(Info, "Successfully packaged skeleton %s", skeleton.name.GetCString());
          }
        }

        else if (extension == "png" ||
          extension == "jpeg" ||
          extension == "tga" ||
          extension == "bmp" ||
          extension == "dds" ||
          extension == "jpg")
        {
          foundation::TextureAsset texture;
          if (texture_pipeline_->Create(rest[i], texture) == false)
          {
            PS_LOG_BUILDER(Error, "Failed to create texture from %s", rest[i].c_str());
            return;
          }

          if (texture_pipeline_->PackageTexture(rest[i], texture) == false)
          {
            PS_LOG_BUILDER(Error, "Failed to package texture %s", texture.name.GetCString());
            return;
          }

          PS_LOG_BUILDER(Info, "Successfully packaged texture %s", texture.name.GetCString());
        }
        else if(extension == "lua")
        {
          foundation::ScriptAsset script;
          if (script_pipeline_->Create(rest[i], script) == false)
          {
            PS_LOG_BUILDER(Error, "Failed to create script from %s", rest[i].c_str());
            return;
          }

          if (script_pipeline_->PackageScript(rest[i], script) == false)
          {
            PS_LOG_BUILDER(Error, "Failed to package script %s", script.name.GetCString());
            return;
          }

          PS_LOG_BUILDER(Info, "Successfully packaged script %s", script.name.GetCString());
        }
        else if (extension == "bank")
        {
          foundation::AudioBankAsset audio_bank;
          if (audio_pipeline_->Create(rest[i], audio_bank) == false)
          {
            PS_LOG_BUILDER(Error, "Failed to create audio bank from %s", rest[i].c_str());
            return;
          }

          if (audio_pipeline_->PackageAudioBank(rest[i], audio_bank) == false)
          {
            PS_LOG_BUILDER(Error, "Failed to package audio bank %s", audio_bank.name.GetCString());
            return;
          }

          PS_LOG_BUILDER(Info, "Successfully packaged audio bank %s", audio_bank.name.GetCString());
        }
      }

      if (input.HasFlag<OutputLocationFlag>() == true)
      {
        ResetOutputLocation();
      }

      PS_LOG_BUILDER(Info, "Done");
    }

    //--------------------------------------------------------------------------
    void Convert::ProcessFolders(const CommandInput& input,
      const eastl::function<bool(const foundation::String&)>& func)
    {
      unsigned int count = input.FlagCount<DirFlag>();
      for (unsigned int i = 0; i < count; ++i)
      {
        Directory location = input.GetFlagArg<DirFlag>(i);
        if (location.Exists() == false)
        {
          PS_LOG_BUILDER(Error, "directory %s does not exist", location.path().GetString().c_str());
          continue;
        }

        foundation::Vector<foundation::Path> files;
        if (input.HasFlag<RecursiveFlag>() == true)
        {
          files = location.GetFilesRecursive();
        }
        else
        {
          files = location.GetFiles();
        }

        for (const foundation::Path& file : files)
        {
          func(file.GetString());
        }
      }
    }

    //--------------------------------------------------------------------------
    void Convert::ProcessFiles(const foundation::String& file_string,
      const CommandInput& input,
      const eastl::function<bool(const foundation::String&)>& func)
    {
      foundation::Vector<foundation::String> files;
      size_t curr_pos = 0;
      size_t div_pos = 0;

      foundation::String name;
      foundation::Vector<Directory> dirs_to_search;
      bool recursive_search = input.HasFlag<RecursiveFlag>();

      if (input.HasFlag<DirFlag>() == false)
      {
        dirs_to_search.push_back(Directory());
      }
      else
      {
        for (unsigned int i = 0; i < input.FlagCount<DirFlag>(); ++i)
        {
          dirs_to_search.push_back(input.GetFlagArg<DirFlag>(i));
          if (recursive_search == true)
          {
            foundation::Vector<Directory> sub_dirs = dirs_to_search.back().GetSubDirsRecursive();
            dirs_to_search.insert(dirs_to_search.end(), sub_dirs.begin(), sub_dirs.end());
          }
        }
      }

      while (curr_pos != file_string.npos)
      {
        div_pos = file_string.find(",", curr_pos);

        for (Directory& location : dirs_to_search)
        {
          if (div_pos == file_string.npos)
          {
            name = file_string.substr(curr_pos);
            name = name.substr(name.find_first_not_of(" ", 0));
            curr_pos = div_pos;
          }
          else
          {
            name = file_string.substr(curr_pos, div_pos - curr_pos);
            name = name.substr(name.find_first_not_of(" ", 0));
            ++div_pos >= file_string.length() ? curr_pos = file_string.npos : curr_pos = div_pos;
          }
          foundation::String full_path = location.path().GetString() + name;

          std::ifstream f(full_path.c_str());
          if (f.good() == true)
          {
            files.push_back(full_path);
            break;
          }
          else
          {
            PS_LOG_BUILDER(Error, "unable to read file %s", full_path.c_str());
          }
        }
      }

      for (const foundation::String& file : files)
      {
        func(file);
      }
    }

    //--------------------------------------------------------------------------
    void Convert::SetOutputLocation(const Directory& location)
    {
      if (location.path().GetString().empty() == true)
      {
        return;
      }

      if (location.Exists() == false)
      {
        location.Create();
      }

      Directory output_folder(location.path().GetString() + Application::package_relative_path());
      if (output_folder.Exists() == false)
      {
        output_folder.Create();
      }

      mesh_pipeline_->SetOutputLocation(location.path());
      model_pipeline_->SetOutputLocation(location.path());
      material_pipeline_->SetOutputLocation(location.path());
      texture_pipeline_->SetOutputLocation(location.path());
      shader_pipeline_->SetOutputLocation(location.path());
      skeleton_pipeline_->SetOutputLocation(location.path());
      animation_pipeline_->SetOutputLocation(location.path());
      script_pipeline_->SetOutputLocation(location.path());
      audio_pipeline_->SetOutputLocation(location.path());
    }

    void Convert::ResetOutputLocation()
    {
      mesh_pipeline_->SetOutputLocation("");
      model_pipeline_->SetOutputLocation("");
      material_pipeline_->SetOutputLocation("");
      texture_pipeline_->SetOutputLocation("");
      shader_pipeline_->SetOutputLocation("");
      skeleton_pipeline_->SetOutputLocation("");
      animation_pipeline_->SetOutputLocation("");
      script_pipeline_->SetOutputLocation("");
      audio_pipeline_->SetOutputLocation("");
    }

    //--------------------------------------------------------------------------
    const char* Convert::GetDescription() const
    {
      return "process all assets found at a specified location \n"
        "   -vertex <name>               name of vertex to be used for the models. \n"
        "                                can be a shader located in the folder that is being processed or an allready processed shader \n"
        "   -pixel <name>                name of vertex to be used for the models. \n"
        "                                can be a shader located in the folder that is being processed or an allready processed shader \n"
        "   [opt]-r                      process all files in the subdirectories as well"
        "   [opt]-dir <path>             path where the assets are located. if not specified working directory will be used \n"
        "   [opt]-output <path>          path where to put the generated cache file and the folder containing the processed assets \n"
        "                                if not specified working directory will be used. \n"
        "                                if specified it is assumed that the vertex and pixel shader specified with the -vertex and -pixel flag are compiled to caches allready located at the given output path \n";
    }
  }
}
