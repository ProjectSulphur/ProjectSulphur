#pragma once

#include "engine/core/entity_system.h"
#include "engine/systems/component_system.h"
#include "engine/graphics/irenderer.h"

#include <foundation/utils/color.h>
#include <foundation/containers/vector.h>
#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace engine
  {
    class LightSystem;

    enum struct LightType
    {
      kDirectionalLight,
      kSpotLight,
      kPointLight
    };
    /**
    * @brief An enumerator that is an index of the element. Should be in sync with @see sulphur::engine::LightData::ComponentSystemData
    */
    enum struct LightComponentElements
    {
      kColor,
      kIntensity,
      kRange,
      kSpotAngle,
      kLightType,
      kEntity
    };
    /**
    * @brief The data of the light component.
    * @author Raymi Klingers
    */
    class LightData
    {
    public:
      /**
      * @brief Alias of the system data. The order of the member variables should be in sync with @see sulphur::engine::LightComponentElements and the pointers in this 
      */
      using ComponentSystemData = SystemData<foundation::Color, float, float, float, LightType, Entity>;
      
      /**
      * @brief Constructor of the light data that initializes the system data by passing it the list of pointers. It uses the first element for this and assumes that the others follow
      */
      LightData()
        :
        data((void**)&color)
      {
      }

      foundation::Color* color;//!< Simple direct access to the color array data.
      float* intensity;//!< Simple direct access to the intensity array data.
      float* range;//!< Simple direct access to the range array data.
      float* spot_angle;//!< Simple direct access to the spot_angle array data.
      LightType* type;//!< Simple direct access to the type array data.
      Entity* entity;//!< Simple direct access to the entity array data.
      ComponentSystemData data;//!< System data of the component.
    };

    class LightComponent : public ComponentHandleBase
    {
    public:
      using System = LightSystem;//!< System alias used to get the system from handles
      template<LightComponentElements Index>
      using Element = LightData::ComponentSystemData::element_type<static_cast<size_t>(Index)>;//!< Get element index alias

      /**
      * @brief Default constructor (creates an empty/invalid handle)
      */
      LightComponent();
      /**
      * @brief Constructor for creating a handle from an integral value
      * @param[in] system (sulphur::engine::LightComponent::System&) A reference to the relevant system that owns this component
      * @param[in] handle (size_t) The integral value to use for constructing the handle
      */
      LightComponent(System& app, size_t handle);

      /*
      * @brief Get function to obtain an element from the component by index
      * @tparam LightComponentElements (Index) element index
      * @author Raymi Klingers
      */
      template<LightComponentElements Index>
      Element<Index> Get(LightComponent handle);

      /*
      * @brief Set function to set an element from the component by index
      * @tparam LightComponentElements (Index) element index
      * @author Raymi Klingers
      */
      template<LightComponentElements Index>
      void Set(LightComponent handle, Element<Index> value);

    private:
      LightSystem* system_;

    };

    class LightSystem : public IComponentSystem<LightComponent, LightData>
    {
    public:
      /**
      * @brief Default constructor
      */
      LightSystem();

      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;

      LightComponent Create(Entity entity) override;
      void Destroy(LightComponent handle) override;

      void OnUpdate(float) override;
      void OnPreRender() override;
      

      //----------------------------------------Component functions------------------------------------------------------
      /**
      * @brief Get function to obtain an element from a component by index
      * @tparam LightComponentElements (Index) element index
      * @author Raymi Klingers
      */
      template<LightComponentElements Index>
      LightComponent::Element<Index> Get(LightComponent handle)
      {
        return component_data_.data.Get<static_cast<size_t>(Index)>(handle);
      }
      /**
      * @brief Set function to set an element from a component by index
      * @tparam LightComponentElements (Index) element index
      * @author Raymi Klingers
      */
      template<LightComponentElements Index>
      void Set(LightComponent handle, LightComponent::Element<Index> value)
      {
        component_data_.data.Get<static_cast<size_t>(Index)>(handle) = value;
      }

    private:
      IRenderer* renderer_;

    };
    template<LightComponentElements Index>
    inline LightComponent::Element<Index> LightComponent::Get(LightComponent handle)
    {
      return system_->Get<static_cast<size_t>(Index)>();
    }
    template<LightComponentElements Index>
    inline void LightComponent::Set(LightComponent handle, Element<Index> value)
    {
      system_->Set<static_cast<size_t>(Index)>(value);
    }
  }
}