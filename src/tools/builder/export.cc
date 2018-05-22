#include "tools/builder/export.h"

#include "tools/builder/pipelines/material_pipeline.h"
#include "tools/builder/pipelines/mesh_pipeline.h"
#include "tools/builder/pipelines/model_pipeline.h"
#include "tools/builder/pipelines/shader_pipeline.h"
#include "tools/builder/pipelines/shader_pipeline_options.h"
#include "tools/builder/shared/shader_compiler_base.h"
#include "tools/builder/pipelines/texture_pipeline.h"
#include "tools/builder/shared/file_system.h"

#include <foundation/pipeline-assets/shader.h>
#include <foundation/pipeline-assets/model_info.h>
#include <foundation/pipeline-assets/texture.h>
#include <foundation/containers/string.h>
#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace builder
  {
    namespace shared
    {
      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API void Initialize()
      {
        foundation::Memory::Initialize(1024ul * 1024ul * 100ul);

        material_pipeline = new MaterialPipeline();
        mesh_pipeline = new MeshPipeline();
        model_pipeline = new ModelPipeline();
        shader_pipeline = new ShaderPipeline();
        texture_pipeline = new TexturePipeline();

        material_pipeline->Initialize();
        mesh_pipeline->Initialize();
        model_pipeline->Initialize();
        shader_pipeline->Initialize();
        texture_pipeline->Initialize();
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API void Shutdown()
      {
        delete material_pipeline;
        delete mesh_pipeline;
        delete model_pipeline;
        delete shader_pipeline;
        delete texture_pipeline;
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool SetOutputPath(const char* path)
      {
        foundation::String path_string(path);

        material_pipeline->SetOutputLocation(path_string);
        mesh_pipeline->SetOutputLocation(path_string);
        model_pipeline->SetOutputLocation(path_string);
        shader_pipeline->SetOutputLocation(path_string);
        texture_pipeline->SetOutputLocation(path_string);

        return true;
      }

      //---------------------------------------------------------------------------------------------------------
      bool SetPackageOutputPath(const char* path)
      {
        foundation::String path_string(path);

        material_pipeline->SetPackageOutputLocation(path_string);
        mesh_pipeline->SetPackageOutputLocation(path_string);
        model_pipeline->SetPackageOutputLocation(path_string);
        shader_pipeline->SetPackageOutputLocation(path_string);
        texture_pipeline->SetPackageOutputLocation(path_string);

        return true;
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool ImportModel(const char* path, bool single_model, 
        const char* vertex_shader, const char* pixel_shader)
      {
        bool success = false;
        foundation::String path_string(path);
        foundation::ModelInfo info = model_pipeline->GetModelInfo(path_string, single_model);
        foundation::Vector<foundation::ModelAsset> models;
        success = model_pipeline->Create(path,
                                         single_model,
                                         info,
                                         *mesh_pipeline,
                                         *material_pipeline,
                                         *texture_pipeline,
                                         *shader_pipeline, 
                                         vertex_shader, 
                                         pixel_shader,
                                         models);

        if (success == false)
        {
          return success;
        }
        
        for (foundation::ModelAsset& model : models)
        {
          success = model_pipeline->PackageModel(path_string,
                                                 model,
                                                 *mesh_pipeline,
                                                 *material_pipeline,
                                                 *texture_pipeline);
        }
        return success;
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool ImportMaterial(const char* path)
      {
        return path != nullptr;
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool ImportShader(const char* path)
      {
        bool success = false;
        foundation::String path_string(path);
        ShaderPipelineOptions options;
        Directory dir("./include/");


        options.additional_include_dirs.push_back(dir);
        options.targets = static_cast<uint8_t>(ShaderCompilerBase::Target::kAll);
        foundation::ShaderAsset out;
        success = shader_pipeline->Create(path_string, options, out);
        if (success == false)
        {
          return success;
        }

        success = shader_pipeline->PackageShader(path_string, out);
        return success;
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool ImportTexture(const char* path)
      {
        bool success = false;
        foundation::String path_string(path);
        foundation::TextureAsset out;

        success = texture_pipeline->Create(path_string, out);
        if (success == false)
        {
          return success;
        }

        success = texture_pipeline->PackageTexture(path_string, out);
        return success;
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool DeleteModel(uint64_t id)
      {
        return model_pipeline->DeleteAsset(id);
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool DeleteMaterial(uint64_t id)
      {
        return material_pipeline->DeleteAsset(id);
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool DeleteShader(uint64_t id)
      {
        return shader_pipeline->DeleteAsset(id);
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool DeleteTexture(uint64_t id)
      {
        return texture_pipeline->DeleteAsset(id);
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool DeleteMesh(uint64_t id)
      {
        return mesh_pipeline->DeleteAsset(id);
      }
    }
  }
}