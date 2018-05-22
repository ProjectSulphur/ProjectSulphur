#pragma once

#ifdef PS_WIN32
#pragma warning(push, 0)
#include <btBulletDynamicsCommon.h>
#pragma warning pop
#elif defined (PS_PS4)
#pragma clang system_header
#include <btBulletDynamicsCommon.h>
#endif