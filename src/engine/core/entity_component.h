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
      /**
      * @brief Constructor with an optional argument to indicate who is takes ownership of the entity
      * @param[in] with_editor (bool) Setting this to true indicates the editor instantiated this entity, rather than the game
      */
      EntityComponentData(bool with_editor = false);

      foundation::Vector<size_t> component_types;//!< Stores the types of the components
      foundation::Vector<ComponentHandleBase> component_handles;//!< Stores the handles of the components
      bool with_editor; //!< Indicates that the editor instantiated this entity, rather than the game
    };
  }
}