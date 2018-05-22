#pragma once

namespace sulphur 
{
  namespace foundation 
  {
    /**
     * @enum sulphur::foundation::AccessType
     * @brief An enumerator for indicating read and write access
     * @author Kenneth Buijssen
     */
    enum struct AccessType
    {
      kRead = 0, //!< Read only access
      kWrite = 1 //!< Read and write access
    };
  }
}