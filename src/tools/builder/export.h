#pragma once
#include <cstdint>
#include <foundation/containers/vector.h>
/**
* @def PS_BUILDER_C_API
* @brief exporting macro for dll using extern C
*/
#define PS_BUILDER_C_API extern "C" __declspec(dllexport)

namespace sulphur
{
  namespace foundation
  {
    using AssetID = uint64_t;
  }

  namespace builder
  {
    class PipelineBase;
    class AnimationPipeline;
    class AudioPipeline;
    class MaterialPipeline;
    class MeshPipeline;
    class ModelPipeline;
    class SceneLoader;
    class ScriptPipeline;
    class ShaderPipeline;
    class SkeletonPipeline;
    class TexturePipeline;
    class WorldPipeline;

    namespace shared
    {
      /**
      * @class sulphur.builder.shared.PipelineContainer
      * @brief Class that handles all available pipelines. You can either get all pipelines as a list of PipelineBase pointers or by their derived type.
      *        There will always be only 1 instance of a certain pipeline type.
      * @author Stan Pepels
      */
      class PipelineContainer
      {
      public:
        /**
        * @brief Creates all available pipelines.
        * @remarks This function must be called before using any of the pipelines.
        */
        void Initialize()
        {
          CreatePipeline<AnimationPipeline>();
          CreatePipeline<AudioPipeline>();
          CreatePipeline<MaterialPipeline>();
          CreatePipeline<MeshPipeline>();
          CreatePipeline<ModelPipeline>();
          CreatePipeline<ScriptPipeline>();
          CreatePipeline<ShaderPipeline>();
          CreatePipeline<SkeletonPipeline>();
          CreatePipeline<TexturePipeline>();
          CreatePipeline<WorldPipeline>();
        }

        /**
        * @brief Gets the pipeline of a specific type
        * @return (T*) Pointer to the pipeline of type T. If the pipeline does not exists a nullptr is returned.
        */
        template<typename T>
        T* GetPipeline()
        {
          size_t index = type_index<T>::index();
          return index < pipelines_.size() ? (T*)pipelines_[index] : nullptr;
        }

        /**
        *@brief Gets the list of pipelines as base class pointers.
        *@rerurn (sulphur.foundation::Vector<sulphur.builder.PipelineBase*>&) Vector of pipeline base pointers.
        */
        const foundation::Vector<PipelineBase*>& pipelines()
        {
          return pipelines_;
        }

        /**
        * @brief Destroys all pipelines. 
        */
        void Shutdown();

 
      private:

        /**
        *@brief Creates a pipeline of type T and adds it to the list.
        */
        template<typename T>
        void CreatePipeline()
        {
          if (type_index<T>::set() == true)
          {
            return;
          }

          pipelines_.push_back(new T());
          type_index<T>::set_index(pipelines_.size() - 1);
        }

        /**
        *@class sulphur.builder.shared.type_index
        *@brief Helper class used to keep tracks of what pipeline type is at a certin index in the asset pipeline list.
        *@remark For internal use only.
        */
        template<typename T>
        class type_index
        {
        public:
          /**
          *@brief Sets the index for a pipeline type.
          *@param[in] idx (size_t) Index of the pipeline type in the list.
          *@remark If called multiple times only the first call will set the index. after that the index can not be changed.
          */
          static void set_index(size_t idx)
          {
            if (set_ == false)
            {
              set_ = true;
              index_ = idx;
            }
          }

          /**
          * @brief gets the index.
          * @return (size_t) index of the pipeline in the pipeline list.
          */
          static size_t index()
          {
            return index_;
          }

          /**
          *@brief Check if the value has been set.
          *@return (bool) True if index was set. False otherwise.
          */
          static bool set()
          {
            return set_;
          }
        private:
          static bool set_; //!< Value for checking if the index for the type T was set
          static size_t index_; //!< index of the pipeline of type T in the pipeline list.
        };

        foundation::Vector<PipelineBase*> pipelines_; //!< Pipelines in this container.
      };

      PipelineContainer* pipelines; //!< Container used to hold all pipelines.

      static SceneLoader* scene_loader = nullptr;             //!< Scene loader used for loading scenes
  
      /**
      * @brief initializes all the pipeline and readies them for use
      */
      PS_BUILDER_C_API void Initialize();

      /**
      * @brief Set the pipelines project directory.
      */
      PS_BUILDER_C_API void SetProjectDirectory(const char* project_dir);

      /**
      * @brief frees resources used by all different pipelines
      */
      PS_BUILDER_C_API void Shutdown();

      /**
      * @brief sets the output location for the pipeline caches
      * @param[in] path (const char*) new folder to output to
      * @return (bool) success return value
      */
      PS_BUILDER_C_API bool SetOutputPath(const char* path);

      /**
      * @brief sets the relative output location for the generated packages
      * @param[in] path (const char*) new folder to output to. Relative to the output path.
      * @return (bool) success return value
      */
      PS_BUILDER_C_API bool SetPackageOutputPath(const char* path);

      /**
      * @brief imports an audio file and packages it for use by the engine and editor
      * @param[in] path (const char*) the absolute or relative path to the file to be imported
      * @return (bool) success return value
      * @remark supported formats: ?
      */
      PS_BUILDER_C_API bool ImportAudio(const char* path, uint64_t* id);

      /**
      * @brief imports a material and packages it for use by the engine and editor
      * @param[in] path (const char*) the absolute or relative path to the file to be imported
      * @return (bool) success return value
      */
      PS_BUILDER_C_API bool ImportMaterial(const char* path, uint64_t* id);

      PS_BUILDER_C_API bool RegisterWorld(const char* path, uint64_t* id);

      /**
      * @brief imports a model and packages it for use by the engine and editor
      * @param[in] path (const char*) the absolute or relative path to the file to be imported
      * @param[in] single_model (bool) If the scene should be interpreted as one model
      * @param[in] vertex_shader (const char*) the name of the vertex shader to use to create materials
      * @param[in] pixel_shader (const char*) the name of the pixel shader to use to create materials
      * @return (bool) success return value
      * @remark supported formats: obj, fbx, gltf
      */
      PS_BUILDER_C_API bool ImportModel(const char* path, bool single_model,
        const char* vertex_shader, const char* pixel_shader, uint64_t* id);

      /**
      * @brief imports a script and packages it for use by the engine and editor
      * @param[in] path (const char*) the absolute or relative path to the file to be imported
      * @return (bool) success return value
      * @remark supported formats: lua
      */
      PS_BUILDER_C_API bool ImportScript(const char* path, uint64_t* id);

      /**
      * @brief imports a shader and packages it for use by the engine and editor
      * @param[in] path (const char*) the absolute or relative path to the file to be imported
      * @param[out] id (uint64_t) The id if the packaged asset.
      * @return (bool) success return value
      * @remark supported formats: hlsl
      */
      PS_BUILDER_C_API bool ImportShader(const char* path, uint64_t* id);

      /**
      * @brief imports a texture and packages it for use by the engine and editor
      * @param[in] path (const char*) the absolute or relative path to the file to be imported
      * @return (bool) success return value
      * @remark supported formats: png, jpeg, bmp
      */
      PS_BUILDER_C_API bool ImportTexture(const char* path, uint64_t* id);

      /**
      * @brief removes an animation asset from the asset cache
      * @param[in] id (int) name of the asset
      * @return (bool) success return value
      */
      PS_BUILDER_C_API bool DeleteAnimation(uint64_t path);

      /**
      * @brief removes an audio asset from the asset cache
      * @param[in] id (int) name of the asset
      * @return (bool) success return value
      */
      PS_BUILDER_C_API bool DeleteAudio(uint64_t path);

      /**
      * @brief removes a material asset from the asset cache
      * @param[in] id (int) name of the asset
      * @return (bool) success return value
      */
      PS_BUILDER_C_API bool DeleteMaterial(uint64_t id);

      /**
      * @brief removes a mesh asset from the asset cache
      * @param[in] id (int) name of the asset
      * @return (bool) success return value
      */
      PS_BUILDER_C_API bool DeleteMesh(uint64_t id);

      /**
      * @brief removes an asset from the asset cache
      * @param[in] id (int) name of the asset
      * @return (bool) success return value
      */
      PS_BUILDER_C_API bool DeleteModel(uint64_t id);

      /**
      * @brief removes a script asset from the asset cache
      * @param[in] id (int) name of the asset
      * @return (bool) success return value
      */
      PS_BUILDER_C_API bool DeleteScript(uint64_t path);

      /**
      * @brief removes a Shader asset from the asset cache
      * @param[in] id (int) name of the asset
      * @return (bool) success return value
      */
      PS_BUILDER_C_API bool DeleteShader(uint64_t id);

      /**
      * @brief removes a skeleton asset from the asset cache
      * @param[in] id (int) name of the asset
      * @return (bool) success return value
      */
      PS_BUILDER_C_API bool DeleteSkeleton(uint64_t path);

      /**
      * @brief removes a texture asset from the asset cache
      * @param[in] id (int) name of the asset
      * @return (bool) success return value
      */
      PS_BUILDER_C_API bool DeleteTexture(uint64_t id);

      /**
      * @brief Updates the cache package pointers
      * @param[in] new_origin (const char**) Array of path values that point to the new loacation of the raw assets associated with a packaged asset.
      * @param[in] asset_ids (uint64_t*) Array of ids of the assets whose origin asset has changed location.
      * @param[in] asset_types (uint32_t*) Array of asset types of the assets whose origin asset has changed location.
      * @param[in] count (int32_t) Size of the arrays.
      */
      PS_BUILDER_C_API bool UpdatePackagePtrs(const char** new_origin, uint64_t* asset_ids, int32_t* asset_types, int32_t count);

      /**
      *@brief Generates the default cahces and assets.
      */
      PS_BUILDER_C_API void CreateDefaults();

      /**
      *@brief Gets the cache name of the cache an asset of a specified type is stored in.
      *@param[in] asset_type (int32_t) Type of the asset cache.
      *@param[out] out_buf (char*) Buffer used to store the name of the cache in.
      *@param[in] buffer_size (int32_t) Size of the buffer used to store the name in.
      */
      PS_BUILDER_C_API bool GetAssetPackageName(int32_t asset_type, char* out_buf, int32_t buffer_size);
    }
  }
}