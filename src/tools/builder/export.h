#pragma once
#include <cstdint>
#
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
    

    class MaterialPipeline;
    class MeshPipeline;
    class ModelPipeline;
    class ShaderPipeline;
    class TexturePipeline;

    namespace shared
    {

      static MaterialPipeline* material_pipeline = nullptr;  //!< material pipeline used for importing materials
      static MeshPipeline* mesh_pipeline = nullptr;          //!< mesh pipeline used for importing meshes
      static ModelPipeline* model_pipeline = nullptr;        //!< model pipeline used for importing models
      static ShaderPipeline* shader_pipeline = nullptr;      //!< shader pipeline used for importing hlsl shaders
      static TexturePipeline* texture_pipeline = nullptr;    //!< texture pipeline used for importing textures
      
      /**
      * @brief initializes all the pipeline and readies them for use
      */
      PS_BUILDER_C_API void Initialize();

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
      * @brief imports a model and packages it for use by the engine and editor
      * @param[in] path (const char*) the absolute or relative path to the file to be imported
      * @param[in] single_model (bool) If the scene should be interpreted as one model
      * @param[in] vertex_shader (const char*) the name of the vertex shader to use to create materials
      * @param[in] pixel_shader (const char*) the name of the pixel shader to use to create materials
      * @return (bool) success return value
      * @remark supported formats: obj, fbx, gltf
      */
      PS_BUILDER_C_API bool ImportModel(const char* path, bool single_model,
        const char* vertex_shader, const char* pixel_shader);

      /**
      * @brief imports a material and packages it for use by the engine and editor
      * @param[in] path (const char*) the absolute or relative path to the file to be imported
      * @return (bool) success return value
      */
      PS_BUILDER_C_API bool ImportMaterial(const char* path);

      /**
      * @brief imports a shader and packages it for use by the engine and editor
      * @param[in] path (const char*) the absolute or relative path to the file to be imported
      * @return (bool) success return value
      * @remark supported formats: hlsl
      */
      PS_BUILDER_C_API bool ImportShader(const char* path);

      /**
      * @brief imports a texture and packages it for use by the engine and editor
      * @param[in] path (const char*) the absolute or relative path to the file to be imported
      * @return (bool) success return value
      * @remark supported formats: png, jpeg, bmp
      */
      PS_BUILDER_C_API bool ImportTexture(const char* path);

      /**
      * @brief removes an asset from the asset cache
      * @param[in] id (int) name of the asset
      * @return (bool) seccess return value
      */
      PS_BUILDER_C_API bool DeleteModel(uint64_t id);

      /**
      * @brief removes a material asset from the asset cache
      * @param[in] id (int) name of the asset
      * @return (bool) seccess return value
      */
      PS_BUILDER_C_API bool DeleteMaterial(uint64_t id);

      /**
      * @brief removes a Shader asset from the asset cache
      * @param[in] id (int) name of the asset
      * @return (bool) seccess return value
      */
      PS_BUILDER_C_API bool DeleteShader(uint64_t id);

      /**
      * @brief removes a texture asset from the asset cache
      * @param[in] id (int) name of the asset
      * @return (bool) seccess return value
      */
      PS_BUILDER_C_API bool DeleteTexture(uint64_t id);

      /**
      * @brief removes a mesh asset from the asset cache
      * @param[in] id (int) name of the asset
      * @return (bool) seccess return value
      */
      PS_BUILDER_C_API bool DeleteMesh(uint64_t id);
    }
  }
}