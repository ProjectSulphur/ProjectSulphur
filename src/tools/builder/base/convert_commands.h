#pragma once
#include "tools/builder/base/commands_base.h"
#include <foundation/utils/asset_definitions.h>

namespace sulphur 
{
  namespace builder 
  {
    class Directory;
    enum struct Error;

    /**
    *@class sulphur::builder::Convert : sulphur::builder::ICommand
    *@brief command that convertes all assets in a folder.
    *@author Stan Pepels
    */
    class Convert : public ICommand
    {
    public:
      /**
      *@brief constructor
      *@param[in] key (const char*) key to identify this command
      */
      Convert(const char* key);

      /**
      *@see sulphur::builder::Command::GetDescription
      */
      const char* GetDescription() const override;

      /**
      *@see sulphur::builder::Command::Run
      */
      void Run(const CommandInput& input) override;

    protected:
      /**
      *@brief process all files inside folders passed via he command line
      *@param[in] input (const sulphur::builder::CommandInput&) parsed command line input containing the folders to be searched
      *@param[in] func (const eastl::function<bool(const foundation::String&)>&) function to use for converting the files
      */
      void ProcessFolders(const CommandInput& input,
                          const eastl::function<bool(const foundation::String&)>& func);

      /**
      *@brief process all files specified in a string delimited with a ','
      *@param[in] file_names (const foundation::string&) string containing the file names delimited with a ','
      *@param[in] input (const sulphur::builder::CommandInput&) the parsed command input
      *@param[in] func (const eastl::function<bool(const foundation::String&)>&) function to use for converting the files
      */
      void ProcessFiles(const foundation::String& file_names,
                        const CommandInput& input,
                        const eastl::function<bool(const foundation::String&)>& func);
 
      /**
      *@brief set the output location for the pipelines cache files
      *@param[in] location (const sulphur::builder::Directory&) the new output location
      *@remark this also changes the package output location
      */
      void SetOutputLocation(const Directory& location);

      /**
      *@brief reset the pipelines output location to the current working directory
      *@remark this also changes the package output location
      */
      void ResetOutputLocation();
    };

    /**
    *@class sulphur::builder::ConvertModels : sulphur::builder::Convert
    *@brief command converts .obj, .fbx, .gltf into an engine readable formats and
    * outputs them to the directory specified in Application::out_dir
    *@author Stan Pepels
    */
    class ConvertModels : public Convert
    {
    public:
      /**
      *@brief constructor
      *@param[in] key (const char*) key to identify this command
      */
      ConvertModels(const char* key);

      /**
      *@see sulphur::builder::Command::GetDescription
      */
      void Run(const CommandInput& input) final;

      /**
      *@see sulphur::builder::Command::Run
      */
      const char* GetDescription() const final;
    };

    /**
    *@class sulphur::builder::CompileShaders : sulphur::builder::Convert
    *@brief command to convert shader files into bytecode for the following platforms: Vulkan, Gnm and DX12
    *@author Stan Pepels
    */
    class CompileShaders : public Convert
    {
    public:
      /**
      *@brief constructor
      *@param[in] key (const char*) key to identify this command
      */
      CompileShaders(const char* key);

      /**
      *@see sulphur::builder::Command::Run
      */
      void Run(const CommandInput& input) final;

      /**
      *@see sulphur::builder::Command::GetDescription
      */
      const char* GetDescription() const final;
    };
    
    /**
    *@class sulphur::builder::ConvertTextures : sulphur::builder::Convert
    *@brief command to textures from raw format such as jpeg or png to an engine readable binary format
    *@author Stan Pepels
    */
    class ConvertTextures : public Convert
    {
    public:
      /**
      *@brief constructor
      *@param[in] key (const char*) key to identify this command
      */
      ConvertTextures(const char* key);

      /**
      *@see sulphur::builder::Command::Run
      */
      void Run(const CommandInput& input) final;

      /**
      *@see sulphur::builder::Command::GetDescription
      */
      const char* GetDescription() const final;
    };

    /**
    *@class sulphur::builder::ConvertSkeletons : sulphur::builder::Convert
    *@brief command to convert bone data from models to an engine readable binary format
    *@author Timo van Hees
    */
    class ConvertSkeletons : public Convert
    {
    public:
      /**
      *@brief constructor
      *@param[in] key (const char*) key to identify this command
      */
      ConvertSkeletons(const char* key);

      /**
      *@see sulphur::builder::Command::Run
      */
      void Run(const CommandInput& input) final;

      /**
      *@see sulphur::builder::Command::GetDescription
      */
      const char* GetDescription() const final;
    };

    /**
    *@class sulphur::builder::ConvertAnimations : sulphur::builder::Convert
    *@brief command to convert keyframe animations from models to an engine readable binary format
    *@author Timo van Hees
    */
    class ConvertAnimations : public Convert
    {
    public:
      /**
      *@brief constructor
      *@param[in] key (const char*) key to identify this command
      */
      ConvertAnimations(const char* key);

      /**
      *@see sulphur::builder::Command::Run
      */
      void Run(const CommandInput& input) final;

      /**
      *@see sulphur::builder::Command::GetDescription
      */
      const char* GetDescription() const final;
    };

    /**
    *@class sulphur::builder::ConvertScript : sulphur::builder::Convert
    *@brief command to convert lua scripts to compiled engine readable binary format
    *@author Timo van Hees
    */
    class ConvertScript : public Convert
    {
    public:
      /**
      *@brief constructor
      *@param[in] key (const char*) key to identify this command
      */
      ConvertScript(const char* key);

      /**
      *@see sulphur::builder::Command::Run
      */
      void Run(const CommandInput& input) final;

      /**
      *@see sulphur::builder::Command::GetDescription
      */
      const char* GetDescription() const final;
    };

    /**
    *@class sulphur::builder::ConvertAudioBank : sulphur::builder::Convert
    *@brief command to convert FMOD audio bank to audio bank asset.
    *@author Timo van Hees
    */
    class ConvertAudioBank : public Convert
    {
    public:
      /**
      *@brief constructor
      *@param[in] key (const char*) key to identify this command
      */
      ConvertAudioBank(const char* key);

      /**
      *@see sulphur::builder::Command::Run
      */
      void Run(const CommandInput& input) final;

      /**
      *@see sulphur::builder::Command::GetDescription
      */
      const char* GetDescription() const final;
    };
  }
}
