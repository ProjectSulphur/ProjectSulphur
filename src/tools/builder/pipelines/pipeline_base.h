#pragma once
#include <foundation/containers/string.h>
#include <foundation/utils/asset_definitions.h>
#include <foundation/containers/map.h>
#include <foundation/logging/logger.h>

/**
 * @file pipeline_base.h
 * @def ASSET_ORIGIN_USER
 * @brief Use when packaging an asset that doesn't come from a file but is created by the user.
 */
#define ASSET_ORIGIN_USER "OriginUser"

namespace sulphur 
{
	namespace builder 
  {
	  /**
     * @brief Iterator typdef used when iterating through asset packages.
     */
    using PackagedAssetsIterator =
      foundation::Map<foundation::AssetID, foundation::PackagePtr>::iterator;

	  /**
	   * @class sulphur::builder::PipelineBase 
     * @brief Base class for all asset pipelines.
     * @author Timo van Hees
     */
    class PipelineBase
    {
    protected:
      PipelineBase();
      virtual ~PipelineBase() = default;

      /**
       * @brief Registers an asset as being part of the package.
       * @param[in] asset_origin (const sulphur::foundation::String&) The file the asset was 
       * loaded from. Should be ASSET_ORIGIN_USER when the asset is created by the user.
       * @param[in|out] name (sulphur::foundation::String&) The name of the asset. Numbers 
       * can be appended onto the back of the name if another asset with the same name already 
       * exists, doesn't have the same @asset_origin and @allow_append_numbers equals true.
       * @param[out] package_path (sulphur::foundation::String&) The file 
       * of the package the asset should be added to.
       * @param id (sulphur::foundation::AssetID&) The ID that has been assigned to this asset.
       * @param[in] allow_append_numbers If numbers can be appended to the name of the asset.
       * @return (bool) False if the asset failed to register. The asset shouldn't be packaged.
       */
      bool RegisterAsset(const foundation::String& asset_origin, foundation::AssetName& name,
        foundation::String& package_path, foundation::AssetID& id, 
        bool allow_append_numbers = true);

      /**
       * @brief Adds one or more asset to the package on creation.
       * @return (bool) True if the assets were added succesfully.
       */
      virtual bool PackageDefaultAssets();

    public:
      /**
       * @brief Initializes the pipeline. Loads the package.
       */
      void Initialize();
      /**
       * @brief Stores changes in the package on disk.
       */
      void ExportCache();

      /**
       * @brief Loads an asset from the package.
       * @tparam T The type of the asset stored in the package.
       * @param[in] id (sulphur::foundation::AssetID) The id of the asset.
       * @param[out] asset (T&) The asset loaded from the package.
       * @return True if the asset was loaded succeesfully.
       */
      template<typename T>
      bool LoadAssetFromPackage(foundation::AssetID id, T& asset);

      /**
      * @brief Loads an asset from the package.
      * @tparam T The type of the asset stored in the package.
      * @param[in] name (const sulphur::foundation::AssetName&) The name of the asset.
      * @param[out] asset (T&) The asset loaded from the package.
      * @return True if the asset was loaded succeesfully.
      */
      template<typename T>
      bool LoadAssetFromPackage(const foundation::AssetName& name, T& asset);

      /**
       * @brief Checks if an asset exists in the package.
       * @param[in] id (sulphur::foundation::AssetID) The ID of the asset.
       * @return (bool) True if the asset exists.
       */
      bool AssetExists(foundation::AssetID id);
      /**
       * @brief Checks if an asset exists in the package.
       * @param[in] name (const sulphur::foundation::AssetName&) The name of the asset.
       * @return (bool) True if the asset exists.
       */
      bool AssetExists(const foundation::AssetName& name);

      /**
       * @brief Deletes an asset from the package.
       * @param[in] id (sulphur::foundation::AssetID) The ID of the asset that you want to delete.
       * @return (bool) If the asset was deleted succesfully.
       */
      bool DeleteAsset(foundation::AssetID id);
      /**
       * @brief Deletes an asset from the package.
       * @param[in] name (const sulphur::foundation::String&) The name of the asset 
       * you want to delete.
       * @return (bool) If the asset was deleted succesfully.
       */
      bool DeleteAsset(const foundation::AssetName& name);

      /**
       * @brief Set the output location of the cache.
       * @param[in] output_path (const sulphur::foundation::String&) The new output 
       * location of the cache.
       * @remark Reinitializes the pipeline automatically. Assets packaged at previous output 
       * location will be saved but won't be accesible through this pipeline. Unless the output 
       * path is changed back.
       */
      void SetOutputLocation(const foundation::String& output_path);

      /**
      * @brief Set the output location of the packages relative to the output path.
      * @param[in] package_path (const sulphur::foundation::String&) The new output
      * location of the package.
      */
      void SetPackageOutputLocation(const foundation::String& package_path);

      /**
       * @brief Getter for the output directory of the caches.
       * @return (const sulphur::foundation::String&) The current output path of this pipeline.
       */
      const foundation::String& output_path() const;

      /**
       * @brief Getter for the output directory of the packages.
       * @return (const sulphur::foundation::String&) The current output path for the packages 
       * relative to the pipeline output path.
       */
      const foundation::String& package_output_path() const;

      /**
       * @brief Asset pipelines deriving from this class override this method to 
       * set the name of the package cache.
       * @return (sulphur::foundation::String) The file name of package cache.
       */

      virtual foundation::String GetCacheName() const = 0;
      /**
       * @brief Asset pipelines deriving from this class override this method to 
       * set the file extension of a package exported by this pipeline.
       * @return (sulphur::foundation::String) The package file extension.
       */
      virtual foundation::String GetPackageExtension() const = 0;

    private:
      foundation::String output_path_;  //!< The ouput path of the caches exported by this pipeline.
      foundation::String package_output_path_; //!< The output path of the packaged exported by this pipeline relative to the output path.
      foundation::Map<foundation::AssetID, foundation::PackagePtr> packaged_assets_; //!< Map of all assets in the package and information about them.
    };

	  template <typename T>
	  bool PipelineBase::LoadAssetFromPackage(foundation::AssetID id, T& asset)
	  {
      PackagedAssetsIterator it = packaged_assets_.find(id);
      if (it == packaged_assets_.end())
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Warning,
          "There is no asset with id(%llu) in the package.", id);
        return false;
      }

      foundation::BinaryReader reader(output_path() + it->second.filepath);
      if(reader.is_ok() == false)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "The packaged asset can not be found.");
        return false;
      }

      asset = reader.Read<T>();

      return true;
	  }

	  template <typename T>
	  bool PipelineBase::LoadAssetFromPackage(const foundation::AssetName& name,
	    T& asset)
	  {
      foundation::AssetID id = foundation::GenerateId(name);
      return LoadAssetFromPackage(id, asset);
	  }
	}
}