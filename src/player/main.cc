#include <engine/application/application.h>
#include <foundation/memory/memory.h>

/**
* @brief 2 Gigabytes of default memory
*/
static size_t kMemoryBlock = 2ul * 1024ul * 1024ul * 1024ul;

int main(int /*argc*/, char** /*argv*/)
{
  sulphur::foundation::Memory::Initialize(kMemoryBlock);
  sulphur::engine::Application app;

  app.Run();

  return 0;
}