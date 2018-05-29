#pragma once
#include "tools/networking/network_value.h"
#include "tools/networking/network_value_owner.h"

namespace sulphur
{
  namespace networking
  {
    using SyncValueID = uint16_t; //!< SyncValueID type define

    /**
    * @class sulphur::networking::SyncableNetworkValue
    * @brief syncable network value description
    * @author Joren Bolhuis
    */
    struct SyncableNetworkValue
    {
      bool updated; //!< If the value has been updated, but not been send yet
      NetworkValue value; //!< A copy of the data including the type, used for comparing
    };

    /**
    * @class sulphur::networking::SyncValueIDWithOwner
    * @brief Sync value struct with owner, used to track ownership
    * @author Joren Bolhuis
    */
    struct SyncValueIDWithOwner
    {
      SyncValueID id; //!< The id
      NetworkValueOwner owner; //!< The owner

      /**
      * @brief Check if this object has the correct id
      * @param[in] other (const sulphur::networking::SyncValueID& id) The id to compare
      * @return (bool) Whether they are the same
      */
      bool operator==(const SyncValueID& other)
      {
        if (id == other)
        {
          return true;
        }
        return false;
      }
    };
  }
}