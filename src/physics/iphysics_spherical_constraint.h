#pragma once

#include "physics/iphysics_constraint.h"

#include <glm/vec3.hpp>

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::IPysicsSphericalConstraint : sulphur::physics::IPhysicsConstraint
    * @brief interface for spherical constraint implementation
    * @author Benjamin Waanders
    */
    class IPysicsSphericalConstraint : IPhysicsConstraint
    {
    public:
      /**
      * @brief Sets pivot point for A's local space
      * @param[in] pivot (glm::vec3&)
      */
      virtual void SetPivotA(glm::vec3& pivot) = 0;
      /**
      * @brief Sets pivot point for B's local space
      * @param[in] pivot (glm::vec3&)
      */
      virtual void SetPivotB(glm::vec3& pivot) = 0;
      /**
      * @brief Sets axis for A's local space
      * @param[in] axis (glm::vec3&)
      */
      virtual void SetAxisA(glm::vec3& axis) = 0;
      /**
      * @brief Sets axis for B's local space
      * @param[in] axis (glm::vec3&)
      */
      virtual void SetAxisB(glm::vec3& axis) = 0;

      /**
      * @brief Gets pivot point for A's local space
      * @return the pivot
      */
      virtual glm::vec3&  GetPivotA() = 0;
      /**
      * @brief Gets pivot point for B's local space
      * @return the pivot
      */
      virtual glm::vec3&  GetPivotB() = 0;
      /**
      * @brief Gets axis for A's local space
      * @return the axis
      */
      virtual glm::vec3&  GetAxisA() = 0;
      /**
      * @brief Gets axis for B's local space
      * @return the axis
      */
      virtual glm::vec3&  GetAxisB() = 0;

      /**
      * @brief Sets the limits of the spherical joint
      * @param[in] limit (glm::vec3&) Where x and y are the ranges in rad and z is the rotation 
      */
      virtual void SetLimit(glm::vec3& limit) = 0;

      /**
      * @brief Gets the limits of the spherical joint
      * @return (glm::vec3&) Where x and y are the ranges in rad and z is the rotation
      */
      virtual glm::vec3& GetLimit() = 0;
    };
  }
}
