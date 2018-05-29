#pragma once
#include "engine/core/entity_system.h"
#include "engine/systems/component_system.h"
#include "engine/scripting/scriptable_object.h"
#include "engine/scripting/scriptable_values/scriptable_callback.h"
#include <tools/networking/syncable_network_value.h>
#include <tools/networking/rpc_data.h>
#include <tools/networking/network_value_owner.h>

namespace sulphur
{
  namespace engine
  {
    class NetworkComponentSystem;
    /**
    * @enum sulphur::engine::NetworkComponentElements elements in data
    */
    enum struct NetworkComponentElements
    {
      kEntity, //!< entity handle
      kID, //!< id handle
      kValueSyncIds, //!< valye sync ids handle
      kRPCHandles, //!< rpc handles
      kRPCScriptHandles, //!< rpc script handles
    };
    /**
    * @struct sulphur::engine::NetworkComponentData
    * @brief The data used by sulphur::engine::NetworkComponent
    * @author Joren Bolhuis
    */
    class NetworkComponentData
    {
    public:
      /**
      * @brief Alias of the system data.
      */
      using ComponentSystemData = SystemData<Entity, uint16_t, foundation::Map<foundation::String, networking::SyncValueID>, foundation::Map<foundation::String, networking::RPCHandle>, foundation::Map<uint16_t, ScriptableCallback>>;
      NetworkComponentData() :
        data((void**)&entity)
      {

      }
      Entity* entity;//!< direct acces to the entity;
      uint16_t* id;//!< direct acces to the id
      foundation::Map<foundation::String, networking::SyncValueID>* value_sync_ids;//!< direct acces to the map of value sync ids
      foundation::Map<foundation::String, networking::RPCHandle>* rpc_handles_;//!< direct acces to the map of rpc ids
      foundation::Map<uint16_t, ScriptableCallback>* rpc_script_handles_;//!< direct acces to the scriptable callback
      ComponentSystemData data;//!< System data of the component.
    };

    /**
    * @class sulphur::engine::NetworkComponent
    * @brief The component type used by the NetworkComponentSystem
    * @author Joren Bolhuis
    */
    SCRIPT_CLASS() class NetworkComponent : public ComponentHandleBase
    {
    public:
      /**
      * @brief Scriptable RPCMode
      * @see sulphur::networking::RPCMode
      */
      SCRIPT_ENUM() enum RPCMode
      {
        kAll, //!< Calls function locally and to all other clients, if executed on a client, the message will be send to the host and then distributed to all the other clients except yourself
        kOthers, //!< Sends a call to all clients except yourself, the message will be send to the host and then distributed to all the other clients
        kServer, //!< Calls function on server, if you are the server, it will be called locally
        kBufferedAll, //!<Call functionality just like kAll, but the RPC is buffered for clients that join later. Buffered rpcs are only supported on the host
        kBufferedOthers //!<Call functionality just like kOthers, but the RPC is buffered for clients that join later. Buffered rpcs are only supported on the host
      };

      /**
      * @brief Scriptable ValueType
      * @see sulphur::networking::NetworkValueType
      */
      SCRIPT_ENUM() enum NetworkValueType
      {
        kFloat, //!< type float
        kDouble, //!< type double
        kInt, //!< type int
        kUnsignedInt, //!< type unsigned int
        kBool, //!< type bool
        kVec2, //!< type vector2
        kVec3, //!< type vector3
        kVec4, //!< type vector4
        kInt16, //!< type int 16
        kInt8, //!< type int 8
        kUnsignedInt16, //!< type unsigned int 16
        kUnsignedInt8, //!< type unsigned int 8
        kString //!< type string of "kStringLength" chars
      };

      /**
      * @brief Scriptable NetworkValueOwner
      * @see sulphur::networking::NetworkValueOwner
      */
      SCRIPT_ENUM() enum NetworkValueOwner
      {
        kHost, //!< If the host should be the owner of this value
        kMe, //!< If I should be the owner of this value
        kOther //!< If someone else is the owner of this value
      };

      SCRIPT_NAME(NetworkComponent);
      SCRIPT_COMPONENT();

      using System = NetworkComponentSystem; //!< System type define
      /**
      * @brief Default constructor (creates an empty/invalid handle)
      */
      NetworkComponent();
      /**
      * @brief Constructor for creating a handle from an integral value
      * @param[in] system (sulphur::engine::System&) A reference to the relevant system that owns this component
      * @param[in] handle (size_t) The integral value to use for constructing the handle
      */
      NetworkComponent(System& system, size_t handle);
      /**
      * @brief Gets the network id
      * @return (uint16_t) The id
      */
      SCRIPT_FUNC() uint16_t GetNetworkID();
      /**
      * @brief Binds a sync value to a component
      * @see sulphur::networking::SNetCreateSyncValue
      * @param[in] name (const sulphur::foundation::String&) The name of the value
      * @param[in] type (const sulphur::networking::NetworkValueType) The type
      * @param[in] owner (sulphur::networking::NetworkValueOwner)
        The owner of the value, defaults to host
      */
      void RegisterSyncValue(const foundation::String& name, 
        const networking::NetworkValueType type,
        networking::NetworkValueOwner owner = networking::NetworkValueOwner::kHost);
      /**
      * @brief Sets the sync value of a component
      * @see sulphur::networking::SNetSetSyncValue
      * @param[in] name (const sulphur::foundation::String&) The name of the value
      * @param[in] value (const sulphur::networking::NetworkValue) The value
      */
      void SetSyncValueEngine(const foundation::String& name, const networking::NetworkValue value);
      /**
      * @brief Gets the sync value of a component
      * @see sulphur::networking::SNetGetSyncValue
      * @param[in] name (const sulphur::foundation::String&) The name of the value
      * @return (const sulphur::networking::NetworkValue) The value
      */
      networking::NetworkValue GetSyncValueEngine(const foundation::String& name);
      /**
      * @brief Register Sync Value used by lua
      * @see sulphur::engine::NetworkComponent::RegisterSyncValue
      */
      SCRIPT_FUNC() void RegisterSyncValue(const foundation::String& name, const NetworkComponent::NetworkValueType type, const NetworkComponent::NetworkValueOwner owner);
      /**
      * @brief Register Sync Value used by lua
      * @see sulphur::engine::NetworkComponent::GetSyncValueEngine
      */
      SCRIPT_FUNC() ScriptHandle GetSyncValue(const foundation::String& name);
      /**
      * @brief Register Sync Value used by lua
      * @see sulphur::engine::NetworkComponent::SetSyncValueEngine
      */
      SCRIPT_FUNC() void SetSyncValue(const foundation::String& name, ScriptHandle value);
      /**
      * @brief Register an rpc to a component
      * @param[in] name (const foundation::String&) The name used to invoke the RPC
      * @param[in] function (eastl::function<void(sulphur::networking::RPCHandle, sulphur::networking::NetworkPlayerData, sulphur::foundation::Vector<sulphur::networking::NetworkValue>)>) The function pointer
      * @param[in] arguments (const sulphur::foundation::Vector<NetworkValueType>&) The arguments
      * @see sulphur::networking::SNetRegisterRPC
      */
      void RegisterRPC(const foundation::String& name, eastl::function<void(networking::RPCHandle,
        networking::NetworkPlayerData, const foundation::Vector<networking::NetworkValue>&)> function,
        const foundation::Vector<networking::NetworkValueType>& arguments);
      /**
      * @brief Invoke rpc that is attached to this component
      * @param[in] name (const foundation::String&) The name of the rpc
      * @param[in] mode (const networking::RPCMode) The mode the rpc needs to use
      * @param[in] arguments (const sulphur::foundation::Vector<sulphur::networking::NetworkValue>&) The arguments to send with the rpc
      */
      void InvokeRPC(const foundation::String& name, const networking::RPCMode mode,
        const foundation::Vector<networking::NetworkValue>& arguments);
      /**
      * @brief Register RPC used by lua, custom implementation for Lua was needed
      * @param[in] args (sulphur::engine::ScriptableArgs*) The args
      */
      SCRIPT_FUNC() void RegisterRPC(ScriptableArgs* args);
      /**
      * @brief Invoke RPC used by lua, custom implementation for Lua was needed
      * @param[in] args (sulphur::engine::ScriptableArgs*) The args
      */
      SCRIPT_FUNC() void InvokeRPC(ScriptableArgs* args);
    private:
      NetworkComponentSystem* system_; //!< The network component system
    };

    /**
    * @class sulphur::engine::NetworkComponentSystem : public IComponentSystem< NetworkComponent, NetworkComponentData >
    * @brief Manages NetworkComponents
    * @see sulphur::engine::NetworkComponent
    * @author Joren Bolhuis
    */
    class NetworkComponentSystem : public IComponentSystem
    {
    public:
      /**
      * @brief Scriptable ValueType
      * @see sulphur::networking::NetworkValueOwner
      */
      SCRIPT_ENUM() enum NetworkValueOwner
      {
        kHost, //!< If the host should be the owner of this value
        kMe, //!< If I should be the owner of this value
        kOther //!< If someone else is the owner of this value
      };

      /**
      * @brief Default constructor
      */
      NetworkComponentSystem();
      /**
      * @internal
      * @see sulphur::engine::IComponentSystem::OnInitialize
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;
      /**
      * @internal
      * @see sulphur::engine::IComponentSystem::OnTerminate
      */
      void OnTerminate() override;
      /**
      * @see sulphur::engine::IComponentSystem::Create
      */
      template<typename ComponentT>
      ComponentT Create(Entity& entity) { return Create(entity); };
      /**
      * @brief Destroys the network component
      * @param[in] handle (sulphur::engine::NetworkComponent) The component to destroy
      * @see sulphur::engine::IComponentSystem::Destroy
      */
      void Destroy(ComponentHandleBase handle) override;
      /**
      * @brief Create a new network component for this entity
      * @param[in] entity (sulphur::engine::Entity) The entity to create this component for
      * @see sulphur::engine::IComponentSystem::Create
      */
      NetworkComponent Create(Entity& entity);
      /**
      * @brief Gets the network id
      * @param[in] handle (sulphur::engine::NetworkComponent) The component handle
      * @return (uint16_t) The id
      */
      uint16_t GetNetworkID(NetworkComponent handle);
      /**
      * @brief Register the ID of the synced value that belongs to this object
      * @param[in] handle (sulphur::engine::NetworkComponent) The component handle
      * @param[in] name (sulphur::foundation::String&) The name of the sync value
      * @param[in] id (sulphur::networking::SyncValueID) The id of the sync value
      */
      void AddSyncValue(NetworkComponent handle, const foundation::String& name, 
        networking::SyncValueID id);
      /**
      * @brief Set the sync value
      * @param[in] handle (sulphur::engine::NetworkComponent) The component handle
      * @param[in] name (const sulphur::foundation::String&) The name of the sync value
      * @param[in] value (const sulphur::networking::NetworkValue&) The new value of the sync value
      */
      void SetSyncValue(NetworkComponent handle, const foundation::String& name, 
        const networking::NetworkValue& value);
      /**
      * @brief Set the sync value
      * @param[in] handle (sulphur::engine::NetworkComponent) The component handle
      * @param[in] name (const sulphur::foundation::String&) The name of the sync value
      * @return (const sulphur::networking::NetworkValue&) The value of the syncvalue
      */
      networking::NetworkValue GetSyncValue(NetworkComponent handle, const foundation::String& name);
      /**
      * @brief Register the ID of the synced value that belongs to this object
      * @param[in] handle (sulphur::engine::NetworkComponent) The component handle
      * @param[in] name (const foundation::String&) The string handle to invoke the rpc
      * @param[in] rpc_handle (sulphur::networking::RPCHandle) The handle to the rpc
      */
      void RegisterRPC(NetworkComponent handle, const foundation::String& name,
        networking::RPCHandle rpc_handle);
      /**
      * @brief Retrieve the RPCHandle from the handle map
      * @param[in] handle (sulphur::engine::NetworkComponent) The component handle
      * @param[in] name (const sulphur::foundation::String&) The rpc name
      * @param[out] h (sulphur::networking::RPCHandle&) The handle, if the call succeeded
      * @return (bool) Whether the call succeeded
      */
      bool GetRPCHandle(NetworkComponent handle, const foundation::String& name, 
        networking::RPCHandle& h);
      /**
      * @brief Add Script RPC function to the lookuptable
      * @param[in] handle (sulphur::engine::NetworkComponent) The component handle
      * @param[in] id (uint16_t) The id to register
      * @param[in] function (sulphur::engine::ScriptableCallback) The function pointer
      */
      void AddScriptRPC(NetworkComponent handle, uint16_t id, ScriptableCallback function);
      /**
      * @brief Retrieves the Script RPC function pointer
      * @param[in] handle (sulphur::engine::NetworkComponent) The component handle
      * @param[in] id (uint16_t) The id to lookup
      * @return (sulphur::engine::ScriptableCallback) The function pointer
      */
      ScriptableCallback* GetScriptRPC(NetworkComponent handle, uint16_t id);
      /**
      * @brief Converts a networkvalue to a scripthandle
      * @param[in] value (const sulphur::networking::NetworkValue&) The component handle
      * @return (sulphur::engine::ScriptHandle) The handle
      */
      ScriptHandle ConvertNetworkValueToScriptHandle(const networking::NetworkValue& value);
      /**
      * @brief Converts a scripthandle to a networkvalue
      * @param[in] handle (sulphur::engine::ScriptHandle) The handle
      * @param[in] type (sulphur::networking::NetworkValueType) The type
      * @return (sulphur::networking::NetworkValue) The value
      */
      networking::NetworkValue ConvertScriptHandleToNetworkValue(ScriptHandle handle, 
        networking::NetworkValueType type);
      /**
      * @brief Gets script state
      * @return (sulphur::engine::ScriptState*) The state
      */
      ScriptState* GetScriptState();
    private:
      uint16_t last_network_id_; //!< last ID assigned to a networkcomponent
      ScriptState* script_state_; //!< A pointer to the script state

      NetworkComponentData component_data_; //!< An instance of the container that stores per-component data
    };
  }
}