#include "tools/builder/pipelines/pipeline_base.h"
#include "tools/builder/shared/util.h"

#include <foundation/io/binary_reader.h>
#include <foundation/io/binary_writer.h>
#include <foundation/containers/string.h>

#include <EASTL/functional.h>


namespace sulphur 
{
  namespace builder 
  {
    //--------------------------------------------------------------------------------
    PipelineBase::PipelineBase() :
      output_path_(""),
      package_output_path_("")
    {
    }

    //--------------------------------------------------------------------------------             
    bool PipelineBase::RegisterAsset(const foundation::String& asset_origin, 
      foundation::AssetName& name, foundation::String& package_path,
      foundation::AssetID& id, bool allow_append_numbers)
    {
      // Generate an id for the name
      id = foundation::GenerateId(name);

      foundation::String original_name = name.GetString();
      for(int i = 0; i < 256; ++i)
      {
        // Check if an asset with the same id has already been packaged
        PackagedAssetsIterator it = packaged_assets_.find(id);
        if (it != packaged_assets_.end())
        {
          // Check if the file has the same origin
          if (it->second.asset_origin != asset_origin || asset_origin == ASSET_ORIGIN_USER)
          {
            if(allow_append_numbers == false)
            {
              return false;
            }

            // If it doesn't we start adding numbers at the end of the name
            char number[4];
            snprintf(number, 4, "%i", i + 1);
            name = original_name + foundation::String(number);
            id = foundation::GenerateId(name);
          }
          else
          {
            break;
          }
        }
        else
        {
          break;
        }

        if(i == 255)
        {
          PS_LOG_WITH(foundation::LineAndFileLogger, Error, 
            "Trying to register a file with a name that is already in use. Can't find a suitable replacement name.");
          return false;
        }
      }

      // Update the package path
      const foundation::String package_relative_path = package_output_path() + 
        name.GetString() + '.' + GetPackageExtension();
      package_path = output_path() + package_relative_path;

      // Register the asset
      foundation::PackagePtr& package = packaged_assets_[id];
      package.asset_origin = asset_origin;
      package.filepath = package_relative_path;

      ExportCache();

      return true;
    }

    //--------------------------------------------------------------------------------
    bool PipelineBase::PackageDefaultAssets()
    {
      return true;
    }

    //--------------------------------------------------------------------------------
    void PipelineBase::Initialize()
    {
      foundation::String cache_file = output_path() + GetCacheName() + ".cache";
      foundation::BinaryReader reader(cache_file, false);
      if (reader.is_ok())
      {
        // Load the cached package information
        packaged_assets_ = reader.ReadMap<foundation::AssetID, foundation::PackagePtr>();
      }
      else
      {
        PackageDefaultAssets();
      }
    }

    //--------------------------------------------------------------------------------
    void PipelineBase::ExportCache()
    {
      foundation::String cache_file = output_path() + GetCacheName() + ".cache";
      foundation::BinaryWriter writer(cache_file);

      writer.Write(packaged_assets_);

      if(writer.Save() == false)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Warning, 
          "Failed to write %s cache.", GetCacheName().c_str());
      }
    }

    //--------------------------------------------------------------------------------
    bool PipelineBase::AssetExists(foundation::AssetID id)
    {
      PackagedAssetsIterator it = packaged_assets_.find(id);
      if (it != packaged_assets_.end())
      {
        return true;
      }

      return false;
    }

    //--------------------------------------------------------------------------------
    bool PipelineBase::AssetExists(const foundation::AssetName& name)
    {
      foundation::AssetID id = foundation::GenerateId(name);
      return AssetExists(id);
    }

    //--------------------------------------------------------------------------------
    bool PipelineBase::DeleteAsset(foundation::AssetID id)
    {
      PackagedAssetsIterator it = packaged_assets_.find(id);
      if (it != packaged_assets_.end())
      {
        foundation::String package = output_path() + it->second.filepath;
        if (remove(package.c_str()) != 0)
        {
          return false;
        }

        packaged_assets_.erase(it);

        ExportCache();

        return true;
      }

      return false;
    }

    //--------------------------------------------------------------------------------
    bool PipelineBase::DeleteAsset(const foundation::AssetName& name)
    {
      foundation::AssetID id = foundation::GenerateId(name);
      return DeleteAsset(id);
    }

    //--------------------------------------------------------------------------------
    void PipelineBase::SetOutputLocation(const foundation::String& output_path)
    {
      output_path_ = output_path;
      eastl::replace(output_path_.begin(), output_path_.end(), '\\', '/');
      if (output_path_.back() != '/' && output_path_.empty() == false)
      {
        output_path_ += '/';
      }

      Initialize();
    }

    //--------------------------------------------------------------------------------
    void PipelineBase::SetPackageOutputLocation(const foundation::String& package_path)
    {
      package_output_path_ = package_path;
      eastl::replace(package_output_path_.begin(), package_output_path_.end(), '\\', '/');
      if (package_output_path_.back() != '/')
      {
        package_output_path_ += '/';
      }
    }

    //--------------------------------------------------------------------------------
    const foundation::String& PipelineBase::output_path() const
    {
      return output_path_;
    }

    //--------------------------------------------------------------------------------
    const foundation::String& PipelineBase::package_output_path() const
    {
      return package_output_path_;
    }
  }
}
