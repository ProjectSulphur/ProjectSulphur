#pragma once

#include <foundation/math/ray.h>
#include <foundation/containers/vector.h>

namespace sulphur
{
  namespace physics
  {
    /**
    * @struct sulphur::physics::RaycastHitInfo
    * @brief Used to store Raycasting hit information and expose it to the engine
    * @author Daniel Konings
    */
    struct RaycastHitInfo
    {
      foundation::Ray ray; //!< The ray that was cast
      glm::vec3 point; //!< The point where the ray intersected a collider
      glm::vec3 normal; //!< The normal of the intersection
      float distance; //!< The distance scalar from the origin of the ray
    };

    using RaycastHits = foundation::Vector<RaycastHitInfo>; //!< typedef for hitinfo collection
  }
}