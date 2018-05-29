#include "tools/builder/export.h"

#include "tools/builder/pipelines/audio_pipeline.h"
#include "tools/builder/pipelines/animation_pipeline.h"
#include "tools/builder/pipelines/material_pipeline.h"
#include "tools/builder/pipelines/mesh_pipeline.h"
#include "tools/builder/pipelines/model_pipeline.h"
#include "tools/builder/pipelines/scene_loader.h"
#include "tools/builder/pipelines/script_pipeline.h"
#include "tools/builder/pipelines/shader_pipeline.h"
#include "tools/builder/pipelines/shader_pipeline_options.h"
#include "tools/builder/pipelines/skeleton_pipeline.h"
#include "tools/builder/pipelines/texture_pipeline.h"

#include "tools/builder/shared/file_system.h"
#include "tools/builder/shared/shader_compiler_base.h"

#include <foundation/pipeline-assets/audio.h>
#include <foundation/pipeline-assets/animation.h>
#include <foundation/pipeline-assets/shader.h>
#include <foundation/pipeline-assets/model_info.h>
#include <foundation/pipeline-assets/texture.h>
#include <foundation/pipeline-assets/script.h>

#include <foundation/containers/string.h>
#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace builder
  {
    namespace shared
    {
      template<typename T>
      size_t PipelineContainer::type_index<T>::index_ = PS_SIZE_MAX;

      template<typename T>
      bool PipelineContainer::type_index<T>::set_ = false;

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API void Initialize()
      {
        foundation::Memory::Initialize(1024ul * 1024ul * 100ul);
        scene_loader = new SceneLoader();

        pipelines = new PipelineContainer();
        pipelines->Initialize();

        for (PipelineBase* pipeline : pipelines->pipelines())
        {
          pipeline->Initialize();
        }
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API void Shutdown()
      {
        if (scene_loader != nullptr)
        {
          delete scene_loader;
        }

        if (pipelines != nullptr)
        {
          pipelines->Shutdown();
          delete pipelines;
        }
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool SetOutputPath(const char* path)
      {
        foundation::String path_string(path);
        for (PipelineBase* pipeline : pipelines->pipelines())
        {
          pipeline->SetOutputLocation(path_string);
        }
        return true;
      }

      //---------------------------------------------------------------------------------------------------------
      bool SetPackageOutputPath(const char* path)
      {
        foundation::String path_string(path);

        for (PipelineBase* pipeline : pipelines->pipelines())
        {
          pipeline->SetPackageOutputLocation(path_string);
        }

        return true;
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool ImportAudio(const char* path, uint64_t* id)
      {
        bool success = false;
        foundation::String path_string(path);
        foundation::AudioBankAsset out;

        AudioPipeline* pipeline = pipelines->GetPipeline<AudioPipeline>();
        success = pipeline->Create(path_string, out);
        if (success == false)
        {
          return success;
        }

        success = pipeline->PackageAudioBank(path_string, out);
        *id = out.id;
        return success;
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool ImportMaterial(const char* path, uint64_t*)
      {
        return path != nullptr;
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool ImportModel(const char* path, bool single_model,
        const char* vertex_shader, const char* pixel_shader, uint64_t* id)
      {
        bool success = false;
        foundation::String path_string(path);
        ModelPipeline* pipeline = pipelines->GetPipeline<ModelPipeline>();

        foundation::ModelInfo info = pipeline->GetModelInfo(*scene_loader,
          path_string, single_model);
        foundation::Vector<foundation::ModelAsset> models;
        success = pipeline->Create(*scene_loader,
          path,
          single_model,
          info,
          *pipelines->GetPipeline<MeshPipeline>(),
          *pipelines->GetPipeline<SkeletonPipeline>(),
          *pipelines->GetPipeline<MaterialPipeline>(),
          *pipelines->GetPipeline<TexturePipeline>(),
          *pipelines->GetPipeline<ShaderPipeline>(),
          vertex_shader,
          pixel_shader,
          models);

        if (success == false)
        {
          return success;
        }

        for (foundation::ModelAsset& model : models)
        {
          success = pipeline->PackageModel(path_string,
            model,
            *pipelines->GetPipeline<MeshPipeline>(),
            *pipelines->GetPipeline<SkeletonPipeline>(),
            *pipelines->GetPipeline<MaterialPipeline>(),
            *pipelines->GetPipeline<TexturePipeline>());
          *id = model.id;
        }
        return success;
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool ImportScript(const char* path, uint64_t* id)
      {
        bool success = false;
        foundation::String path_string(path);
        foundation::ScriptAsset out = {};
        ScriptPipeline* pipeline = pipelines->GetPipeline<ScriptPipeline>();

        success = pipeline->Create(path_string, out);
        if (success == false)
        {
          return success;
        }

        success = pipeline->PackageScript(path_string, out);
        *id = out.id;
        pipeline->ExportCache();
        return success;
      }
      
      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool ImportShader(const char* path, uint64_t* id)
      {
        bool success = false;
        foundation::String path_string(path);
        ShaderPipelineOptions options;
        options.targets = static_cast<uint8_t>(ShaderCompilerBase::Target::kAll);
        foundation::ShaderAsset out;
        ShaderPipeline* pipeline = pipelines->GetPipeline<ShaderPipeline>();

        success = pipeline->Create(path_string, options, out);
        if (success == false)
        {
          return success;
        }

        success = pipeline->PackageShader(path_string, out);
        foundation::PackagePtr native_ptr;
        pipeline->GetPackagePtrById(out.id, native_ptr);
        *id = out.id;
        return success;
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool ImportTexture(const char* path, uint64_t* id)
      {
        bool success = false;
        foundation::String path_string(path);
        foundation::TextureAsset out;
        TexturePipeline* pipeline = pipelines->GetPipeline<TexturePipeline>();

        success = pipeline->Create(path_string, out);
        if (success == false)
        {
          return success;
        }

        success = pipeline->PackageTexture(path_string, out);
        *id = out.id;
        return success;
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool DeleteAudio(uint64_t id)
      {
        return pipelines->GetPipeline<AudioPipeline>()->DeleteAsset(id);
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool DeleteAnimation(uint64_t id)
      {
        return pipelines->GetPipeline<AnimationPipeline>()->DeleteAsset(id);
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool DeleteMaterial(uint64_t id)
      {
        return pipelines->GetPipeline<MaterialPipeline>()->DeleteAsset(id);
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool DeleteMesh(uint64_t id)
      {
        return pipelines->GetPipeline<MeshPipeline>()->DeleteAsset(id);
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool DeleteModel(uint64_t id)
      {
        return pipelines->GetPipeline<ModelPipeline>()->DeleteAsset(id);
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool DeleteScript(uint64_t id)
      {
        return pipelines->GetPipeline<ScriptPipeline>()->DeleteAsset(id);
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool DeleteShader(uint64_t id)
      {
        return pipelines->GetPipeline<ShaderPipeline>()->DeleteAsset(id);
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool DeleteSkeleton(uint64_t id)
      {
        return pipelines->GetPipeline<SkeletonPipeline>()->DeleteAsset(id);
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool DeleteTexture(uint64_t id)
      {
        return pipelines->GetPipeline<TexturePipeline>()->DeleteAsset(id);
      }

      //---------------------------------------------------------------------------------------------------------
      PS_BUILDER_C_API bool UpdatePackagePtrs(const char**, uint64_t*, int32_t* asset_types, int32_t count)
      {
        for (int i = 0; i < count; ++i)
        {
          switch (asset_types[i])
          {
          case 3: //kShader
            // update the pointer
          default:
            break;
          }
        }
        return false;
      }

      PS_BUILDER_C_API void CreateDefaults()
      {
        for (PipelineBase* pipeline : pipelines->pipelines())
        {
          pipeline->PackageDefaultAssets();
          pipeline->ExportCache();
        }
      }

      PS_BUILDER_C_API bool GetAssetPackageName(int32_t type, char* out_buf, int32_t buffer_size)
      {
        foundation::String name = "";
        switch (type)
        {
        case 0: // model
          name = pipelines->GetPipeline<ModelPipeline>()->GetCacheName();
          break;
        case 1: // texture
          name = pipelines->GetPipeline<TexturePipeline>()->GetCacheName();
          break;
        case 2: // shader
          name = pipelines->GetPipeline<ShaderPipeline>()->GetCacheName();
          break;
        case 3: // material
          name = pipelines->GetPipeline<MaterialPipeline>()->GetCacheName();
          break;
        case 4: // mesh
          name = pipelines->GetPipeline<MeshPipeline>()->GetCacheName();
          break;
        case 5: // audio
          name = pipelines->GetPipeline<AudioPipeline>()->GetCacheName();
          break;
        case 6: // animation
          name = pipelines->GetPipeline<AnimationPipeline>()->GetCacheName();
          break; 
        case 7: // script
          name = pipelines->GetPipeline<ScriptPipeline>()->GetCacheName();
          break;
        case 8: // skeleton
          name = pipelines->GetPipeline<SkeletonPipeline>()->GetCacheName();
          break;
        default:
          return false;
        }

        if (name.length() < buffer_size)
        {
          strncpy_s(out_buf, buffer_size, name.c_str(),name.length());
          return true;
        }
        return false;
      }

      void PipelineContainer::Shutdown()
      {
        delete GetPipeline<AnimationPipeline>();
        delete GetPipeline<AudioPipeline>();
        delete GetPipeline<MaterialPipeline>();
        delete GetPipeline<MeshPipeline>();
        delete GetPipeline<ModelPipeline>();
        delete GetPipeline<ScriptPipeline>();
        delete GetPipeline<ShaderPipeline>();
        delete GetPipeline<SkeletonPipeline>();
        delete GetPipeline<TexturePipeline>();
        pipelines_.clear();
      }
    }
  }
}
