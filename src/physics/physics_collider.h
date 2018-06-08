#pragma once

#include <glm/gtc/quaternion.hpp>

namespace sulphur
{
  namespace physics
  {
    class IPhysicsBody;
    class IPhysicsShape;

    /**
    * @class sulphur::physics::PhysicsCollider
    * @brief An instance of a collision shape that is attached to a sulphur::physics::IPhysicsBody. 
    * This class also contains functionality for physics materials and collider offsets.
    * @author Angelo van der Mark
    */
    class PhysicsCollider
    {
    public:

      /**
      * @brief Describes the method used to combine the friction/restitution of colliding objects.
      * @note If combine modes don't match, the mode is prioritized as follows: Average < Min < Multiply < Max.
      */
      enum MaterialCombineMode
      {
        kAverage, //!< Use the average the two variables.
        kMin, //!< Use the smallest of the two variables.
        kMultiply, //!< Multiply the two variables.
        kMax //!< Use the largest of the two variables.
      };


      /**
      * @brief Constructor, for use by sulphur::physics::IPhysicsBody only.
      * @param[in] owner (sulphur::physics::IPhysicsBody*) The physics body this collider is attached to.
      * @param[in] shape (sulphur::physics::IPhysicsShape*) The shape of this collider.
      */
      PhysicsCollider(IPhysicsBody* owner, IPhysicsShape* shape);

      /**
      * @brief Destructor.
      */
      virtual ~PhysicsCollider();

      /**
      * @brief Sets the collider's translation.
      * @param[in] translation (const glm:::vec3&) The local translation for this collider.
      */
      void SetTranslation(const glm::vec3& translation);

      /**
      * @brief Returns the collider's local translation.
      * @return (const glm::vec3&) The local translation of this collider.
      */
      const glm::vec3& translation() const;

      /**
      * @brief Sets the collider's rotation.
      * @param[in] rotation (const glm:::quat&) The local rotation for this collider.
      */
      void SetRotation(const glm::quat& rotation);

      /**
      * @brief Returns the collider's local rotation.
      * @return (const glm::quat&) The local rotation of this collider.
      */
      const glm::quat& rotation() const;

      /**
      * @brief Returns the physics shape.
      * @return (sulphur::physics::IPhysicsShape*) The physics shape.
      */
      IPhysicsShape* shape() const;

      /**
      * @brief Returns the PhysicsBody this collider is attached to.
      * @return (sulphur::physics::IPhysicsBody*) This collider's owner.
      */
      IPhysicsBody* owner() const;

      /**
      * @brief Function intended to update the collider and/or PhysicsBody if required by the implementation.
      */
      virtual void OnShapeUpdate() = 0;

      /**
      * @brief Sets the static friction coefficient of this shape's material.
      * @param[in] friction (float) The static friction coefficient.
      */
      virtual void SetStaticFriction(float friction) = 0;

      /**
      * @brief Returns the static friction coefficient of this shape's material.
      * @returns (float) The static friction coefficient.
      */
      virtual float GetStaticFriction() const = 0;

      /**
      * @brief Sets the dynamic friction coefficient of this shape's material.
      * @param[in] friction (float) The dynamic friction coefficient.
      */
      virtual void SetDynamicFriction(float friction) = 0;

      /**
      * @brief Returns the dynamic friction coefficient of this shape's material.
      * @returns (float) The dynamic friction coefficient.
      */
      virtual float GetDynamicFriction() const = 0;

      /**
      * @brief Sets the rolling friction coefficient of this shape's material.
      * @param[in] friction (float) The rolling friction coefficient.
      */
      virtual void SetRollingFriction(float friction) = 0;

      /**
      * @brief Returns the rolling friction coefficient of this shape's material.
      * @returns (float) The rolling friction coefficient.
      */
      virtual float GetRollingFriction() const = 0;

      /**
      * @brief Sets the friction combine mode to use.
      * @param[in] mode (sulphur::physics::PhysicsCollider::MaterialCombineMode) The mode to use.
      */
      virtual void SetFrictionCombineMode(MaterialCombineMode mode) = 0;

      /**
      * @brief Gets the friction combine mode to be used.
      * @returns (sulphur::physics::PhysicsCollider::MaterialCombineMode) The mode to be used.
      */
      virtual MaterialCombineMode GetFrictionCombineMode() const = 0;

      /**
      * @brief Sets the bounciness of this shape's material.
      * @note A combined value of 0 causes no bounce. A combined value of 1 will bounce without energy loss.
      * @param[in] restitution (float) The restitution.
      */
      virtual void SetRestitution(float restitution) = 0;

      /**
      * @brief Gets the bounciness of this shape's material.
      * @note A combined value of 0 causes no bounce. A combined value of 1 will bounce without energy loss.
      * @returns (float) The restitution.
      */
      virtual float GetRestitution() const = 0;

      /**
      * @brief Sets the restitution combine mode to use.
      * @param[in] mode (sulphur::physics::PhysicsCollider::MaterialCombineMode) The mode to use.
      */
      virtual void SetRestitutionCombineMode(MaterialCombineMode mode) = 0;

      /**
      * @brief Gets the restitution combine mode to be used.
      * @returns (sulphur::physics::PhysicsCollider::MaterialCombineMode) The mode to be used.
      */
      virtual MaterialCombineMode GetRestitutionCombineMode() const = 0;

      static const float kStaticFrictionThreshold; //!< Threshold that determines whether static or dynamic friction should be used.

    protected:
      IPhysicsBody* owner_; //!< PhysicsBody this collider is attached to.
      IPhysicsShape* shape_; //!< The shape this collider is using.
      glm::vec3 translation_; //!< The collider's local translation.
      glm::quat rotation_; //!< The collider's local rotation.
    };

  }
}