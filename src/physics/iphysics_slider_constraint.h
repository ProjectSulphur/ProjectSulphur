#pragma once

#include "physics/iphysics_constraint.h"

#include <glm/vec3.hpp>

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::IPysicsSliderConstraint : sulphur::physics::IPhysicsConstraint
    * @brief interface for ballsocket implementation
    * @author Benjamin Waanders
    */
    class IPysicsSliderConstraint : IPhysicsConstraint
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
        * @brief Sets the minimum angle
        * @param[in] min (float)
        */
        virtual void SetMinAngle(float min) = 0;
        /**
        * @brief Sets the maximum angle
        * @param[in] max (float)
        */
        virtual void SetMaxAngle(float max) = 0;
        /**
        * @brief Gets the minimum angle
        */
        virtual float GetMinAngle() = 0;
        /**
        * @brief Gets the maximum angle
        */
        virtual float GetMaxAngle() = 0;

        /**
        * @brief Sets the minimum Linear reach
        * @param[in] min (float)
        */
        virtual void SetMinLinear(float min) = 0;
        /**
        * @brief Sets the maximum Linear reach
        * @param[in] max (float)
        */
        virtual void SetMaxLinear(float max) = 0;
        /**
        * @brief Gets the minimum Linear reach
        */
        virtual float GetMinLinear() = 0;
        /**
        * @brief Gets the maximum Linear reach
        */
        virtual float GetMaxLinear() = 0;
    };
  }
}
