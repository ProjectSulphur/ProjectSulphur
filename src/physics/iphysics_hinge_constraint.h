#pragma once

#include "physics/iphysics_constraint.h"

#include <glm/vec3.hpp>

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::IPhysicsHingeConstraint : public IPhysicsConstraint
    * @brief interface for hinge constraint implementation
    * @author Benjamin Waanders
    */
    class IPhysicsHingeConstraint : public IPhysicsConstraint
    {
      public:
        /**
        * @brief Sets translation point for A's local space
        * @param[in] pivot (glm::vec3&) The pivot point
        * @param[in] axis (glm::vec3&) The axis to turn around
        * @param[in] calc_b (bool) Weather or not to calculate B frame based upon A's
        */
        using IPhysicsConstraint::SetFrameA;
        virtual void SetFrameA(const glm::vec3& pivot, const glm::vec3& axis, bool calc_b) = 0;
        /**
        * @brief Sets translation point for B's local space
        * @param[in] pivot (glm::vec3&) The pivot point
        * @param[in] axis (glm::vec3&) The axis to turn around
        */
        using IPhysicsConstraint::SetFrameB;
        virtual void SetFrameB(const glm::vec3& pivot, const glm::vec3& axis) = 0;

        /**
        * @brief Gets pivot point for A's local space
        * @return the pivot point for A
        */
        virtual const glm::vec3  GetPivotA() const = 0;
        /**
        * @brief Gets pivot point for B's local space
        * @return the pivot point for B
        */
        virtual const glm::vec3  GetPivotB() const = 0;
        /**
        * @brief Gets axis for A's local space
        * @return the axis for B
        */
        virtual const glm::vec3  GetAxisA() const = 0;
        /**
        * @brief Gets axis for B's local space
        * @return the axis for A
        */
        virtual const glm::vec3  GetAxisB() const = 0;

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
        * @brief Sets the minimum angle
        * @param[in] min (float)
        */
        virtual void SetMin(float min) = 0;
        /**
        * @brief Sets the maximum angle
        * @param[in] max (float)
        */
        virtual void SetMax(float max) = 0;
        /**
        * @brief Gets the minimum angle
        * @return (float) the max angle
        */
        virtual float GetMin() const = 0;
        /**
        * @brief Gets the maximum angle
        * @return (float) the min angle
        */
        virtual float GetMax() const = 0;
    };
  }
} 
