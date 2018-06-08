#pragma once

#include "physics/iphysics_sphere_shape.h"

class btSphereShape;

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletSphereShape : public sulphur::physics::IPhysicsSphereShape
    * @brief The specialization for sphere shapes.
    * @author Daniel Konings, Angelo van der Mark
    */
    class BulletSphereShape : public IPhysicsSphereShape
    {
    public:

      /**
      * @brief Constructs a sphere shape
      */
      BulletSphereShape();

      /**
      * @brief Destructor.
      */
      virtual ~BulletSphereShape();

      /**
      * @brief Sets the radius of this sphere shape.
      * @param[in] radius (float) The radius to set.
      */
      void SetRadius(float radius) override;

      /**
      * @brief Returns the radius of this sphere shape.
      * @return (float) The radius of the sphere shape.
      */
      float GetRadius() const override;

      /**
      * @brief Returns a pointer to the platform-specific shape implementation.
      * @return (void*) Pointer to the shape.
      */
      void* GetInternalShape() const override;

    private:
      btSphereShape* shape_; //!< Implementation-specific shape instance.
    };
  }
}