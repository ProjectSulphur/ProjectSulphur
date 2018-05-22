#pragma once
#include "physics/physics_shape.h"

class btCollisionShape;

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletShape : public sulphur::physics::PhysicsShape
    * @brief Used to wrap the multiple internal shapes provided by Bullet
    * @author Daniel Konings, Angelo van der Mark
    */
    class BulletShape : public PhysicsShape
    {
    public:
      /**
      * @see sulphur::physics::PhysicsShape::GetInternalShape
      */
      void* GetInternalShape() const override;

    protected:
      /**
      * @brief Default constructor.
      */
      BulletShape();

      /**
      * @brief Destructs the Bullet shape.
      */
      virtual ~BulletShape();

    protected:
      btCollisionShape* shape_; //!< The internal base Bullet shape.
    };
  }
}
