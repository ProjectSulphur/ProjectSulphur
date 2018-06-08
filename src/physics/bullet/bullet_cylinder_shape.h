#pragma once

#include "physics/iphysics_cylinder_shape.h"

class btCylinderShape;

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletCylinderShape : public sulphur::physics::IPhysicsCylinderShape
    * @brief The specialization for cylinder shapes.
    * @author Angelo van der Mark
    */
    class BulletCylinderShape : public IPhysicsCylinderShape
    {
    public:

      /**
      * @brief Constructs a cylinder shape
      */
      BulletCylinderShape();

      /**
      * @brief Destructor.
      */
      virtual ~BulletCylinderShape();

      /**
      * @brief Sets the radius of this cylinder shape.
      * @param[in] radius (float) The radius to set.
      */
      void SetRadius(float radius) override;

      /**
      * @brief Returns the radius of this cylinder shape.
      * @return (float) The radius of the cylinder shape.
      */
      float GetRadius() const override;

      /**
      * @brief Sets the height of the cylinder shape.
      * @param[in] height (float) The total height of the cylinder.
      */
      void SetHeight(float height) override;

      /**
      * @brief Returns the total height of this cylinder shape.
      * @return (float) The height of the cylinder shape.
      */
      float GetHeight() const override;

      /**
      * @brief Returns a pointer to the platform-specific shape implementation.
      * @return (void*) Pointer to the shape.
      */
      void* GetInternalShape() const override;

    private:
      btCylinderShape* shape_; //!< Implementation-specific shape instance.
    };
  }
}