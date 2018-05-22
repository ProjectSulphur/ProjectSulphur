#pragma once
#include <foundation/containers/vector.h>
#include <foundation/containers/string.h>

namespace sulphur 
{
  namespace builder 
  {
    /**
    *@class sulphur::builder::Directory
    *@brief class for doing directory / filesystem operations
    *@author Stan Pepels
    */
    class Directory
    {
    public:
      /**
      @brief contructor
      */
      Directory();

      /**
      *@brief initializes class to represent the folder at 'path'
      *@param[in] path (cont char*) path to the folder folder paths should always end with a '\'
      */
      Directory(const char* path);

      /**
      *@brief initializes class to represent the folder at 'path'
      *@param[in] path (sulphur::foundation::String&) path to the folder. Folder paths should always end with a '\'
      */
      Directory(const foundation::String& path);

      /**
      *@brief destructor
      */
      ~Directory() = default;

      /**
      *@brief compares the path given to the path of the folder this class represents
      *@param[in] path (const sulphur::builder::String&) other path to compare to.
      *@return (bool) boolean indicating whether path points to this directory
      */
      bool operator==(const foundation::String& path);

      /**
      *@brief compares the path given to the path of the folder this class represents
      *@param[in] path (const sulphur::builder::String&) other path to compare to.
      *@return (bool) boolean indicating whether path points to this directory
      */
      bool operator==(const char* path);

      /**
      *@brief compares the path from the folder given to the path of the folder this class represents
      *@param[in] path (const sulphur::builder::Directory&) other directory to compare to.
      *@return (bool) boolean indicating whether other.path() points to this directory
      */
      bool operator==(const Directory& other);

      /**
      *@brief get the sub directories
      *@return (sulphur::foundation::Vector<sulphur::builder::Directory>) vector containing all sub-directories
      */
      foundation::Vector<Directory> GetSubDirs() const;

      /**
      *@brief get the parent directory
      *@return (sulphur::builder::Directory) the parent directory of this directory
      */
      Directory GetParentDir() const;

      /**
      *@brief get the sub directories recursivly
      *@return (sulphur::foundation::Vector<sulphur::builder::Directory>) vector containing all sub-directories found recursively
      */
      foundation::Vector<Directory> GetSubDirsRecursive() const;

      /**
      *@brief get the files in this directory
      *@return (sulphur::foundation::Vector<sulphur::foundation::String>) vector containing files found in this directory
      */
      foundation::Vector<foundation::String> GetFiles() const;

      /**
      *@brief get the files in this directory and all subdirectories
      *@return (sulphur::foundation::Vector<sulphur::foundation::String>) vector containing files recursively found from this directory
      */
      foundation::Vector<foundation::String> GetFilesRecursive() const;

      /**
      *@brief get the path_ parameter
      *@return (sulphur::foundation::String) relative or absolute path to this directory
      */
      foundation::String path() const;

      /**
      *@brief get the absolute path to this folder. 
      *@return (sulphur::foundation::String) absolute path to this directory
      */
      foundation::String GetAbsolutePath() const;

      /**
      *@brief check if a folder at the path_ parameter exists
      *@return (bool) boolean indicating whether a folder exists at the path given in the constructor
      */
      bool Exists() const;

      /**
      *@brief create a folder with the path given with path_ if none exists
      */
      void Create() const;
    private:
      foundation::String path_; //<! path to a folder

      /**
      * @brief checks if the input character is a forward slash
      * @param[in] in (char) character to check
      */
      static bool IsForwardSlash(char in);
    };
  }
}