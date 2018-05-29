#pragma once

#include <foundation/utils/type_definitions.h>
#include <foundation/utils/generation_handle.h>

namespace sulphur
{
  namespace engine
  {
    class Application;

    /**
    * @class sulphur::engine::ComponentHandleBase : public sulphur::foundation::GenerationHandle<size_t,8>
    * @brief Base class of the component handles which stores the index and the generation.
    * @todo Refactor this mess and properly defined what should be private and not
    * @todo Add is valid check and remove the static invalidhandle function
    * @todo Properly define the constructors
    * @todo Move this into the component folder? Or its own folder
    * @todo Add generation bits so we can check for not alive handles
    * @author Raymi Klingers
    */
    class ComponentHandleBase : public foundation::GenerationHandle<size_t,8>
    {
    public:
      /**
      * @brief Constructor that creates a handle from an integral value
      * @param handle (size_t) The integral value that is used to create the handle
      */
      explicit ComponentHandleBase(size_t handle);
      /**
      * @brief Default constructor that creates an empty/invalid component handle
      */
      ComponentHandleBase();

      /**
      * @brief Gets the internal data and is also temporary
      * @return (size_t) A component handle's data
      * @todo Refactor temporary code
      */
      size_t Handle() 
      { 
        return handle; 
      };

      /**
      * @brief Checks if two components are identical
      * @param[in] other (const sulphur::engine::ComponentHandleBase&) The base handle to compare to
      * @return (bool) Are the components identical?
      * @todo Refactor Is there a better way to define this?
      */
      bool operator==(const ComponentHandleBase& other) const 
      {
        return handle == other.handle; 
      }
      /**
      * @brief Checks if two components are not identical
      * @param[in] other (const sulphur::engine::ComponentHandleBase&) The base handle to compare to
      * @return (bool) Are the components not identical?
      * @todo Refactor Is there a better way to define this?
      */
      bool operator!=(const ComponentHandleBase& other) const 
      {
        return handle != other.handle; 
      }
      /**
      * @brief Creates an invalid handle
      * @return (sulphur::engine::ComponentHandleBase) An invalid handle
      * @todo Refactor temporary code
      */
      static ComponentHandleBase InvalidHandle()
      {
        return ComponentHandleBase(PS_SIZE_MAX);
      }

      /**
      * @brief Check if this handle is valid
      * @return (bool) Is this handle valid
      */
      bool IsValid() const
      {
        return handle != PS_SIZE_MAX;
      }

      /**
      * @brief Is used by component systems to obtain a unique ID for the component. ID is used for the type info of component handles
      * @return (size_t) A return value is used so that we could run this function to initialize a static function so that it is only ran once.
      * @todo Refactor is the statement above? And fix undefined behaviour
      */
      static size_t SetupID(size_t& id)// Hacking the ID 
      {
        return id = unique_id++;
      }
      static size_t unique_id;//!< Counter to obtain unique IDs
    };

    //-------------------------------------------------------------------------
    inline ComponentHandleBase::ComponentHandleBase(size_t handle) :
      foundation::GenerationHandle<size_t, 8>(handle)
    {
    }

    //-------------------------------------------------------------------------
    inline ComponentHandleBase::ComponentHandleBase() :
      foundation::GenerationHandle<size_t, 8>(PS_SIZE_MAX)
    {
    }

  }
}