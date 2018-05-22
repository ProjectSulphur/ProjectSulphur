#include <Windows.h>
#include <iostream>
#include <sstream>

void GenerateErrorMessage(long hr)
{
  LPSTR messageBuffer = nullptr;
  size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

  std::string message(messageBuffer, size);

  //Free the buffer.
  LocalFree(messageBuffer);

  std::ostringstream oss;
  oss << "Unable to create Direct3D environment.\nError code: 0x" << std::hex << hr << std::endl << message << std::endl;
  MessageBoxA(nullptr, oss.str().c_str(), "Error", MB_ICONERROR);
}