#pragma once
#include "foundation/containers/string.h"

namespace sulphur
{
	namespace foundation
	{
		class SystemPath
		{
		public:
			SystemPath(const String& path);
			
			operator String() const;
      operator const char*() const;
			
		private:
			String path_;
		};
	}
}
