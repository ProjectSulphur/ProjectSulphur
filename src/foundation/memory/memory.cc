#include "memory.h"
#include "allocators/allocator.h"
#include "foundation/logging/logger.h"

#include "foundation/utils/native.h"

namespace sulphur
{
  namespace foundation
  {
    using MemoryLogger = Logger<LoggingChannel::kMemory, DefaultFormat, DefaultTarget>;

    //--------------------------------------------------------------------------
    GeneralAllocator Memory::default_allocator_;
    bool Memory::initialized_ = false;

    //--------------------------------------------------------------------------
    void Memory::Initialize(size_t heap_size)
    {
      void* addr = Native::InitializeMemoryPool(heap_size);
      initialized_ = true;

      default_allocator_ = GeneralAllocator(addr, heap_size);
    }

    //--------------------------------------------------------------------------
    IAllocator& Memory::default_allocator()
    {
      return default_allocator_;
    }

    //--------------------------------------------------------------------------
    void* Memory::Allocate(size_t size, size_t alignment, IAllocator* allocator)
    {
      if (allocator == nullptr)
      {
        allocator = &default_allocator();
      }

      size_t header_size = sizeof(MemoryHeader);
      void* base = reinterpret_cast<MemoryHeader*>(
        allocator->Allocate(size + sizeof(MemoryHeader) + alignment - 1, alignment));

      void* ptr = OffsetBytes(base, header_size);
      size_t a = AlignUpDelta(ptr, alignment);

      ptr = OffsetBytes(ptr, a);

      MemoryHeader* header = reinterpret_cast<MemoryHeader*>(OffsetBytes(base, a));

      header->allocator = allocator;
      header->size = size;
      header->alignment = a;

      return ptr;
    }

    //--------------------------------------------------------------------------
    void Memory::Deallocate(const void* ptr)
    {
      MemoryHeader* header = reinterpret_cast<MemoryHeader*>(
        OffsetBytes(ptr, -static_cast<int>(sizeof(MemoryHeader))));

      header->allocator->Deallocate(OffsetBytes(
          reinterpret_cast<void*>(header), 
          -static_cast<intptr_t>(header->alignment)
        ));
    }

    //--------------------------------------------------------------------------
    bool Memory::IsInitialized()
    {
      if (initialized_ == false)
      {
        printf("[Memory] Memory is not initialized, all containers will be unavailable\n");
      }

      return initialized_;
    }

    //--------------------------------------------------------------------------
    void Memory::Shutdown()
    {
      default_allocator_.Shutdown();
    }

    //--------------------------------------------------------------------------
    void Memory::FallBack(IAllocator*& allocator)
    {
      if (allocator == nullptr)
      {
        allocator = &default_allocator();
      }
    }
    //--------------------------------------------------------------------------
    void Memory::LogWarning(const char* message)
    {
      PS_LOG_WITH(MemoryLogger, Warning, message);
    }
  }
}
