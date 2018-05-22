#include "foundation/io/filesystem.h"

namespace sulphur
{
  namespace foundation
  {
    SystemPath::SystemPath(const String& path)
    {
      path_ = path;
    }

    SystemPath::operator String() const
    {
      return path_;
    }

    SystemPath::operator const char*() const
    {
      return path_.c_str();
    }
  }
}