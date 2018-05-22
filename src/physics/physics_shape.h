#pragma once

#include "physics/platform_physics_body.h"

namespace sulphur
{
  namespace physics
  {
    /**
    * @class sulphur::physics::PhysicsShape
    * @brief The platform-independent base class for every shape to use
    * @author Daniel Konings, Angelo van der Mark
    */
    class PhysicsShape
    {

    public:

      /**
      * @brief The different shape types that exist
      */
      enum struct ShapeTypes
      {
        kBox, //!< A box shape with extents from the origin
        kSphere, //!< A spherical shape with a radius
        kCapsule, //!< A capsule shape, basically a cylinder with spherically capped ends
        kMesh //!< A mesh shape, consisting of multiple triangles
      };

      /**
      * @brief Default constructor.
      */
      PhysicsShape();

      /**
      * @brief Virtual destructor.
      */
      virtual ~PhysicsShape();

      /**
      * @brief Returns the type of this shape.
      * @return (sulphur::physics::ShapeTypes) The type of this shape.
      */
      virtual ShapeTypes GetType() const = 0;

      /**
      * @brief Attaches this shape to the provided body.
      * @note If already attached to a body, it will deattach it from the old one.
      * @param[in] body (sulphur::physics::PhysicsBody*) The body to attach it to, or none if nullptr.
      */
      void AttachToBody(PhysicsBody* body);

      /**
      * @brief Returns a pointer to the platform-specific shape implementation.
      * @return (void*) Pointer to the shape.
      */
      virtual void* GetInternalShape() const = 0;

      static const float kDefaultExtents; //!< The default extents for various shapes

    protected:
      PhysicsBody* attached_body_; //!< PhysicsBody this shape is attached to.
    };
  }
}