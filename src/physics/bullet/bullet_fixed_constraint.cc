#include "physics/bullet/bullet_fixed_constraint.h"
#include "physics/bullet/bullet_include.h"
#include "physics/bullet/bullet_conversions.h"

#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace physics
  {
    //-------------------------------------------------------------------------
    BulletFixedConstraint::BulletFixedConstraint(
      PhysicsBody* body_a,
      btDynamicsWorld* world) :
      body_a_(body_a),
      body_b_(nullptr),
      world_(world)
    {
      bullet_constraint_ =
        foundation::Memory::Construct<btGeneric6DofSpring2Constraint>(
          *body_a->rigid_body(), btTransform::getIdentity());

      // Make this constraint fixed:
      bullet_constraint_->setAngularLowerLimit(btVector3(0, 0, 0));
      bullet_constraint_->setAngularUpperLimit(btVector3(0, 0, 0));
      bullet_constraint_->setLinearLowerLimit(btVector3(0, 0, 0));
      bullet_constraint_->setLinearUpperLimit(btVector3(0, 0, 0));

      world->addConstraint(bullet_constraint_, true);
    }

    //-------------------------------------------------------------------------
    BulletFixedConstraint::~BulletFixedConstraint()
    {
      world_->removeConstraint(bullet_constraint_);
      foundation::Memory::Destruct(bullet_constraint_);
    }

    //-------------------------------------------------------------------------
    void BulletFixedConstraint::SetTorqueLimit(float limit)
    {
      // Sadly not implemented by bullet
      if (limit == 0.0f)
      {
        limit = FLT_MAX;
      }
      bullet_constraint_->setBreakingImpulseThreshold(limit);
    }

    //-------------------------------------------------------------------------
    void BulletFixedConstraint::SetForceLimit(float limit)
    {
      // So this does the same as a force, or well, an impulse
      if (limit == 0.0f)
      {
        limit = FLT_MAX;
      }
      bullet_constraint_->setBreakingImpulseThreshold(limit);
    }

    //-------------------------------------------------------------------------
    float BulletFixedConstraint::GetForceLimit() const
    {
      return bullet_constraint_->getBreakingImpulseThreshold();
    }

    //-------------------------------------------------------------------------
    float BulletFixedConstraint::GetTorqueLimit() const
    {
      return bullet_constraint_->getBreakingImpulseThreshold();
    }

    //-------------------------------------------------------------------------
    void BulletFixedConstraint::SetEnabled(bool enabled)
    {
      bullet_constraint_->setEnabled(enabled);
    }

    //-------------------------------------------------------------------------
    bool BulletFixedConstraint::IsEnabled() const
    {
      return bullet_constraint_->isEnabled();
    }

    //-------------------------------------------------------------------------
    PhysicsBody* BulletFixedConstraint::GetBodyA()
    {
      return body_a_;
    }

    //-------------------------------------------------------------------------
    PhysicsBody* BulletFixedConstraint::GetBodyB()
    {
      return body_b_;
    }

    //-------------------------------------------------------------------------
    void BulletFixedConstraint::SetBodyB(PhysicsBody* body_b)
    {
      body_b_ = body_b;

      world_->removeConstraint(bullet_constraint_);
      bullet_constraint_->~btGeneric6DofSpring2Constraint();

      if (body_b_ != nullptr)
      {
        // Takes both transforms and sets up the constraint to be in the middle of the two.
        btTransform body_a_trans;
        btTransform body_b_trans;
        body_a_->rigid_body()->getMotionState()->getWorldTransform(body_a_trans);
        body_b_->rigid_body()->getMotionState()->getWorldTransform(body_b_trans);

        btTransform fixedPoint = btTransform::getIdentity();
        fixedPoint.setOrigin(
          (body_a_trans.getOrigin() + body_b_trans.getOrigin()) * 0.5f);

        btTransform frame_a_ = body_a_trans.inverse() * fixedPoint;
        btTransform frame_b_ = body_b_trans.inverse() * fixedPoint;

        new(bullet_constraint_) btGeneric6DofSpring2Constraint(
          *body_a_->rigid_body(), *body_b_->rigid_body(),
          frame_a_, frame_b_);
      }
      else
      {
        // Set constraint on body A, as it gives the most stability.
        new(bullet_constraint_) btGeneric6DofSpring2Constraint(
          *body_a_->rigid_body(), btTransform::getIdentity());
      }

      // Make this constraint fixed.
      bullet_constraint_->setAngularLowerLimit(btVector3(0, 0, 0));
      bullet_constraint_->setAngularUpperLimit(btVector3(0, 0, 0));
      bullet_constraint_->setLinearLowerLimit(btVector3(0, 0, 0));
      bullet_constraint_->setLinearUpperLimit(btVector3(0, 0, 0));

      world_->addConstraint(bullet_constraint_, true);
    }

    //-------------------------------------------------------------------------
    void BulletFixedConstraint::SetFrameA(const glm::mat4x4& frame)
    {
      // If this constraint doesn't have a second body attached, Frame B is used!
      if (body_b_ != nullptr)
      {
        bullet_constraint_->setFrames(BulletConversions::ToBt(frame),
          bullet_constraint_->getFrameOffsetB());
      }
      else
      {
        bullet_constraint_->setFrames(bullet_constraint_->getFrameOffsetA(), 
          BulletConversions::ToBt(frame));
      }
    }

    //-------------------------------------------------------------------------
    void BulletFixedConstraint::SetFrameB(const glm::mat4x4& frame)
    {
      if (body_b_ != nullptr)
      {
        bullet_constraint_->setFrames(bullet_constraint_->getFrameOffsetA(),
          BulletConversions::ToBt(frame));
      }
    }

    //-------------------------------------------------------------------------
    glm::mat4x4 BulletFixedConstraint::GetFrameA() const
    {
      // If this constraint doesn't have a second body attached, Frame B is used!
      if (body_b_ == nullptr)
      {
        return BulletConversions::ToGlm(bullet_constraint_->getFrameOffsetB());
      }
      return BulletConversions::ToGlm(bullet_constraint_->getFrameOffsetA());
    }

    //-------------------------------------------------------------------------
    glm::mat4x4 BulletFixedConstraint::GetFrameB() const
    {
      if (body_b_ != nullptr)
      {
        return BulletConversions::ToGlm(bullet_constraint_->getFrameOffsetB());
      }
      return BulletConversions::ToGlm(bullet_constraint_->getFrameOffsetA());
    }
  }
}
