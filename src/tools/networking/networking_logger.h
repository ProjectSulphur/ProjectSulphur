#pragma once
#include <foundation/logging/logger.h>
using NetworkLogger = 
sulphur::foundation::Logger<sulphur::foundation::LoggingChannel::kNetworking, 
  sulphur::foundation::DefaultFormat, sulphur::foundation::DefaultTarget>;