#pragma once
#include "tools/builder/base/commands_base.h"

namespace sulphur 
{ 
  namespace builder 
  {
    class ModelPipeline;
    class MaterialPipeline;
    class MeshPipeline;
    class ShaderPipeline;
    class TexturePipeline;
    class AudioPipeline;
    enum struct Error;

    /**
    *@class sulphur::builder::SetOutputDir : sulphur::builder::Command
    *@brief command that sets the current directory to output the cache files to
    *@remark only available when running as .exe
    *@author Stan Pepels
    */
    class SetOutputDir : public ICommand
    {
    public:
      /**
      *@brief constructor
      *@param[in] key (const char*) key to identify this command
      */
      SetOutputDir(const char* key);

      /**
      *@see sulphur::builder::Command::GetDescription
      */
      const char* GetDescription() const override;

      /**
      *@see sulphur::builder::Command::Run
      */
      void Run(const CommandInput& input) override;
    };

    /**
    *@class sulphur::builder::SetPackagePath : sulphur::builder::Command
    *@brief command that sets the current directory to output the packages to relative to the cache files
    *@remark only available when running as .exe
    *@author Stan Pepels
    */
    class SetPackagePath : public ICommand
    {
    public:
      /**
      *@brief constructor
      *@param[in] key (const char*) key to identify this command
      */
      SetPackagePath(const char* key);

      /**
      *@see sulphur::builder::Command::GetDescription
      */
      const char* GetDescription() const override;

      /**
      *@see sulphur::builder::Command::Run
      */
      void Run(const CommandInput& input) override;
    };

    /**
    *@class sulphur::builder::ClearOutputFolders : sulphur::builder::Command 
    *@brief clears the caches and the output folder.
    *@remark this reinializes all the pipelines thus all default assets will also be rebuilt.
    *@author Stan Pepels
    */
    class ClearOutputFolders : public ICommand
    {
    public:
      /**
      *@brief constructor
      *@param[in] key (const char*) key to identify this command
      */
      ClearOutputFolders(const char* key);

      /**
      *@see sulphur::builder::Command::GetDescription
      */
      const char* GetDescription() const override;

      /**
      *@see sulphur::builder::Command::Run
      */
      void Run(const CommandInput& input) override;
    };

    /**
    *@class sulphur::builder::RefreshCacheFiles : sulphur::builder::Command
    *@brief refreshes the cache of all pipelines
    *@author Stan Pepels
    */
    class RefreshCacheFiles : public ICommand
    {
    public:
      /**
      *@brief constructor
      *@param[in] key (const char*) key to identify this command
      */
      RefreshCacheFiles(const char* key);

      /**
      *@see sulphur::builder::Command::GetDescription
      */
      const char* GetDescription() const override;

      /**
      *@see sulphur::builder::Command::Run
      */
      void Run(const CommandInput& input) override;
    };
  }
}