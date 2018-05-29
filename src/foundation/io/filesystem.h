#pragma once
#include "foundation/containers/string.h"

namespace sulphur
{
  namespace foundation
  {
    /**
     * @class sulphur::foundation::Path
     * @brief Utility class to make working with file/folder paths simpler.
     * @author Timo van Hees
     */
    class Path
    {
    public:
      /**
      * @brief Default constructor.
      */
      Path();
      /**
       * @brief Construct path from a C string.
       * @param[in] path (const char*) File or folder path.
       */
      Path(const char* path);
      /**
      * @brief Construct path from a string.
      * @param[in] path (const sulphur::foundation::String&) File or folder path.
      */
      Path(const String& path);

      bool operator==(const String& rhs) const; //!< equals operator
      bool operator==(const char* rhs) const; //!< equals operator
      bool operator==(const Path& rhs) const; //!< equals operator
      bool operator!=(const String& rhs) const; //!< doesn't equal operator
      bool operator!=(const char* rhs) const; //!< doesn't equal operator
      bool operator!=(const Path& rhs) const; //!< doesn't equal operator
      bool operator<(const Path& rhs) const; //!< less than operator
      Path operator+(const String& rhs) const; //!< Addition operator
      Path operator+(const char* rhs) const; //!< Addition operator
      Path operator+(const Path& rhs) const; //!< Addition operator

      /**
       * @brief Get the path as a string.
       * @return (sulphur::foundation::String) The string.
       */
      String GetString() const;

      /**
       * @brief Get the path.
       * @return (const sulphur::foundation::String&) the path.
       */
      const String& path() const;
      /**
       * @brief Get if file path.
       * @return (bool) If the path points to a file.
       */
      bool is_file_path() const;
      /**
       * @brief Get is relative path.
       * @return (bool) If the path is a relative path.
       */
      bool is_relative_path() const;
      /**
       * @brief Get the folder path.
       * @return (sulphur::foundation::String) The folder path.
       */
      String GetFolderPath() const;
      /**
      * @brief Get the file name without extension.
      * @return (sulphur::foundation::String) The file name.
      */
      String GetFileName() const;
      /**
      * @brief Get the file extension.
      * @return (sulphur::foundation::String) The file extension.
      */
      String GetFileExtension() const;
      /**
      * @brief Get the file name with extension.
      * @return (sulphur::foundation::String) The file name.
      */
      String GetFileNameWithExtension() const;
      /**
      * @brief Check if the file or folder exists.
      * @return (bool) If the file or folder exists.
      */
      bool Exists() const;

      /**
      * @brief Creates a folder of it doesn't exist.
      * @return (bool) If the folder was succesfully created.
      */
      bool Create() const;
      /**
      * @brief If the path is empty.
      * @return (bool) If the path is empty.
      */
      bool IsEmpty() const;

      /**
      * @brief Make the string lower case.
      * @param[in|out] str (sulpher::foundation::String&) The string.
      */
      static void ToLowercase(String& str);
      /**
      * @brief Make all double backslashes in the string a forward slash.
      * @param[in|out] str (sulpher::foundation::String&) The string.
      */
      static void BackToForwardSlashes(String& str);
      /**
      * @brief Remove redundent characters from path.
      * @param[in|out] path (sulpher::foundation::String&) The path.
      */
      static void RemoveRedundantCharacters(String& path);
      /**
      * @brief Check if the path points to a file.
      * @param[in] path (const sulpher::foundation::String&) The path.
      * @return (bool) If the path points to a file.
      */
      static bool CheckIfFilePath(const String& path);
      /**
      * @brief Check if the path is a relative path.
      * @param[in] path (const sulpher::foundation::String&) The path.
      * @return (bool) If the path is a relative path.
      */
      static bool CheckIfRelativePath(const String& path);
      
    private:
      /**
       * @brief Get information about and clean the new path.
       */
      void ProcessString();
      /**
       * @brief Applies platform specific operations on the path.
       */
      void AddPlatformSpecific(String& path) const;
      /*
       * @brief Removes platform specific things from the path.
       */
      void RemovePlatformSpecific(String& path) const;

      String path_;       //!< The path
      bool is_file_path_; //!< If the path points to a file
      bool is_relative_;  //!< If the path is a relative path
    };
  }
}
