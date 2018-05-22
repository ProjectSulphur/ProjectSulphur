#pragma once
#include <foundation/containers/string.h>
#include <foundation/containers/vector.h>

#include "../glslang/Public/ShaderLang.h"
#include <D3Dcommon.h>

namespace sulphur 
{
  namespace foundation 
  {
    class BinaryReader;
  }

  namespace builder 
  {
    /**
    *@class sulphur::builder::GLSLangIncluder : glslang::TShader::Includer
    *@brief implementation of glslang includer interface
    *@author Stan Pepels
    */
    class GLSLangIncluder : public glslang::TShader::Includer
    {
    public:
      /**
      *@brief default constructor
      */
      GLSLangIncluder();

      /**
      *@brief default destructor
      */
      virtual ~GLSLangIncluder() override;

      /**
      *@brief tries to find a local include result
      *@param[in] header_name (const char*) name of the header to include
      *@param[in] includer_name (const char*) file the include directive is in
      *@param[in] inclusion_depth (size_t) the current depth of the include directive
      *@return (glslang::TShader::Includer::IncludeResult) the file found if any
      *@remark if no file can be found nullptr is returned
      */
      virtual IncludeResult* includeLocal(const char* header_name,
                                          const char* includer_name,
                                          size_t inclusion_depth) override;

      /**
      *@brief release resources allocated by sulphur::builder::GLSLangIncluder::NewIncludeResult
      *@param[in] result(glslang::Includer::IncludeResult) resource to free
      */
      virtual void releaseInclude(IncludeResult* result) override;

      /**
      *@brief add an include directory to the include list
      *@param[in] path(const char*) directory to add to search list
      */
      void AddIncludeDirectory(const char* path);
    private:
      foundation::Vector<foundation::String> directory_stack_; //<! directory search list
      int external_local_directory_count_; //<! number of additional include directories

      /**
      *@brief try and find the include file in one of the directories of the directory_stack_
      *@param[in] header_name (const char*) name of the header to include
      *@param[in] includer_name (const char*) file the include directive is in
      *@param[in] inclusion_depth (int) the current depth of the include directive
      *@return (glslang::TShader::Includer::IncludeResult) the file found if any
      *@remark if no file can be found nullptr is returned
      */
      IncludeResult* ReadLocalPath(const char* header_name, const char* includer_name, int depth);

      /**
      *@brief read a file into a new include result structure
      *@param[in] path (const sulphur::foundation::String&) path to file
      *@param[in] reader (sulphur::foundation::BinaryReader&) reader to read file with
      *@return (glslang::TShader::Includer::IncludeResult) the file found if any
      *@remark if no file can be found nullptr is returned
      */
      IncludeResult* NewIncludeResult(const foundation::String& path,
                                      foundation::BinaryReader& reader) const;

      /**
      *@brief get the directory from a path to a file
      *@param[in] path (const sulphur::foundation::String&) path to the file
      *@return (sulphur::foundation::String) path without the file appended
      */
      foundation::String getDirectory(const foundation::String& path) const;
    };


    /**
    *@class sulphur::builder::D3DIncluder : ID3DInclude
    *@brief implementation of ID3DInclude interface
    *@author Stan Pepels
    */
    class D3DIncluder : public ID3DInclude
    {
    public:
      /**
      *@brief default constructor
      */
      D3DIncluder();

      /**
      *@brief default destructor
      */
      ~D3DIncluder();

      /**
      *@brief free data alocated by sulphur::builder::D3DIncluder::Open
      *@param[in] data (LPCVOID) pointer to data block
      */
      HRESULT Close(LPCVOID data) override;

      /**
      *@brief
      *@param[in] include_type (D3D_INCLUDE_TYPE) system or loacl include
      *@param[in] file_name (LPCSTR) name of the file to inlcude
      *@param[in] parent_data (LPCVOID) data about the includer
      *@param[out] data (LPCVOID*) Pointer to the buffer that contains the include directives. 
      *@param[out] bytes (UINT*) size of the buffer that contains the include directives. 
      */
      HRESULT Open(D3D_INCLUDE_TYPE include_type,
                   LPCSTR file_name,
                   LPCVOID /*parent_data*/,
                   LPCVOID* data,
                   UINT* bytes) override;

      /**
      *@brief add an include directory to the include list
      *@param[in] path (const char*) directory to add to search list
      */
      void AddIncludeDirectory(const char* path);
    private:
      foundation::Vector<foundation::String> directory_stack_; //<! directory search list
      int external_local_directory_count_;//<! number of additional include directories

      /**
      *@brief try and find the include file in one of the directorues of the directory_stack_
      *@param[in] header_name (const char*) name of the header to include
      *@param[out] includer_name (LPCVOID*) include data read from file
      *@param[out] inclusion_depth (const char*) size of the out buffer
      *@return (HRESULT) error code indicating succes or fail
      */
      HRESULT ReadLocalPath(const char* header_name,LPCVOID* buf_out, unsigned int& out_size);

      /**
      *@brief get the directory from a path to a file
      *@param[in] path (const sulphur::foundation::String&) path to the file
      *@return (sulphur::foundation::String) path without the file appended
      */
      foundation::String getDirectory(const foundation::String& path) const;
    };
  }
}