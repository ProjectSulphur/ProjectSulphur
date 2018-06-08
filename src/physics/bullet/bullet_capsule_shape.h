#pragma once

#include "physics/iphysics_capsule_shape.h"

class btCapsuleShape;

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletCapsuleShape : public sulphur::physics::IPhysicsCapsuleShape
    * @brief The specialization for capsule shapes.
    * @author Angelo van der Mark
    */
    class BulletCapsuleShape : public IPhysicsCapsuleShape
    {
    public:

      /**
      * @brief Constructs a capsule shape.
      */
      BulletCapsuleShape();

      /**
      * @brief Destructor.
      */
      virtual ~BulletCapsuleShape();

      /**
      * @brief Sets the radius of this capsule shape.
      * @param[in] radius (float) The radius to set.
      */
      void SetRadius(float radius) override;

      /**
      * @brief Returns the radius of this capsule shape.
      * @return (float) The radius of the capsule shape.
      */
      float GetRadius() const override;

      /**
      * @brief Sets the height of the capsule shape.
      * @note This takes the radius into account and should not be set lower than the radius.
      * @param[in] height (float) The total height of the capsule.
      */
      void SetHeight(float height) override;

      /**
      * @brief Returns the total height of this capsule shape.
      * @return (float) The height of the capsule shape.
      */
      float GetHeight() const override;

      /**
      * @brief Returns a pointer to the platform-specific shape implementation.
      * @return (void*) Pointer to the shape.
      */
      void* GetInternalShape() const override;

    private:
      btCapsuleShape* shape_; //!< Implementation-specific shape instance.
    };
  }
}