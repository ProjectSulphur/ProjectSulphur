#include "physics/bullet/bullet_hinge_constraint.h"
#include "physics/bullet/bullet_include.h"
#include "physics/bullet/bullet_conversions.h"

#include <foundation/memory/memory.h>

namespace sulphur
{
  namespace physics
  {
    //-------------------------------------------------------------------------
    BulletHingeConstraint::BulletHingeConstraint(
      PhysicsBody* body_a,
      btDynamicsWorld* world) :
      world_(world),
      body_a_(body_a),
      body_b_(nullptr)
    {
      bullet_constraint_ = 
        foundation::Memory::Construct<btHingeConstraint>(
          *body_a->rigid_body(), btVector3(0,0,0), btVector3(0,0,1));

      world->addConstraint(bullet_constraint_);
    }

    //-------------------------------------------------------------------------
    BulletHingeConstraint::~BulletHingeConstraint()
    {
      world_->removeConstraint(bullet_constraint_);
      foundation::Memory::Destruct(bullet_constraint_);
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetTorqueLimit(float limit)
    {
      // Sadly not implemented by bullet
      if (limit == 0.0f)
      {
        limit = FLT_MAX;
      }
      bullet_constraint_->setBreakingImpulseThreshold(limit);
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetForceLimit(float limit)
    {
      // So this does the same as a force, or well, an impulse
      if (limit == 0.0f)
      {
        limit = FLT_MAX;
      }
      bullet_constraint_->setBreakingImpulseThreshold(limit);
    }

    //-------------------------------------------------------------------------
    float BulletHingeConstraint::GetForceLimit() const
    {
      return bullet_constraint_->getBreakingImpulseThreshold();
    }

    //-------------------------------------------------------------------------
    float BulletHingeConstraint::GetTorqueLimit() const
    {
      return bullet_constraint_->getBreakingImpulseThreshold();
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetEnabled(bool enabled)
    {
      bullet_constraint_->setEnabled(enabled);
    }

    //-------------------------------------------------------------------------
    bool BulletHingeConstraint::IsEnabled() const
    {
      return bullet_constraint_->isEnabled();
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetFrameA(const glm::mat4x4& transform)
    {
      bullet_constraint_->setFrames(
        BulletConversions::ToBt(transform), bullet_constraint_->getFrameOffsetB());
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetFrameB(const glm::mat4x4& transform)
    {
      bullet_constraint_->setFrames(
        bullet_constraint_->getFrameOffsetA(), BulletConversions::ToBt(transform));
    }

    //-------------------------------------------------------------------------
    glm::mat4x4 BulletHingeConstraint::GetFrameA() const
    {
      return BulletConversions::ToGlm(bullet_constraint_->getAFrame());
    }

    //-------------------------------------------------------------------------
    glm::mat4x4 BulletHingeConstraint::GetFrameB() const
    {
      return BulletConversions::ToGlm(bullet_constraint_->getBFrame());
    }

    //-------------------------------------------------------------------------
    glm::vec3 BulletHingeConstraint::GetPivotA() const
    {
      return BulletConversions::ToGlm(bullet_constraint_->getAFrame().getOrigin());
    }

    //-------------------------------------------------------------------------
    glm::vec3 BulletHingeConstraint::GetPivotB() const
    {
      return BulletConversions::ToGlm(bullet_constraint_->getBFrame().getOrigin());
    }

    //-------------------------------------------------------------------------
    glm::vec3 BulletHingeConstraint::GetAxisA() const
    {
      return BulletConversions::ToGlm(
        bullet_constraint_->getAFrame().getBasis().getColumn(2));
    }

    //-------------------------------------------------------------------------
    glm::vec3 BulletHingeConstraint::GetAxisB() const
    {
      return BulletConversions::ToGlm(
        bullet_constraint_->getBFrame().getBasis().getColumn(2));
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetPivotA(const glm::vec3& pivot)
    {
      btTransform frame_a = bullet_constraint_->getAFrame();
      frame_a.setOrigin(BulletConversions::ToBt(pivot));
      bullet_constraint_->setFrames(frame_a, bullet_constraint_->getBFrame());
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetPivotB(const glm::vec3& pivot)
    {
      btTransform frame_b = bullet_constraint_->getBFrame();
      frame_b.setOrigin(BulletConversions::ToBt(pivot));
      bullet_constraint_->setFrames(bullet_constraint_->getAFrame(), frame_b);
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetAxisA(const glm::vec3& axis)
    {
      // Warning fix because setAxis wants a non-const reference for no reason.
      btVector3 axis_in_a = BulletConversions::ToBt(axis); 
      bullet_constraint_->setAxis(axis_in_a);
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetAxisB(const glm::vec3& axis)
    {
      // Code from btHingeConstraint::setAxis
      btTransform frame_a = bullet_constraint_->getAFrame();
      btTransform frame_b = bullet_constraint_->getBFrame();

      btVector3 rb_axis_b1, rb_axis_b2;
      btPlaneSpace1(BulletConversions::ToBt(axis), rb_axis_b1, rb_axis_b2);
      btVector3 pivot_in_b = frame_b.getOrigin();
      
      frame_b.getBasis().setValue(rb_axis_b1.getX(), rb_axis_b2.getX(), axis.x,
        rb_axis_b1.getY(), rb_axis_b2.getY(), axis.y,
        rb_axis_b1.getZ(), rb_axis_b2.getZ(), axis.z);

      btVector3 axis_in_a = bullet_constraint_->getRigidBodyB().getCenterOfMassTransform().
        getBasis() * BulletConversions::ToBt(axis);

      btQuaternion rotation_arc = shortestArcQuat(BulletConversions::ToBt(axis), axis_in_a);
      btVector3 rb_axis_a1 = quatRotate(rotation_arc, rb_axis_b1);
      btVector3 rb_axis_a2 = axis_in_a.cross(rb_axis_a1);

      frame_a.getOrigin() = bullet_constraint_->getRigidBodyA().getCenterOfMassTransform().
        inverse()(bullet_constraint_->getRigidBodyB().getCenterOfMassTransform()(pivot_in_b));

      frame_a.getBasis().setValue(rb_axis_a1.getX(), rb_axis_a2.getX(), axis_in_a.getX(),
        rb_axis_a1.getY(), rb_axis_a2.getY(), axis_in_a.getY(),
        rb_axis_a1.getZ(), rb_axis_a2.getZ(), axis_in_a.getZ());
      frame_a.getBasis() = bullet_constraint_->getRigidBodyA().getCenterOfMassTransform()
        .getBasis().inverse() * frame_a.getBasis();

      bullet_constraint_->setFrames(frame_a, frame_b);
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetMinimumAngle(float angle)
    {
      bullet_constraint_->setLimit(angle, GetMaximumAngle(),
                                   GetSoftness(), GetBiasFactor(), GetRelaxationFactor());
    }
    
    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetMaximumAngle(float angle)
    {
      bullet_constraint_->setLimit(GetMinimumAngle(), angle,
                                   GetSoftness(), GetBiasFactor(), GetRelaxationFactor());
    }

    //-------------------------------------------------------------------------
    float BulletHingeConstraint::GetMinimumAngle() const
    {
      return bullet_constraint_->getLowerLimit();
    }

    //-------------------------------------------------------------------------
    float BulletHingeConstraint::GetMaximumAngle() const
    {
      return bullet_constraint_->getUpperLimit();
    }

    //-------------------------------------------------------------------------
    float BulletHingeConstraint::GetHingeAngle() const
    {
      return bullet_constraint_->getHingeAngle();
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetSoftness(float softness)
    {
      bullet_constraint_->setLimit(GetMaximumAngle(), GetMaximumAngle(),
                                   softness, GetBiasFactor(), GetRelaxationFactor());
    }

    //-------------------------------------------------------------------------
    float BulletHingeConstraint::GetSoftness() const
    {
      return bullet_constraint_->getLimitSoftness();
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetBiasFactor(float bias)
    {
      bullet_constraint_->setLimit(GetMaximumAngle(), GetMaximumAngle(),
                                   GetSoftness(), bias, GetRelaxationFactor());
    }

    //-------------------------------------------------------------------------
    float BulletHingeConstraint::GetBiasFactor() const
    {
      return bullet_constraint_->getLimitBiasFactor();
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetRelaxationFactor(float relaxation)
    {
      bullet_constraint_->setLimit(GetMaximumAngle(), GetMaximumAngle(),
                                   GetSoftness(), GetBiasFactor(), relaxation);
    }

    //-------------------------------------------------------------------------
    float BulletHingeConstraint::GetRelaxationFactor() const
    {
      return bullet_constraint_->getLimitRelaxationFactor();
    }

    //-------------------------------------------------------------------------
    PhysicsBody* BulletHingeConstraint::GetBodyA()
    {
      return body_a_;
    }

    //-------------------------------------------------------------------------
    PhysicsBody* BulletHingeConstraint::GetBodyB()
    {
      return body_b_;
    }

    //-------------------------------------------------------------------------
    void BulletHingeConstraint::SetBodyB(PhysicsBody* body_b)
    {
      body_b_ = body_b;

      glm::vec3 pivot_a = GetPivotA();
      glm::vec3 axis_a = GetAxisA();

      world_->removeConstraint(bullet_constraint_);
      bullet_constraint_->~btHingeConstraint();

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

        new(bullet_constraint_) btHingeConstraint(
          *body_a_->rigid_body(), *body_b_->rigid_body(),
          frame_a_, frame_b_);

        SetAxisA(axis_a); // Override default axis for the previous one.
      }
      else
      {
        new(bullet_constraint_) btHingeConstraint(
          *body_a_->rigid_body(), BulletConversions::ToBt(pivot_a),
          BulletConversions::ToBt(axis_a));
      }

      world_->addConstraint(bullet_constraint_);
    }
  }
}
