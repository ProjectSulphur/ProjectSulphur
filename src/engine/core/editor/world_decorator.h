#pragma once 
#include "engine/networking/editor/editor_listener.h"
#include "engine/systems/owner_system.h"
#include "engine/core/world.h"

namespace sulphur
{
  namespace foundation
  {
    class JobGraph;
  }

  namespace engine
  {
    class Application;

    namespace editor
    {
      /**
      * @class sulphur::engine::editor::WorldDecorator : sulphur::engine::editor::IEditorListener
      * @brief Class that extends the engine world class with editor specific functionality.
      * @remark This class is only available when the engine is compiled in editor mode.
      * @see sulphur::engine::World
      * @author Stan Pepels
      */
      class WorldDecorator : public IEditorListener
      {
      public:

        /**
        * @brief Constructor that sets the currentworld to extend the functionality to.
        * @param[in] world (sulphur::engine::world) world apply the operations to.
        */
        WorldDecorator(World* world);

        /**
        * @brief Destructor
        */
        ~WorldDecorator();

        /**
        * @see sulphur::engine::editor::IEditorListener::OnReceive
        */
        void OnReceive(EditorMessageID id, const MessagePayload& payload) override;

        /**
        * @brief Initualizes this class and creates the additional editor systems.
        * @param[in] app (sulphur::engine::Application&) The application to pass to the systems
        * @param[in] job_graph (sulphur::foundation::JobGraph&) Job graph used to schedule execution of jobs during the frame
        */
        void Initialize(Application& app, foundation::JobGraph& job_graph);

        /**
        * @brief Get the self-owned system T in from the world.
        * @return (T&) A reference to the system instance.
        * @remarks T Must be derived from sulphur::engine::IOwnerSystemBase.
        */
        template<typename T>
        T& GetWorldOwner()
        {
          return world_->GetOwner<T>();
        }

        /**
        * @brief Get the self-owned system T from the world.
        * @return (T&) A const-reference to the system instance.
        * @remarks T Must be derived from sulphur::engine::IOwnerSystemBase.
        */
        template<typename T>
        const T& GetWorldOwner() const
        {
          return world_->GetOwner<T>();
        }

        /**
        * @brief Get a self-owned system from the world by type-id
        * @param[in] idx (size_t) The index of the system. This can be obtained using
        *                          foundation::type_id<T>() for all self-owned systems.
        * @return (sulphur::engine::IOwnerSystemBase&) A reference to the system instance without
        *   type information
        */
        IOwnerSystemBase& GetWorldOwner(size_t idx)
        {
          return world_->GetOwner(idx);
        }

        /**
        * @brief Get the component system T from the world
        * @return (T&) A reference to the system instance
        * @remarks T must be derived from sulphur::engine::IComponentSystemBase
        */
        template<typename T>
        T& GetWorldComponent()
        {
          return world_->GetComponent<T>();
        }

        /**
        * @brief Get the component system T from the world
        * @return (T&) A const-reference to the system instance
        * @remarks T must be derived from sulphur::engine::IComponentSystemBase
        */
        template<typename T>
        const T& GetWorldComponent() const
        {
          return world_->GetComponent<T>();
        }

        /**
        * @brief Get a component system from the world by type-id
        * @param[in] idx (size_t) The index of the component system. This can be obtained using
        *                         foundation::type_id<T>() for all component systems.
        * @return (sulphur::engine::IComponentSystemBase&) A reference to the system instance without
        *   type information
        */
        IComponentSystem& GetWorldComponent(size_t idx)
        {
          return world_->GetComponent(idx);
        }

        /**
        * @brief Get the editor specific self-owned system T associated with the current world 
        * @return (T&) A reference to the system instance.
        * @remarks T Must be derived from sulphur::engine::IOwnerSystemBase.
        */
        template<typename T>
        T& GetOwner()
        {
          static_assert(std::is_base_of<IOwnerSystemBase, T>::value,
            "T should be derived from sulphur::engine::IOwnerSystemBase");
          return owners_.Get<T>();
        }

        /**
        * @brief Get the editor specific self-owned system T associated with the current world 
        * @return (T&) A const-reference to the system instance
        * @remarks T must be derived from sulphur::engine::IOwnerSystemBase
        */
        template<typename T>
        const T& GetOwner() const
        {
          static_assert(std::is_base_of<IOwnerSystemBase, T>::value,
            "T should be derived from sulphur::engine::IOwnerSystemBase");
          return owners_.Get<T>();
        }

        /**
        * @brief Get a editor specific self-owned system associated with the current world by type-id
        * @param[in] idx (size_t) The index of the system. This can be obtained using
        *                         foundation::type_id<T>() for all self-owned systems.
        * @return (sulphur::engine::IOwnerSystemBase&) A reference to the system instance without
        *   type information
        */
        IOwnerSystemBase& GetOwner(size_t idx)
        {
          return owners_.Get(idx);
        }

        /**
        * @brief Set the current world to use.
        * @param[in] world (sulphur::engine::World*) A world created by the sulphur::engine::WorldProviderSystem
        */
        void set_world(World* world);

        /**
        * @ brief Release the allocated resources allocated in the Initialize function. For every Initialize call there should be a Release call.
        */
        void Release();
      private:
        World* world_; //!< World instance to extend the functionality to.
        SystemSet<IOwnerSystemBase> owners_; //!< Editor specific owner systems. only available to the world when in editor mode
        foundation::Vector<IEditorListener*> listeners_; //!< List of listeners. These listeners are extensions of the regular world component systems.
      };
    }
  }
}