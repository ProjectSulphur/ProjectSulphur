#pragma once

/**
 * @file foundation/utils/macro.h
 */

/**
 * @def PS_UNUSED_IN_RELEASE(variable)
 * @brief Mark variable an unused in release builds, 
 *        supresses unreferenced formal parameter warnings.
 * @param[in] variable The variable to mark as unused
 */

#if PS_DEBUG
  #define PS_UNUSED_IN_RELEASE(variable)
#else
  #define PS_UNUSED_IN_RELEASE(variable) (void)variable
#endif

