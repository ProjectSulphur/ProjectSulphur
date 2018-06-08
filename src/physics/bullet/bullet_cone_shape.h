#pragma once

#include "physics/iphysics_cone_shape.h"

class btConeShape;

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletConeShape : public sulphur::physics::IPhysicsConeShape
    * @brief The specialization for cone shapes.
    * @author Angelo van der Mark
    */
    class BulletConeShape : public IPhysicsConeShape
    {
    public:

      /**
      * @brief Constructs a cone shape.
      */
      BulletConeShape();

      /**
      * @brief Destructor.
      */
      virtual ~BulletConeShape();

      /**
      * @brief Sets the radius of this cone shape.
      * @param[in] radius (float) The radius to set.
      */
      void SetRadius(float radius) override;

      /**
      * @brief Returns the radius of this cone shape.
      * @return (float) The radius of the cone shape.
      */
      float GetRadius() const override;

      /**
      * @brief Sets the height of the cone shape.
      * @param[in] height (float) The total height of the cone.
      */
      void SetHeight(float height) override;

      /**
      * @brief Returns the total height of this cone shape.
      * @return (float) The height of the cone shape.
      */
      float GetHeight() const override;

      /**
      * @brief Returns a pointer to the platform-specific shape implementation.
      * @return (void*) Pointer to the shape.
      */
      void* GetInternalShape() const override;

    private:
      btConeShape* shape_; //!< Implementation-specific shape instance.
    };
  }
}