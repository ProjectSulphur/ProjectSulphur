#pragma once
#include <foundation/containers/string.h>
#include <foundation/utils/asset_definitions.h>

namespace sulphur 
{
	namespace foundation 
	{
    /**
     * @class sulphur::foundation::ModelInfo
     * @brief Stores data about the models stored in a scene and the options used to load them.
     * @author Timo van Hees
     */
    class ModelInfo
    {
    public:
      /**
       * @brief Creates and initializes an instance of ModelInfo. The file associated 
       * with or the number of models in this instance cannot be changed.
       * @param[in] file (onst sulphur::foundation::String&) The file containing the 
       * scene this instance will contain information about.
       * @param[in] number_of_models (unsigned int) The number of models to allocate storage for.
       */
      ModelInfo(const String& file, unsigned int number_of_models);
      /**
       * @brief Default constructor.
       */
      ModelInfo() = default;
      /**
       * @brief Default destructor.
       */
      ~ModelInfo() = default;

      /**
       * @return (unsigned int) The number of models this instance stores.
       */
      unsigned int number_of_models() const;
      /**
       * @brief Set the name of an asset in the scene.
       * @param[in] index (unsigned int) The index of the asset.
       * @param[in] name (const char*) The new name of the asset. 
       */
      void SetAssetName(unsigned int index, const char* name);
      /**
       * @brief Get the name of an asset in the scene.
       * @param[in] index (unsigned int) The index of the asset.
       * @return (sulphur::foundation::AssetName) The name of the asset.
       */
      AssetName GetAssetName(unsigned int index) const;
      /**
       * @brief Set if a model should be loaded.
       * @param[in] index (unsigned int) The index of the model.
       * @param[in] load (bool) If the model should be loaded.
       */
      void SetModelsToLoad(unsigned int index, bool load);
      /**
       * @brief Get if a model should be loaded.
       * @param[in] index (unsigned int) The index of the model.
       * @return (bool) If the model @index should be loaded.
       */
      bool GetModelsToLoad(unsigned int index) const;
      /**
       * @brief Get the name of the file containing the scene that was used to create this instance.
       * @return (sulphur::foundation::String) The name of the file containing the scene.
       */
      String GetFile() const;

    private:
      uint32_t number_of_models_;                             //!< The number of models in this instance.
      Vector<AssetName> model_names_; //!< The names of the models in this instance.
      Vector<bool> models_to_load_;               //!< List of which models should be loaded.
      SizedString<260> file_;                     //!< The file containing the scene.
    };
	}
}
