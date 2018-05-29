#pragma once
#include "engine/core/entity_system.h"
#include "engine/systems/component_system.h"

#include "engine/audio/audio_vca.h"

#include "engine/scripting/scriptable_object.h"
#include "engine/audio/audio_system.h"


namespace sulphur
{
  namespace engine
  {
    class AudioVCASystem;

    /**
    * @class sulphur::engine::AudioVCAComponent
    * @brief Component that interfaces to an audio vca
    * @author Rodi Lankester
    */
    SCRIPT_CLASS() class AudioVCAComponent : public ComponentHandleBase
    {
    public:
      SCRIPT_NAME(AudioVCAComponent);
      SCRIPT_COMPONENT();

      using System = AudioVCASystem; //!< The system type that this component uses

      /**
      * @brief Constructs an audio vca component
      */
      AudioVCAComponent();

      /**
      * @brief Constructs an audio vca component with a system and a handle
      * @param[in] system (Sulphuir::Engine::System&) The system that created this component
      * @param[in] handle (size_t) the handle that belongs to this component
      */
      AudioVCAComponent(System& system, size_t handle);

      /**
      * @brief Set the bank where to load the event from
      * @param[in] bank (Sulphur::foundation::String) The bank name
      */
      SCRIPT_FUNC() void SetBank(foundation::String bank);

      /**
      * @brief Set the path where to load the event from
      * @param[in] bank (Sulphur::foundation::String) The path name
      */
      SCRIPT_FUNC() void SetPath(foundation::String path);

      /**
      @brief Get the current bank name
      @returns (Sulphur::foundation::String) The current bank name
      */
      SCRIPT_FUNC() foundation::String GetBank();
      /**
      @brief Get the current path
      @returns (Sulphur::foundation::String) The current path
      */
      SCRIPT_FUNC() foundation::String GetPath();
      
      /**
      * @brief Get the current volume of the event
      * @returns (float) The current volume of the event
      */
      SCRIPT_FUNC() float GetVolume();

      /**
      * @brief Set the volume of the event
      * @param[in] volume (float) The volume to set it to
      */
      SCRIPT_FUNC() void SetVolume(float volume);

    private:
      AudioVCASystem* system_; //!< The system that belongs to this event
    };

    /**
    * @class sulphur::engine::AudioVCAData
    * @brief The data belonging to audio vca components
    * @author Rodi Lankester
    */
    class AudioVCAData
    {
    public:

      /**
      * @brief Create an instance of AudioVCAData
      */
      AudioVCAData()
        : data((void**)&bank_path)
      {

      }

      using ComponentSystemData = SystemData <
        foundation::String,
        foundation::String,
        AudioVCA*,
        AudioBank*,
        bool,
        Entity
      >; //!< Specifies the data types to use

      foundation::String* bank_path; //!< The path to the current audio bank
      foundation::String* vca_path; //!< The current event path
      AudioVCA** audio_vca; //!< The instance of the currently loaded event
      AudioBank** audio_bank; //!< The currently selected audio bank
      bool* loaded; //!< Is this event loaded
      ComponentSystemData data; //!< The actual data container of this component
    };

    /**
    * @class sulphur::engine::AudioVCASystem
    * @brief The system belonging the the audio vca component
    * @author Rodi Lankester
    */
    class AudioVCASystem : public IComponentSystem
    {
    public:
      /**
      * @brief Constructs an audio vca system
      */
      AudioVCASystem();

      /**
      * @see sulphur::engine::IComponentSystem::Create
      */
      template<typename ComponentT>
      ComponentT Create(Entity& entity) { return Create(entity); };

      /**
      * @brief Create an audio vca component belonging to the passed entity
      * @param[in] entity (Entity&) The entity this component belongs to
      * @returns (Sulphur::Engine::AudioVCAComponent) The created audio vca component
      */
      AudioVCAComponent Create(Entity& entity);

      /**
      * @brief Destroy an audio vca component
      * @param[in] handle (Sulphur::Engine::AudioVCAComponent) The component to destroy
      */
      void Destroy(ComponentHandleBase handle) override;

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
      * @brief Set the bank where to load the event from
      * @param[in] handle (Sulphur::Engine::AudioVCAComponent) The handle of the component to perform this action for
      * @param[in] bank (Sulphur::foundation::String) The bank name
      */
      SCRIPT_FUNC() void SetBank(AudioVCAComponent handle, foundation::String bank);
      /**
      * @brief Set the path of the event
      * @param[in] handle (Sulphur::Engine::AudioVCAComponent) The handle of the component to perform this action for
      * @param[in] bank (Sulphur::foundation::String) The path
      */
      SCRIPT_FUNC() void SetPath(AudioVCAComponent handle, foundation::String path);

      /**
      @brief Get the current bank name
      * @param[in] handle (Sulphur::Engine::AudioVCAComponent) The handle of the component to perform this action for
      @returns (Sulphur::foundation::String) The current bank name
      */
      SCRIPT_FUNC() foundation::String GetBank(AudioVCAComponent handle);
      /**
      @brief Get the current path
      * @param[in] handle (Sulphur::Engine::AudioVCAComponent) The handle of the component to perform this action for
      @returns (Sulphur::foundation::String) The current path
      */
      SCRIPT_FUNC() foundation::String GetPath(AudioVCAComponent handle);

      /**
      * @brief Get the current volume of the event
      * @param[in] handle (Sulphur::Engine::AudioVCAComponent) The handle of the component to perform this action for
      * @returns (float) The current volume of the event
      */
      float GetVolume(AudioVCAComponent handle);

      /**
      * @brief Set the volume of the event
      * @param[in] handle (Sulphur::Engine::AudioVCAComponent) The handle of the component to perform this action for
      * @param[in] volume (float) The volume to set it to
      */
      void SetVolume(AudioVCAComponent handle, float volume);
    private:

      AudioSystem* audio_system_; //!< The instance of the audio system
      AudioVCAData component_data_; //!< The component data
    };
  }
}