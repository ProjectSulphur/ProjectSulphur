#include "tools/builder/pipelines/scene_loader.h"
#include "tools/builder/base/logger.h"
#include <assimp/postprocess.h>

namespace sulphur 
{
  namespace builder 
  {
    //--------------------------------------------------------------------------------
    const aiScene* SceneLoader::LoadScene(const foundation::Path& file)
    {
      if (file == last_file_loaded_)
      {
        return GetScene();
      }

      const aiScene* ai_scene = importer_.ReadFile(file.GetString().c_str(),
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices |
        aiProcess_LimitBoneWeights |
        aiProcess_RemoveRedundantMaterials |
        aiProcess_Triangulate |
        aiProcess_SortByPType |
        aiProcess_ImproveCacheLocality);

      if (ai_scene == nullptr)
      {
        PS_LOG_BUILDER(Error,
          "Assimp: %s", importer_.GetErrorString());
        return nullptr;
      }

      last_file_loaded_ = file;

      foundation::String extension = file.GetFileExtension();
      if (extension == "fbx")
      {
        model_file_type_ = ModelFileType::kFBX;
      }
      else if (extension == "obj")
      {
        model_file_type_ = ModelFileType::kOBJ;
      }
      else if (extension == "gltf")
      {
        model_file_type_ = ModelFileType::kglTF;
      }
      else
      {
        model_file_type_ = ModelFileType::kUnknown;
      }

      return GetScene();
    }

    //--------------------------------------------------------------------------------
    const aiScene* SceneLoader::GetScene() const
    {
      return importer_.GetScene();
    }

    //--------------------------------------------------------------------------------
    const foundation::Path& SceneLoader::GetLastFileLoaded() const
    {
      return last_file_loaded_;
    }

    //--------------------------------------------------------------------------------
    ModelFileType SceneLoader::GetModelFileType() const
    {
      return model_file_type_;
    }
  }
}
