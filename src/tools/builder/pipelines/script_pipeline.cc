#include "tools/builder/pipelines/script_pipeline.h"
#include <foundation/pipeline-assets/script.h>

//#define SCRIPT_COMPILE_DEBUG

namespace sulphur
{
  namespace builder
  {
    //--------------------------------------------------------------------------------
    bool ScriptPipeline::Create(const foundation::Path& file,
      foundation::ScriptAsset& script) const
    {
      if (ValidatePath(file) == false)
      {
        PS_LOG_BUILDER(Error,
          "Invalid file path passed. The path %s does not point to a location in the project directory %s", file.path().c_str(),
          project_dir().path().c_str());
        return false;
      }

      foundation::Path file_path = file.is_relative_path() ? project_dir() + file : file;

      char buffer[1024] = {};
#ifdef SCRIPT_COMPILE_DEBUG
      snprintf(buffer, sizeof(buffer), "luajit -bg \"%s\" temp_script.temp", file_path.GetString().c_str());
#else
      snprintf(buffer, sizeof(buffer), "luajit -b \"%s\" temp_script.temp", file_path.GetString().c_str());
#endif

      int result = system(buffer);
      if (result != 0)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "Failed to create bytecode from script. file: %s", file_path.GetString().c_str());
        return false;
      }

      foundation::BinaryReader binary_reader("temp_script.temp");
      if (binary_reader.is_ok() == false)
      {
        return false;
      }

      remove("temp_script.temp");

      script.name = file_path.GetFileName();

      script.data.binary.resize(binary_reader.GetSize());
      memcpy_s(script.data.binary.data(), script.data.binary.size(),
        binary_reader.data().data(), binary_reader.GetSize());

      return true;
    }

    //--------------------------------------------------------------------------------
    bool ScriptPipeline::PackageScript(const foundation::Path& asset_origin,
      foundation::ScriptAsset& script)
    {
      if (ValidatePath(asset_origin) == false)
      {
        PS_LOG_BUILDER(Error,
          "Invalid file path passed. The path %s does not point to a location in the project directory %s", asset_origin.path().c_str(),
          project_dir().path().c_str());
        return false;
      }

      foundation::Path origin = CreateProjectRelativePath(asset_origin);

      if (script.name.get_length() == 0)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Warning,
          "Script name not initialized. The script will not be packaged.");
        return false;
      }

      if (script.data.binary.empty() == true)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Error,
          "Script holds no binary data. The script will not be packaged.");
        return false;
      }

      foundation::Path output_file = "";
      if (RegisterAsset(origin, script.name, output_file, script.id) == false)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Warning,
          "Failed to register script. It will not be packaged.");

        return false;
      }

      foundation::BinaryWriter writer(output_file);

      writer.Write(script.data);

      if (writer.Save() == false)
      {
        PS_LOG_WITH(foundation::LineAndFileLogger, Warning,
          "Failed to package script.");
        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    foundation::String ScriptPipeline::GetPackageExtension() const
    {
      return "ssc";
    }

    //--------------------------------------------------------------------------------
    foundation::String ScriptPipeline::GetCacheName() const
    {
      return "script_package";
    }
  }
}
