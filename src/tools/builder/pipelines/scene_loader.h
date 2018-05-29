#pragma once
#include <foundation/containers/string.h>
#include <foundation/containers/vector.h>
#include <assimp/Importer.hpp>
#include <foundation/io/filesystem.h>

struct aiNode;

namespace sulphur 
{
  namespace builder 
  {
    /**
     * @brief The types of supported model file formats
     */
    enum struct ModelFileType : uint8_t
    {
      kUnknown,
      kOBJ,
      kFBX,
      kglTF,
    };

    /**
     * @class sulphur::builder::SceneLoader
     * @brief Utility class for loading scenes from model files.
     * @author Timo van Hees
     */
    class SceneLoader
    {
    public:
      /**
      * @brief Loads the scene from a file.
      * @param[in] file (const sulphur::foundation::Path&) The file containing the scene.
      * @return (const aiScene*) Returns the loaded scene or nullptr.
      */
      const aiScene* LoadScene(const foundation::Path& file);

      /**
       * @return (const aiScene*) The last loaded scene or nullptr. 
       */
      const aiScene* GetScene() const;
      /**
      * @return (const sulphur::foundation::Path&) The last loaded file.
      */
      const foundation::Path& GetLastFileLoaded() const;
      /**
      * @return (sulphur::builder::ModelFileType) The type of the last loaded file.
      */
      ModelFileType GetModelFileType() const;


    private:
      Assimp::Importer importer_; //!< Importer used to load all scenes. 
      foundation::Path last_file_loaded_; //!< The file name of the last loaded scene.
      ModelFileType model_file_type_; //!< The type of model file loaded.
    };
  }
}
