#include "tools/builder/shared/shader_compiler_includer.h"
#include <foundation/io/binary_reader.h>
#include <foundation/io/filesystem.h>

namespace sulphur 
{
  namespace builder 
  {
    //-----------------------------------------------------------------------------------------------
    GLSLangIncluder::GLSLangIncluder() :
      external_local_directory_count_(0)
    {
    }

    //-----------------------------------------------------------------------------------------------
    GLSLangIncluder::~GLSLangIncluder()
    {

    }

    //-----------------------------------------------------------------------------------------------
    GLSLangIncluder::IncludeResult* GLSLangIncluder::includeLocal(
      const char* header_name,
      const char* includer_name,
      size_t inclusion_depth)
    {
      return ReadLocalPath(header_name, includer_name, static_cast<int>(inclusion_depth));
    }

    //-----------------------------------------------------------------------------------------------
    void GLSLangIncluder::releaseInclude(GLSLangIncluder::IncludeResult* result)
    {
      if (result != nullptr)
      {
        delete[] static_cast<char*>(result->userData);
        delete result;
      }
    }

    void GLSLangIncluder::AddIncludeDirectory(const char* path)
    {
      ++external_local_directory_count_;
      directory_stack_.push_back(path);
    }

    //-----------------------------------------------------------------------------------------------
    GLSLangIncluder::IncludeResult* GLSLangIncluder::ReadLocalPath(
      const char* header_name,
      const char* includer_name,
      int depth)
    {
      // initialize our directory stack when parsing at lowest level
      directory_stack_.resize(depth + external_local_directory_count_);
      if (depth == 1)
      {
        directory_stack_.back() = getDirectory(includer_name);
      }

      eastl::reverse_iterator<foundation::String*> it;
      for (it = directory_stack_.rbegin(); it != directory_stack_.rend(); ++it)
      {
        foundation::String path = *it + '/' + header_name;
        eastl::replace(path.begin(), path.end(), '\\', '/');
        foundation::BinaryReader reader(foundation::Path(path), false);
        if (reader.is_ok() == true)
        {
          directory_stack_.push_back(getDirectory(path));
          return NewIncludeResult(path, reader);
        }
      }
      return nullptr;
    }

    //-----------------------------------------------------------------------------------------------
    GLSLangIncluder::IncludeResult* GLSLangIncluder::NewIncludeResult(
      const foundation::String& path,
      foundation::BinaryReader& reader) const
    {
      char* content = new char[reader.GetSize()];
      reader.Seek(0);
      reader.ReadBytes(reinterpret_cast<unsigned char*>(content), reader.GetSize());
      return new IncludeResult(path.c_str(), content, reader.GetSize(), content);
    }

    //-----------------------------------------------------------------------------------------------
    foundation::String GLSLangIncluder::getDirectory(const foundation::String& path) const
    {
      size_t last = path.find_last_of("/\\");
      return last == foundation::String::npos ? "." : path.substr(0, last);
    }

    //-----------------------------------------------------------------------------------------------
    D3DIncluder::D3DIncluder()
      : external_local_directory_count_(0)
    {
    }

    //-----------------------------------------------------------------------------------------------
    D3DIncluder::~D3DIncluder()
    {
    }

    //-----------------------------------------------------------------------------------------------
    HRESULT D3DIncluder::Close(LPCVOID data)
    {
      if (data != nullptr)
      {
        delete[] static_cast<const char*>(data);
      }
      return S_OK;
    }

    //-----------------------------------------------------------------------------------------------
    HRESULT D3DIncluder::Open(
      D3D_INCLUDE_TYPE include_type,
      LPCSTR file_name,
      LPCVOID /*parent_data*/,
      LPCVOID* data,
      UINT * bytes)
    {
      *bytes = 0;
      switch (include_type)
      {
      case D3D_INCLUDE_TYPE::D3D_INCLUDE_LOCAL:
      {
        return ReadLocalPath(file_name, data, *bytes);
        break;
      }
      case D3D_INCLUDE_TYPE::D3D_INCLUDE_SYSTEM:
        return E_FAIL;
        break;
      }
      return E_FAIL;
    }

    void D3DIncluder::AddIncludeDirectory(const char* path)
    {
      ++external_local_directory_count_;
      directory_stack_.push_back(path);
    }


    HRESULT D3DIncluder::ReadLocalPath(
      const char* header_name,
      LPCVOID* out_data,
      unsigned int& out_size)
    {
      directory_stack_.push_back(".");

      eastl::reverse_iterator<foundation::String*> it;
      for (it = directory_stack_.rbegin(); it != directory_stack_.rend(); ++it)
      {
        foundation::String path = *it + '/' + header_name;
        eastl::replace(path.begin(), path.end(), '\\', '/');
        foundation::BinaryReader reader(path.c_str(),false);
        if (reader.is_ok() == true)
        {
          directory_stack_.push_back(getDirectory(path));
          out_size = reader.GetSize();
          char* data = new char[out_size];
          reader.Seek(0);
          reader.ReadBytes(reinterpret_cast<unsigned char*>(data), reader.GetSize());
          *out_data = data;
          return S_OK;
        }
      }
      return E_FAIL;
    }

    //-----------------------------------------------------------------------------------------------
    foundation::String D3DIncluder::getDirectory(const foundation::String& path) const
    {
      size_t last = path.find_last_of("/\\");
      return last == foundation::String::npos ? "." : path.substr(0, last);
    }
  }
}