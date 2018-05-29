#include "tools/builder/shared/Application.h"
#include <foundation/memory/memory.h>  

namespace sulphur 
{
  namespace builder 
  {
    Directory* Application::out_dir_ = nullptr; //!< working directory
    Directory* Application::package_dir_ = nullptr; //!< the output di
    foundation::String* Application::package_relative_path_ = nullptr;

    Application::Application()
    {
      // initialize application
      out_dir_ =  foundation::Memory::Construct<Directory>();
      package_dir_ = foundation::Memory::Construct<Directory>();
      package_relative_path_ = foundation::Memory::Construct<foundation::String>();

      Application::set_out_dir("./");
      Application::set_package_location("sulphur-builder-output");
    }

    void Application::set_out_dir(const foundation::String& path)
    {
      out_dir() = Directory(path);
    }

    void Application::set_package_location(const foundation::String& path)
    {
      package_relative_path() = path;
      package_dir() = Directory(out_dir().path().GetString() + path);
    }

    Directory& Application::out_dir()
    {
      return *out_dir_;
    }

    Directory& Application::package_dir()
    {
      return *package_dir_;
    }
    foundation::String& Application::package_relative_path()
    {
      return *package_relative_path_;
    }
    void Application::ShutDown()
    {
      foundation::Memory::Destruct<Directory>(out_dir_);
      foundation::Memory::Destruct<Directory>(package_dir_);
      foundation::Memory::Destruct<foundation::String>(package_relative_path_);
    }
  }
}