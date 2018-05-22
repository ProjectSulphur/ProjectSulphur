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
#include "tools/builder/shared/shader_compiler_base.h"

#include <foundation/pipeline-assets/model_info.h>
#include <foundation/pipeline-assets/shader.h>
#include <foundation/pipeline-assets/texture.h>
#include <fstream>

namespace sulphur 
{
  namespace builder 
  {
    //--------------------------------------------------------------------------
    ConvertModels::ConvertModels(const char* key, ModelPipeline* model_pipeline,
      MeshPipeline* mesh_pipeline, MaterialPipeline* material_pipeline,
      TexturePipeline* texture_pipeline, ShaderPipeline* shader_pipeline) :
      Convert(key,
              model_pipeline,
              mesh_pipeline,
              material_pipeline,
              texture_pipeline,
              shader_pipeline)
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

      eastl::function<bool(const foundation::String&)> func =
        [this, &input](const foundation::String& file)
      {
        foundation::String extension = file.substr(file.find_last_of("."));
        char8_t(*to_lower_case)(char in) = eastl::CharToLower;
        eastl::transform(extension.begin(), extension.end(), extension.begin(), to_lower_case);
        if (extension != ".obj" && extension != ".fbx" && extension != ".gltf")
        {
          PS_LOG(Error, "Only OBJ, FBX and GLTF model formats are supported.")
            return false;
        }

        foundation::ModelInfo info = 
          model_pipeline_->GetModelInfo(file.c_str(), input.HasFlag<SingleFlag>());

        foundation::Vector<foundation::ModelAsset> models;
        if (model_pipeline_->Create(file.c_str(),
                                    input.HasFlag<SingleFlag>(),
                                    info, *mesh_pipeline_,
                                    *material_pipeline_,
                                    *texture_pipeline_,
                                    *shader_pipeline_,
                                    input.GetFlagArg<VertexShaderFlag>(),
                                    input.GetFlagArg<PixelShaderFlag>(),
                                    models) == false)
        {
          PS_LOG(Error, "Unable to create models.");
          return false;
        }

        for (foundation::ModelAsset& model : models)
        {
          if (model_pipeline_->PackageModel(file,
                                            model,
                                            *mesh_pipeline_,
                                            *material_pipeline_, 
                                            *texture_pipeline_) == false)
          {
            PS_LOG(Error, "unable to package model %s\n", model.name.GetCString());
            continue;
          }

          PS_LOG(Info, "Model %s packaged succesfully.\n", model.name.GetCString());
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
        "   -dir<path>                   convert all models at the specified path. multiple flags can be specified. \n"
        "   [opt]-file<name>,<name>...   convert single files located in the directory specified with -dir every file must be delimitied with a ',' \n"
        "   [opt]-single                 forces the model to be interpreted as a single mesh \n"
        "                                if not specified -file must be specified. cannot be combined with -file flag \n"
        "   [opt]-r                      search the directory specified with -dir flag recursivly i.e. also go through subfolders \n"
        "   [opt]-output <path>          path where to put the generated cache file and the folder containing the processed assets \n"
        "                                if not specified working directory will be used \n"
        "                                if specified it is assumed that the vertex and pixel shader specified with the -vertex and -pixel flag are compiled to caches allready located at the given output path \n";
    }

    //--------------------------------------------------------------------------
    CompileShaders::CompileShaders(const char* key, ModelPipeline* model_pipeline,
      MeshPipeline* mesh_pipeline, MaterialPipeline* material_pipeline,
      TexturePipeline* texture_pipeline, ShaderPipeline* shader_pipeline) :
      Convert(key,
        model_pipeline,
        mesh_pipeline,
        material_pipeline,
        texture_pipeline,
        shader_pipeline)
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

      eastl::function<bool(const foundation::String&)> func =
        [this, &options](const foundation::String& file)
      {
        foundation::ShaderAsset shader = {};
        if (shader_pipeline_->Create(file, options, shader) == false)
        {
          PS_LOG(Error, "unable to create shader from %s\n", file.c_str());
          return false;
        }

        if (shader_pipeline_->PackageShader(file, shader) == false)
        {
          PS_LOG(Error, "unable to package shader %s\n", file.c_str());
          return false;
        }

        PS_LOG(Info, "Shader %s packaged succesfully.\n", shader.name.GetCString());
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
        "   -dir<path>                   convert all shaders located in working directory \n"
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
    ConvertTextures::ConvertTextures(const char* key, ModelPipeline* model_pipeline,
      MeshPipeline* mesh_pipeline, MaterialPipeline* material_pipeline,
      TexturePipeline* texture_pipeline, ShaderPipeline* shader_pipeline) :
      Convert(key,
        model_pipeline,
        mesh_pipeline,
        material_pipeline,
        texture_pipeline,
        shader_pipeline)
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

      foundation::TextureAsset out;
      eastl::function<bool(const foundation::String&)> func =
        [this, &out](const foundation::String& file)
      {
        if (texture_pipeline_->Create(file, out) == false)
        {
          PS_LOG(Error, "unable to create texture\n");
          return false;
        }

        if (texture_pipeline_->PackageTexture(file, out) == false)
        {
          PS_LOG(Error, "unable to package texture\n");
          return false;
        }

        PS_LOG(Info, "Texture %s packaged succesfully.\n", out.name.GetCString());
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
      return "convert textures from *png, *.jpeg, *.tga, *.bmp, *.psd, *.gif, *.hdr, *.pic, *.pnm \n"
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
    Convert::Convert(const char* key, ModelPipeline* model_pipeline,
      MeshPipeline* mesh_pipeline, MaterialPipeline* material_pipeline,
      TexturePipeline* texture_pipeline, ShaderPipeline* shader_pipeline) :
      ICommand(key),
      model_pipeline_(model_pipeline),
      mesh_pipeline_(mesh_pipeline),
      material_pipeline_(material_pipeline),
      texture_pipeline_(texture_pipeline),
      shader_pipeline_(shader_pipeline)
    {
      SetValidFlags<DirFlag, VertexShaderFlag, PixelShaderFlag, OutputLocationFlag, FileFlag>();
      HasParameter<DirFlag>(true);
      HasParameter<VertexShaderFlag>(true);
      HasParameter<PixelShaderFlag>(true);
      HasParameter<OutputLocationFlag>(true);
      HasParameter<FileFlag>(true);
      AllowMultipleOccurances<DirFlag>(true);
      IsOptional<FileFlag>(true);
      IsOptional<OutputLocationFlag>(true);
      IsOptional<DirFlag>(true);
    }

    //--------------------------------------------------------------------------
    void Convert::Run(const CommandInput& input)
    {
      if (input.HasFlag<DirFlag>() == false)
      {
        return;
      }

      if (input.HasFlag<OutputLocationFlag>() == true)
      {
        Directory location = input.GetFlagArg<OutputLocationFlag>();
        SetOutputLocation(location);
      }

      Directory asset_dir(input.GetFlagArg<DirFlag>());
      foundation::Vector<foundation::String> files = asset_dir.GetFilesRecursive();
      foundation::Vector<foundation::String> shaders;
      foundation::Vector<foundation::String> rest;

      char8_t(*to_lower_case)(char in) = eastl::CharToLower;
      for (size_t i = 0; i < files.size(); ++i)
      {
        foundation::String extension = files[i].substr(files[i].find_last_of("."));
        eastl::transform(extension.begin(), extension.end(), extension.begin(), to_lower_case);
        if (extension == ".pixe" ||
          extension == ".geom" ||
          extension == ".vert" ||
          extension == ".comp" ||
          extension == ".doma" ||
          extension == ".hull")
        {
          shaders.push_back(files[i]);
        }
        else
        {
          rest.push_back(files[i]);
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
          PS_LOG(Error, "unable to create shader");
          return;
        }

        if (shader_pipeline_->PackageShader(shaders[i], shader) == false)
        {
          PS_LOG(Error, "unable to package shader");
          return;
        }

        PS_LOG(Info, "successfully packaged shader %s", shaders[i].c_str());
      }

      // handle all other assets
      for (int i = 0; i < rest.size(); ++i)
      {
        foundation::String extension = files[i].substr(files[i].find_last_of("."));
        eastl::transform(extension.begin(), extension.end(), extension.begin(), to_lower_case);

        if (extension == ".obj" ||
          extension == ".fbx" ||
          extension == ".gtlf")
        {
          foundation::ModelInfo info = model_pipeline_->GetModelInfo(rest[i], true);
          foundation::Vector<foundation::ModelAsset> models;

          if (model_pipeline_->Create(rest[i],
            true,
            info,
            *mesh_pipeline_,
            *material_pipeline_,
            *texture_pipeline_,
            *shader_pipeline_,
            input.GetFlagArg<VertexShaderFlag>(),
            input.GetFlagArg<PixelShaderFlag>(), models) == false)
          {
            PS_LOG(Error, "unable to create model %s", rest[i].c_str());
            return;
          }

          for (foundation::ModelAsset& model : models)
          {
            if (model_pipeline_->PackageModel(rest[i],
                                              model, *mesh_pipeline_,
                                              *material_pipeline_, 
                                              *texture_pipeline_) == false)
            {
              PS_LOG(Error, "unable to package model %s", rest[i].c_str());
              return;
            }
          }
          PS_LOG(Info, "successfully packaged model %s", rest[i].c_str());

        }

        else if (extension == ".png" ||
          extension == ".jpeg" ||
          extension == ".tga" ||
          extension == ".bmp" ||
          extension == ".psd" ||
          extension == ".gif" ||
          extension == ".hdr" ||
          extension == ".pic" ||
          extension == ".pnm" ||
          extension == ".dds" ||
          extension == ".jpg")
        {
          foundation::TextureAsset texture;
          if (texture_pipeline_->Create(rest[i], texture) == false)
          {
            PS_LOG(Error, "unable to create texture %s", rest[i].c_str());
            return;
          }

          if (texture_pipeline_->PackageTexture(rest[i], texture) == false)
          {
            PS_LOG(Error, "unable to package texture %s", rest[i].c_str());
            return;
          }

          PS_LOG(Info, "successfully packaged texture %s", rest[i].c_str());
        }
      }

      if (input.HasFlag<OutputLocationFlag>() == true)
      {
        ResetOutputLocation();
      }

      PS_LOG(Info, "Done \n");
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
          PS_LOG(Error, "directory %s does not exist", location.path().c_str());
          continue;
        }

        foundation::Vector<foundation::String> files;
        if (input.HasFlag<RecursiveFlag>() == true)
        {
          files = location.GetFilesRecursive();
        }
        else
        {
          files = location.GetFiles();
        }

        for (const foundation::String& file : files)
        {
          func(file);
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
      for (unsigned int i = 0; i < input.FlagCount<DirFlag>(); ++i)
      {
        dirs_to_search.push_back(input.GetFlagArg<DirFlag>(i));
        if (recursive_search == true)
        {
          foundation::Vector<Directory> sub_dirs = dirs_to_search.back().GetSubDirsRecursive();
          dirs_to_search.insert(dirs_to_search.end(), sub_dirs.begin(), sub_dirs.end());
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
          foundation::String full_path = location.path() + name;

          std::ifstream f(full_path.c_str());
          if (f.good() == true)
          {
            files.push_back(full_path);
            break;
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
      if (location.path().empty() == true)
      {
        return;
      }

      if (location.Exists() == false)
      {
        location.Create();
      }

      Directory output_folder(location.path() + Application::package_relative_path());
      if (output_folder.Exists() == false)
      {
        output_folder.Create();
      }

      mesh_pipeline_->SetOutputLocation(location.path());
      model_pipeline_->SetOutputLocation(location.path());
      material_pipeline_->SetOutputLocation(location.path());
      texture_pipeline_->SetOutputLocation(location.path());
      shader_pipeline_->SetOutputLocation(location.path());
    }

    void Convert::ResetOutputLocation()
    {
      mesh_pipeline_->SetOutputLocation("");
      model_pipeline_->SetOutputLocation("");
      material_pipeline_->SetOutputLocation("");
      texture_pipeline_->SetOutputLocation("");
      shader_pipeline_->SetOutputLocation("");
    }

    //--------------------------------------------------------------------------
    const char* Convert::GetDescription() const
    {
      return "process all assets found at a specified location \n"
        "   -vertex <name>               name of vertex to be used for the models. \n"
        "                                can be a shader located in the folder that is being processed or an allready processed shader \n"
        "   -pixel <name>                name of vertex to be used for the models. \n"
        "                                can be a shader located in the folder that is being processed or an allready processed shader \n"
        "   [opt]-dir <path>             path where the assets are located. if not specified working directory will be used \n"
        "   [opt]-output <path>          path where to put the generated cache file and the folder containing the processed assets \n"
        "                                if not specified working directory will be used. \n"
        "                                if specified it is assumed that the vertex and pixel shader specified with the -vertex and -pixel flag are compiled to caches allready located at the given output path \n";
    }
  }
}
