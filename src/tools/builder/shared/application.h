#pragma once
#include "tools/builder/shared/file_system.h"

namespace sulphur 
{
  namespace builder 
  {
    /**
    * @class sulphur::builder::Application
    * @brief structure for holding global application data
    * @author Stan Pepels
    */
    class Application
    {
    public:
      Application();
      /**
       * @brief Sets the output directory of the pipeline caches.
       * @param[in] path (sulphur::foundation::String) The new output directory.
       */
      static void set_out_dir(const foundation::String& path);

      /**
       * @brief Sets the output directory of the pipeline packages relative to the cache.
       * @param[in] path (sulphur::foundation::String) The new output directory.
       */
      static void set_package_location(const foundation::String& path);

      /**
       * @brief Getter for the cache output directory.
       * @return (sulpher::builder::Directory) The cache output direcoty.
       */
      static Directory& out_dir();

      /**
       * @brief Getter for the package output directory.
       * @return (sulpher::builder::Directory) The package output directory.
       */
      static Directory& package_dir();

      /**
       * @brief Getter for the package output directory relative the the cache output directory.
       * @return (sulphur::builder::Directory) The relative package output directory.
       */
      static foundation::String& package_relative_path();
      void ShutDown();

    private:
      static Directory* out_dir_; //!< working directory of the application. this directory serves as the root for finding unconverted assets
      static Directory* package_dir_; //!< the output directory of the packages.
      static foundation::String* package_relative_path_; //!< the output directory of the packages relative to the output directory.
    };
  }
}
