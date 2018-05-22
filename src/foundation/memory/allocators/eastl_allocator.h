#pragma once
#include <EASTL/allocator.h>


namespace sulphur
{
  namespace foundation
  {
    class IAllocator;
    class EASTLAllocator
    {
    public:
      EASTLAllocator( const char* pName = EASTL_NAME_VAL( "EASTLAllocator" ) );
      EASTLAllocator( const eastl::allocator& x, const char* pName = EASTL_NAME_VAL( "EASTLAllocator" ) );
      ~EASTLAllocator();
      EASTLAllocator& operator=( const EASTLAllocator& x );
      bool operator==( const EASTLAllocator& x );
      void* allocate( size_t n, int flags = 0 );
      void* allocate( size_t n, size_t alignment, size_t offset, int flags = 0 );
      void deallocate( void * ptr, size_t size );
      const char* name() const;
      void set_name( const char * name );
    protected:
#if EASTL_NAME_ENABLED
      const char* name_;
#endif
      IAllocator* allocator_;
    };
  }
}