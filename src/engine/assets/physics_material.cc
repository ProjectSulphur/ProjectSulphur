#include "engine/assets/physics_material.h"

namespace sulphur
{
  namespace engine
  {

    //-------------------------------------------------------------------------
    PhysicsMaterial::PhysicsMaterial() :
      static_friction_(0.6f),
      dynamic_friction_(0.6f),
      rolling_friction_(0.0f),
      restitution_(0.0f),
      friction_combine_mode_(physics::PhysicsCollider::kAverage),
      restitution_combine_mode_(physics::PhysicsCollider::kAverage)
    {
    }

    //-------------------------------------------------------------------------
    void PhysicsMaterial::set_static_friction(float friction)
    {
      static_friction_ = friction;
    }

    //-------------------------------------------------------------------------
    float PhysicsMaterial::static_friction() const
    {
      return static_friction_;
    }

    //-------------------------------------------------------------------------
    void PhysicsMaterial::set_dynamic_friction(float friction)
    {
      dynamic_friction_ = friction;
    }

    //-------------------------------------------------------------------------
    float PhysicsMaterial::dynamic_friction() const
    {
      return dynamic_friction_;
    }

    //-------------------------------------------------------------------------
    void PhysicsMaterial::set_rolling_friction(float friction)
    {
      rolling_friction_ = friction;
    }

    //-------------------------------------------------------------------------
    float PhysicsMaterial::rolling_friction() const
    {
      return rolling_friction_;
    }

    //-------------------------------------------------------------------------
    void PhysicsMaterial::set_friction_combine_mode
      (physics::PhysicsCollider::MaterialCombineMode mode)
    {
      friction_combine_mode_ = mode;
    }

    //-------------------------------------------------------------------------
    physics::PhysicsCollider::MaterialCombineMode PhysicsMaterial::friction_combine_mode() const
    {
      return friction_combine_mode_;
    }

    //-------------------------------------------------------------------------
    void PhysicsMaterial::set_restitution(float restitution)
    {
      restitution_ = restitution;
    }

    //-------------------------------------------------------------------------
    float PhysicsMaterial::restitution() const
    {
      return restitution_;
    }

    //-------------------------------------------------------------------------
    void PhysicsMaterial::set_restitution_combine_mode
      (physics::PhysicsCollider::MaterialCombineMode mode)
    {
      restitution_combine_mode_ = mode;
    }

    //-------------------------------------------------------------------------
    physics::PhysicsCollider::MaterialCombineMode PhysicsMaterial::restitution_combine_mode() const
    {
      return restitution_combine_mode_;
    }
  }
}