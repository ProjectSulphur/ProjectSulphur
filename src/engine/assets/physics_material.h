#pragma once
#include "engine/assets/asset_interfaces.h"

#include <physics/physics_collider.h>

namespace sulphur 
{
  namespace engine 
  {
    /**
    * @class sulphur::engine::PhysicsMaterial
    * @brief A set of properties for a physics material that can be applied to a collider as a preset.
    * @author Angelo van der Mark
    */
    class PhysicsMaterial
    {
    public:
      /**
      * @brief Default constructor, creates a default physics material.
      */
      PhysicsMaterial();

      /**
      * @brief Sets the static friction coefficient for this material.
      * @param[in] friction (float) The static friction coefficient.
      */
      void set_static_friction(float friction);

      /**
      * @brief Returns the static friction coefficient of this material.
      * @returns (float) The static friction coefficient.
      */
      float static_friction() const;

      /**
      * @brief Sets the dynamic friction coefficient for this material.
      * @param[in] friction (float) The dynamic friction coefficient.
      */
      void set_dynamic_friction(float friction);

      /**
      * @brief Returns the dynamic friction coefficient of this material.
      * @returns (float) The dynamic friction coefficient.
      */
      float dynamic_friction() const;

      /**
      * @brief Sets the rolling friction coefficient for this material.
      * @param[in] friction (float) The rolling friction coefficient.
      */
      void set_rolling_friction(float friction);

      /**
      * @brief Returns the rolling friction coefficient of this material.
      * @returns (float) The rolling friction coefficient.
      */
      float rolling_friction() const;

      /**
      * @brief Sets the friction combine mode to use for this material.
      * @param[in] mode (sulphur::physics::PhysicsCollider::MaterialCombineMode) The mode to use.
      */
      void set_friction_combine_mode(physics::PhysicsCollider::MaterialCombineMode mode);

      /**
      * @brief Gets the friction combine mode to be used by this material.
      * @returns (sulphur::physics::PhysicsCollider::MaterialCombineMode) The mode to be used.
      */
      physics::PhysicsCollider::MaterialCombineMode friction_combine_mode() const;

      /**
      * @brief Sets the bounciness for this material.
      * @note A combined value of 0 causes no bounce. A combined value of 1 will bounce without energy loss.
      * @param[in] restitution (float) The restitution.
      */
      void set_restitution(float restitution);

      /**
      * @brief Gets the bounciness of this material.
      * @note A combined value of 0 causes no bounce. A combined value of 1 will bounce without energy loss.
      * @returns (float) The restitution.
      */
      float restitution() const;

      /**
      * @brief Sets the restitution combine mode to use for this material.
      * @param[in] mode (sulphur::physics::PhysicsCollider::MaterialCombineMode) The mode to use.
      */
      void set_restitution_combine_mode(physics::PhysicsCollider::MaterialCombineMode mode);

      /**
      * @brief Gets the restitution combine mode to be used by this material.
      * @returns (sulphur::physics::PhysicsCollider::MaterialCombineMode) The mode to be used.
      */
      physics::PhysicsCollider::MaterialCombineMode restitution_combine_mode() const;

    private:

      float static_friction_; //!< This material's static friction coefficient.
      float dynamic_friction_; //!< This material's dynamic friction coefficient.
      float rolling_friction_; //!< This material's rolling friction coefficient.

      float restitution_; //!< This material's restitution.

      physics::PhysicsCollider::MaterialCombineMode friction_combine_mode_; //!< The method used to combine friction coefficients.
      physics::PhysicsCollider::MaterialCombineMode restitution_combine_mode_; //!< The method used to combine restitution values.
    };

    using PhysicsMaterialHandle = AssetHandle<PhysicsMaterial>; //!< Asset handle to a physics material.
  }
}