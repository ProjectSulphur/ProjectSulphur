#include "foundation/io/filesystem.h"
#include <direct.h>

namespace sulphur
{
  namespace foundation
  {
    //-----------------------------------------------------------------------------------------------
    bool Path::Create() const
    {
      if (is_file_path() == true)
      {
        return false;
      }

      size_t slash_pos = path_.find('/', 0);
      while (slash_pos != path_.npos)
      {
        Path temp_dir = path_.substr(0, slash_pos);
        if (temp_dir.Exists() == false)
        {
          if (_mkdir(temp_dir.GetString().c_str()) != 0)
          {
            return false;
          }
        }
        slash_pos = path_.find('/', slash_pos + 1);
      }

      return true;
    }

    //-----------------------------------------------------------------------------------------------
    void Path::AddPlatformSpecific(String& /*path*/) const
    {
    }

    //-----------------------------------------------------------------------------------------------
    void Path::RemovePlatformSpecific(String& /*path*/) const
    {
    }
  }
}