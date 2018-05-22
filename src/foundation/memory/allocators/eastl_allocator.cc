#include "eastl_allocator.h"
#include "allocator.h"
#include "../memory.h"
#include "foundation/utils/macro.h"

namespace sulphur
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    EASTLAllocator::EASTLAllocator(const char* pName) :
      allocator_(&Memory::default_allocator())
    {
      PS_UNUSED_IN_RELEASE(pName);

#if EASTL_NAME_ENABLED
      name_ = pName ? pName : EASTL_ALLOCATOR_DEFAULT_NAME;
#endif
    }

    //--------------------------------------------------------------------------
    EASTLAllocator::EASTLAllocator(const eastl::allocator&, const char* pName) :
      allocator_(&Memory::default_allocator())
    {
      PS_UNUSED_IN_RELEASE(pName);

#if EASTL_NAME_ENABLED
      name_ = pName ? pName : EASTL_ALLOCATOR_DEFAULT_NAME;
#endif
    }

    //--------------------------------------------------------------------------
    EASTLAllocator::~EASTLAllocator()
    {

    }

    //--------------------------------------------------------------------------
    EASTLAllocator& EASTLAllocator::operator=(const EASTLAllocator& x)
    {
      allocator_ = x.allocator_;
      return *this;
    }

    //--------------------------------------------------------------------------
    bool EASTLAllocator::operator==(const EASTLAllocator&)
    {
      return true;
    }

    //--------------------------------------------------------------------------
    void* EASTLAllocator::allocate(size_t n, int)
    {
      return foundation::Memory::Allocate(n, 16, allocator_);
    }

    //--------------------------------------------------------------------------
    void* EASTLAllocator::allocate(size_t n, size_t alignment, size_t, int)
    {
      return foundation::Memory::Allocate(n, alignment, allocator_);
    }

    //--------------------------------------------------------------------------
    void EASTLAllocator::deallocate(void* ptr, size_t /*size*/)
    {
      foundation::Memory::Deallocate(ptr);
    }

    //--------------------------------------------------------------------------
    void EASTLAllocator::set_name(const char* name)
    {
      PS_UNUSED_IN_RELEASE(name);

#if EASTL_NAME_ENABLED
      name_ = name;
#endif
    }

    //--------------------------------------------------------------------------
    const char* EASTLAllocator::name() const
    {
#if EASTL_NAME_ENABLED
      return name_;
#else
      return "Custom EASTL allocator";
#endif
    }
  }
}
