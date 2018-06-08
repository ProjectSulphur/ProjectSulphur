#pragma once

#include "physics/iphysics_constraint.h"

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::IPhysicsHingeConstraint : public IPhysicsConstraint
    * @brief interface for hinge constraint implementation
    * @author Benjamin Waanders, Angelo van der Mark
    */
    class IPhysicsHingeConstraint : public IPhysicsConstraint
    {
      public:

        /**
        * @see sulphur::physics::IPhysicsConstraint::GetType
        */
        ConstraintTypes GetType() const override { return IPhysicsConstraint::ConstraintTypes::kHinge; }

        /**
        * @brief Gets pivot point for A's local space
        * @return the pivot point for A
        */
        virtual glm::vec3 GetPivotA() const = 0;
        /**
        * @brief Gets pivot point for B's local space
        * @return the pivot point for B
        */
        virtual glm::vec3 GetPivotB() const = 0;
        /**
        * @brief Gets axis for A's local space
        * @return the axis for B
        */
        virtual glm::vec3 GetAxisA() const = 0;
        /**
        * @brief Gets axis for B's local space
        * @return the axis for A
        */
        virtual glm::vec3 GetAxisB() const = 0;

        /**
        * @brief Sets pivot for B's local space
        * @param[in] pivot (const glm::vec3&) the pivot point for frame B
        */
        virtual void SetPivotB(const glm::vec3& pivot) = 0;
        /**
        * @brief Sets pivot for A's local space
        * @param[in] pivot (const glm::vec3&) the pivot point for frame A
        */
        virtual void SetPivotA(const glm::vec3& pivot) = 0;
        /**
        * @brief Sets axis for A's local space
        * @param[in] axis (const glm::vec3&) the pivot point for frame A
        */
        virtual void SetAxisA(const glm::vec3& axis) = 0;
        /**
        * @brief Sets axis for B's local space
        * @param[in] axis (const glm::vec3&) the pivot point for frame B
        */
        virtual void SetAxisB(const glm::vec3& axis) = 0;

        /**
        * @brief Sets the minimum angle for the constraint.
        * @param[in] angle (float) The angle in radians.
        */
        virtual void SetMinimumAngle(float angle) = 0;
        /**
        * @brief Sets the maximum angle for the constraint.
        * @param[in] angle (float) The angle in radians.
        */
        virtual void SetMaximumAngle(float angle) = 0;
        /**
        * @brief Gets the minimum angle of the constraint.
        * @return (float) The angle in radians.
        */
        virtual float GetMinimumAngle() const = 0;
        /**
        * @brief Gets the maximum angle of the constraint.
        * @return (float) The angle in radians.
        */
        virtual float GetMaximumAngle() const = 0;

        /**
        * @brief Gets the current angle of the constraint.
        * @return (float) The angle in radians.
        */
        virtual float GetHingeAngle() const = 0;

        /**
        * @brief Sets the softness of the constraint.
        * @param[in] softness (float) Softness factor.
        */
        virtual void SetSoftness(float softness) = 0;
        
        /**
        * @brief Gets the softness of the constraint.
        * @return (float) Softness factor.
        */
        virtual float GetSoftness() const = 0;

        /**
        * @brief Sets the bias factor of the constraint.
        * @param[in] bias (float) The bias factor.
        */
        virtual void SetBiasFactor(float bias) = 0;

        /**
        * @brief Gets the bias factor of the constraint.
        * @return (float) The bias factor.
        */
        virtual float GetBiasFactor() const = 0;
        
        /**
        * @brief Sets the relaxation factor of the constraint.
        * @param[in] relaxation (float) The relaxation factor.
        */
        virtual void SetRelaxationFactor(float relaxation) = 0;

        /**
        * @brief Gets the relaxation factor of the constraint.
        * @return (float) The relaxation factor.
        */
        virtual float GetRelaxationFactor() const = 0;


    };
  }
} 
