#include "tools/builder/shared/Application.h"

namespace sulphur 
{
  namespace builder 
  {
    Directory Application::out_dir_;
    Directory Application::package_dir_;
    foundation::String Application::package_relative_path_;

    void Application::set_out_dir(const foundation::String& path)
    {
      out_dir_ = Directory(path);
    }

    void Application::set_package_location(const foundation::String& path)
    {
      package_relative_path_ = path;
      package_dir_ = Directory(out_dir_.path() + path);
    }

    Directory Application::out_dir()
    {
      return out_dir_;
    }

    Directory Application::package_dir()
    {
      return package_dir_;
    }
    foundation::String Application::package_relative_path()
    {
      return package_relative_path_;
    }
  }
}