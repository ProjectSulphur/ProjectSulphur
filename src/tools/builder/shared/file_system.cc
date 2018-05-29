#include "tools/builder/shared/file_system.h"
#include <foundation/logging/logger.h>
#include <filesystem>
#include <EASTL/algorithm.h>

namespace std_filesystem = std::experimental::filesystem::v1;

namespace sulphur
{
  namespace builder
  {
    //-----------------------------------------------------------------------------------------------
    Directory::Directory() :
      path_("")
    {}

    //-----------------------------------------------------------------------------------------------
    Directory::Directory(const char* path) :
      path_(path)
    {}

    //-----------------------------------------------------------------------------------------------
    Directory::Directory(const foundation::String& path) :
      path_(path)
    {}

    //-----------------------------------------------------------------------------------------------
    bool Directory::operator==(const foundation::String& path)
    {
      return path_ == path;
    }

    //-----------------------------------------------------------------------------------------------
    bool Directory::operator==(const char* path)
    {
      return path_ == path;
    }

    //-----------------------------------------------------------------------------------------------
    bool Directory::operator==(const Directory& other)
    {
      return other.path_ == path_;
    }

    //-----------------------------------------------------------------------------------------------
    foundation::Vector<Directory> Directory::GetSubDirs() const
    {
      foundation::Vector<Directory> result;
      std_filesystem::directory_iterator dir_iter =
        std_filesystem::directory_iterator(path_.GetString().c_str());

      while (dir_iter != std_filesystem::end(dir_iter))
      {
        if (std_filesystem::is_directory(dir_iter->path()) == true)
        {
          Directory directory = Directory(dir_iter->path().string().c_str());
          result.push_back(directory);
        }
        ++dir_iter;
      }
      return result;
    }

    //-----------------------------------------------------------------------------------------------
    Directory Directory::GetParentDir() const
    {
      foundation::String path = path_.GetString();
      size_t last_div = path.find_last_of("\\");
      Directory parent_dir = Directory(path.substr(0, last_div));
      return parent_dir;
    }

    //-----------------------------------------------------------------------------------------------
    foundation::Vector<Directory> Directory::GetSubDirsRecursive() const
    {
      foundation::Vector<Directory> result;
      std_filesystem::recursive_directory_iterator dir_iter =
        std_filesystem::recursive_directory_iterator(path_.GetString().c_str());

      while (dir_iter != std_filesystem::end(dir_iter))
      {
        if (std_filesystem::is_directory(dir_iter->path()) == true)
        {
          foundation::String tmp = dir_iter->path().string().c_str();
          tmp += "/";
          Directory directory = Directory(tmp);
          result.push_back(directory);
        }
        ++dir_iter;
      }
      return result;
    }

    //-----------------------------------------------------------------------------------------------
    foundation::Vector<foundation::Path> Directory::GetFiles() const
    {
      foundation::String temp_path = path_.GetString();
      if(temp_path.empty() == true)
      {
        temp_path = "./";
      }
      foundation::Vector<foundation::Path> result = {};
      std_filesystem::directory_iterator dir_iter =
        std_filesystem::directory_iterator(temp_path.c_str());

      while (dir_iter != std_filesystem::end(dir_iter))
      {
        if (std_filesystem::is_regular_file(dir_iter->path()) == true)
        {
          foundation::Path path = dir_iter->path().string().c_str();
          result.push_back(path);
        }
        ++dir_iter;
      }
      return result;
    }

    //-----------------------------------------------------------------------------------------------
    foundation::Vector<foundation::Path> Directory::GetFilesRecursive() const
    {
      foundation::String temp_path = path_.GetString();
      if (temp_path.empty() == true)
      {
        temp_path = "./";
      }
      foundation::Vector<foundation::Path> result;;
      std_filesystem::recursive_directory_iterator dir_iter =
        std_filesystem::recursive_directory_iterator(temp_path.c_str());

      while (dir_iter != std_filesystem::end(dir_iter))
      {
        if (std_filesystem::is_regular_file(dir_iter->path()) == true)
        {
          foundation::Path path = dir_iter->path().string().c_str();
          result.push_back(path);
        }
        ++dir_iter;
      }
      return result;
    }

    //-----------------------------------------------------------------------------------------------
    foundation::Path Directory::path() const
    {
      return path_;
    }

    //-----------------------------------------------------------------------------------------------
    foundation::Path Directory::GetAbsolutePath() const
    {
      foundation::Path result = path_;
      result = std_filesystem::system_complete(result.GetString().c_str()).string().c_str();
      return result;
    }

    //-----------------------------------------------------------------------------------------------
    bool Directory::Exists() const
    {
      if(path_.IsEmpty() == true)
      {
        return true;
      }
      return std_filesystem::exists(path_.GetString().c_str());
    }

    //-----------------------------------------------------------------------------------------------
    void Directory::Create() const
    {
      if(path_.IsEmpty())
      {
        return;
      }

      if (Exists() == false)
      {
        std_filesystem::create_directory(path_.GetString().c_str());
      }
    }

    //-----------------------------------------------------------------------------------------------
    bool Directory::IsForwardSlash(char in)
    {
      if (in == '\\')
      {
        return true;
      }
      return false;
    }

    //-----------------------------------------------------------------------------------------------
    bool Path::IsRelative(const foundation::String& path)
    {
      std_filesystem::path p = path.c_str();
      return p.is_relative();
    }

    //-----------------------------------------------------------------------------------------------
    bool Path::IsAbsolute(const foundation::String& path)
    {
      std_filesystem::path p = path.c_str();
      return p.is_absolute();
    }
  }
}