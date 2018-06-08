#pragma once

#include "physics/iphysics_box_shape.h"

class btBoxShape;

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::BulletBoxShape : public sulphur::physics::IPhysicsBoxShape
    * @brief The specialization for box shapes.
    * @author Daniel Konings, Angelo van der Mark
    */
    class BulletBoxShape : public IPhysicsBoxShape
    {
    public:

      /**
      * @brief Constructs a box shape.
      */
      BulletBoxShape();

      /**
      * @brief Destructor.
      */
      virtual ~BulletBoxShape();

      /**
      * @brief Sets the extents of this box shape.
      * @param[in] extents (const glm::vec3&) The extents to set.
      */
      void SetExtents(const glm::vec3& extents) override;

      /**
      * @brief Returns the extents of this box shape.
      * @return (glm::vec3) The extents of the box shape.
      */
      glm::vec3 GetExtents() const override;

      /**
      * @brief Returns a pointer to the platform-specific shape implementation.
      * @return (void*) Pointer to the shape.
      */
      void* GetInternalShape() const override;

    private:
      btBoxShape* shape_; //!< Implementation-specific shape instance.
    };
  }
}