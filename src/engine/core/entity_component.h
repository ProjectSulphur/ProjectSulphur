#pragma once

#include "handle_base.h"

#include <foundation/containers/vector.h>

namespace sulphur
{
  namespace engine
  {
    /**
    * @class sulphur::engine::EntityComponentData
    * @brief Storage for the component and its type info
    * @todo Store component types as uint8?
    * @todo Move this somewhere else and move some of the functionality that currently is in the entity manager into here.
    * @author Raymi Klingers
    */
    class EntityComponentData
    {
    public:
      foundation::Vector<size_t> component_types;//!< Stores the types of the components
      foundation::Vector<ComponentHandleBase> component_handles;//!< Stores the handles of the components
    };
  }
}