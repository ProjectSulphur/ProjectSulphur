#pragma once

#include "physics/physics_collider.h"

namespace sulphur
{
  namespace physics
  {

    /**
    * @class sulphur::physics::BulletCollider : public sulphur::physics::PhysicsCollider
    * @brief The Bullet implementation of shape instances.
    * @author Angelo van der Mark
    */
    class BulletCollider : public PhysicsCollider
    {
    public:

      /**
      * @see sulphur::physics::PhysicsCollider
      * @param[in] owner (sulphur::physics::IPhysicsBody*) The physics body this collider is attached to.
      * @param[in] shape (sulphur::physics::PhysicsShape*) The shape of this collider.
      */
      BulletCollider(IPhysicsBody* owner, PhysicsShape* shape);

      /**
      * @brief Function intended to update the collider and/or PhysicsBody if required by the implementation.
      * @see sulphur::physics::PhysicsCollider::OnShapeUpdate
      */
      void OnShapeUpdate() override;

      /**
      * @see sulphur::physics::PhysicsCollider::SetStaticFriction
      */
      void SetStaticFriction(float friction) override;

      /**
      * @see sulphur::physics::PhysicsCollider::GetStaticFriction
      */
      float GetStaticFriction() const override;

      /**
      * @see sulphur::physics::PhysicsCollider::SetDynamicFriction
      */
      void SetDynamicFriction(float friction) override;

      /**
      * @see sulphur::physics::PhysicsCollider::GetDynamicFriction
      */
      float GetDynamicFriction() const override;

      /**
      * @see sulphur::physics::PhysicsCollider::SetRollingFriction
      */
      void SetRollingFriction(float friction) override;

      /**
      * @see sulphur::physics::PhysicsCollider::GetRollingFriction
      */
      float GetRollingFriction() const override;

      /**
      * @see sulphur::physics::PhysicsCollider::SetFrictionCombineMode
      */
      void SetFrictionCombineMode(MaterialCombineMode mode) override;

      /**
      * @see sulphur::physics::PhysicsCollider::GetFrictionCombineMode
      */
      MaterialCombineMode GetFrictionCombineMode() const override;

      /**
      * @see sulphur::physics::PhysicsCollider::SetRestitution
      */
      void SetRestitution(float restitution) override;

      /**
      * @see sulphur::physics::PhysicsCollider::GetRestitution
      */
      float GetRestitution() const override;

      /**
      * @see sulphur::physics::PhysicsCollider::SetRestitutionCombineMode
      */
      void SetRestitutionCombineMode(MaterialCombineMode mode) override;

      /**
      * @see sulphur::physics::PhysicsCollider::GetRestitutionCombineMode
      */
      MaterialCombineMode GetRestitutionCombineMode() const override;
    };

  }
}