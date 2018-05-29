#include "foundation/io/filesystem.h"
#include <sys/stat.h>

namespace sulphur
{
  namespace foundation
  {
    //-----------------------------------------------------------------------------------------------
    Path::Path()
    {
      path_ = "";
      ProcessString();
    }

    //-----------------------------------------------------------------------------------------------
    Path::Path(const char* path)
    {
      path_ = path;
      ProcessString();
    }

    //-----------------------------------------------------------------------------------------------
    Path::Path(const String& path)
    {
      path_ = path;
      ProcessString();
    }

    //-----------------------------------------------------------------------------------------------
    bool Path::operator==(const String& rhs) const
    {
      return *this == Path(rhs);
    }

    //-----------------------------------------------------------------------------------------------
    bool Path::operator==(const char* rhs) const
    {
      return *this == Path(rhs);
    }

    //-----------------------------------------------------------------------------------------------
    bool Path::operator==(const Path& rhs) const
    {
      return path_ == rhs.path_;
    }

    //-----------------------------------------------------------------------------------------------
    bool Path::operator!=(const String& rhs) const
    {
      return *this != Path(rhs);
    }

    //-----------------------------------------------------------------------------------------------
    bool Path::operator!=(const char* rhs) const
    {
      return *this != Path(rhs);
    }

    //-----------------------------------------------------------------------------------------------
    bool Path::operator!=(const Path& rhs) const
    {
      return path_ != rhs.path_;
    }

    //-----------------------------------------------------------------------------------------------
    bool Path::operator<(const Path& rhs) const
    {
      return path_ < rhs.path_;
    }

    //-----------------------------------------------------------------------------------------------
    Path Path::operator+(const String& rhs) const
    {
      return *this + Path(rhs);
    }

    //-----------------------------------------------------------------------------------------------
    Path Path::operator+(const char* rhs) const
    {
      return *this + Path(rhs);
    }

    //-----------------------------------------------------------------------------------------------
    Path Path::operator+(const Path& rhs) const
    {
      return path_ + rhs.path_;
    }

    //-----------------------------------------------------------------------------------------------
    String Path::GetString() const
    {
      foundation::String path = path_;
      AddPlatformSpecific(path);
      return path;
    }

    //-----------------------------------------------------------------------------------------------
    bool Path::is_file_path() const
    {
      return is_file_path_;
    }

    //-----------------------------------------------------------------------------------------------
    bool Path::is_relative_path() const
    {
      return is_relative_;
    }

    //-----------------------------------------------------------------------------------------------
    const String& Path::path() const
    {
      return path_;
    }

    //-----------------------------------------------------------------------------------------------
    String Path::GetFolderPath() const
    {
      if (is_file_path_ == false)
      {
        return path_;
      }

      const size_t slash_pos = path_.find_last_of('/');
      if(slash_pos == path_.npos)
      {
        return "";
      }

      return path_.substr(0, slash_pos + 1);
    }

    //-----------------------------------------------------------------------------------------------
    String Path::GetFileName() const
    {
      if(is_file_path_ == false)
      {
        return "";
      }

      const size_t dot_pos = path_.find_last_of('.');

      const size_t slash_pos = path_.find_last_of('/');
      if (slash_pos == path_.npos)
      {
        return path_.substr(0, dot_pos);
      }

      return path_.substr(slash_pos + 1, dot_pos - slash_pos - 1);
    }

    //-----------------------------------------------------------------------------------------------
    String Path::GetFileExtension() const
    {
      if (is_file_path_ == false)
      {
        return "";
      }

      const size_t dot_pos = path_.find_last_of('.');
      if (dot_pos == path_.npos)
      {
        return "";
      }

      return path_.substr(dot_pos + 1);
    }

    //-----------------------------------------------------------------------------------------------
    String Path::GetFileNameWithExtension() const
    {
      if (is_file_path_ == false)
      {
        return "";
      }

      const size_t slash_pos = path_.find_last_of('/');
      if (slash_pos == path_.npos)
      {
        return path_;
      }

      return path_.substr(slash_pos + 1);
    }

    //-----------------------------------------------------------------------------------------------
    bool Path::Exists() const
    {
      struct stat info;
      if(stat(path_.c_str(), &info) != 0)
      {
        return false;
      }

      return true;
    }

    //-----------------------------------------------------------------------------------------------
    bool Path::IsEmpty() const
    {
      return path_.empty();
    }

    //-----------------------------------------------------------------------------------------------
    void Path::ToLowercase(String& str)
    {
      char8_t(*to_lower_case)(char in) = eastl::CharToLower;
      eastl::transform(str.begin(), str.end(), str.begin(), to_lower_case);
    }

    //-----------------------------------------------------------------------------------------------
    void Path::BackToForwardSlashes(String& str)
    {
      eastl::replace(str.begin(), str.end(), '\\', '/');
    }

    //-----------------------------------------------------------------------------------------------
    void Path::RemoveRedundantCharacters(String& path)
    {
      // Get rid of ./ in the path
      size_t pos = path.find("/./");
      while(pos != path.npos)
      {
        path.erase(pos, 2);
        pos = path.find("/./");
      }

      if(path.length() >= 2 && path[0] == '.' && path[1] == '/')
      {
        path.erase(0, 2);
      }

      // Get rid of any // that happened while combining paths
      pos = path.find("//");
      while (pos != path.npos)
      {
        path.erase(pos, 1);
        pos = path.find("//");
      }

      // Get rid of illegal characters
      eastl::replace(path.begin(), path.end(), '|', '_');
      eastl::replace(path.begin(), path.end(), '*', '_');
      if (CheckIfRelativePath(path) == true)
      {
        eastl::replace(path.begin(), path.end(), ':', '_');
      }
      eastl::replace(path.begin(), path.end(), '?', '_');
      eastl::replace(path.begin(), path.end(), '\"', '_');
      eastl::replace(path.begin(), path.end(), '<', '_');
      eastl::replace(path.begin(), path.end(), '>', '_');
    }

    //-----------------------------------------------------------------------------------------------
    bool Path::CheckIfFilePath(const String& path)
    {
      struct stat s;
      if(stat(path.c_str(), &s) == 0) //!< If the path exists
      {
        if((s.st_mode & S_IFDIR) > 0) //!< If the path is a dir
        {
          return false;
        }

        if((s.st_mode & S_IFREG) > 0)  //!< If the path is a file
        {
          return true;
        }
      }

      // Fallback. Check if the path has a file extension.
      const size_t dot_pos = path.find_last_of('.');
      const size_t slash_pos = path.find_last_of('/');
      return dot_pos != path.npos && (dot_pos > slash_pos || slash_pos == path.npos);
    }

    //-----------------------------------------------------------------------------------------------
    bool Path::CheckIfRelativePath(const String& path)
    {
      return path.length() <= 2 || path[1] != ':';
    }

    //-----------------------------------------------------------------------------------------------
    void Path::ProcessString()
    {
      ToLowercase(path_);
      BackToForwardSlashes(path_);
      RemoveRedundantCharacters(path_);
      RemovePlatformSpecific(path_);
      is_file_path_ = CheckIfFilePath(path_);
      is_relative_ = CheckIfRelativePath(path_);

      if(is_file_path_ == false && IsEmpty() == false && path_.back() != '/')
      {
        path_.append("/");
      }
    }
  }
}
