#pragma once

#include <glm/glm.hpp>

namespace sulphur 
{
  namespace foundation 
  {
    /**
    * @struct sulphur::foundation::Ray
    * @brief Used for raycasting/raytracing, casts a ray from an origin with a specific direction
    * @author Daniel Konings
    */
    struct Ray
    {
      /**
      * @brief Default constructor
      */
      Ray();

      /**
      * @brief Construct with an origin and direction
      * @param[in] origin (const glm::vec3&) The origin of the ray
      * @param[in] direction (const glm::vec3&) The direction of the ray
      */
      Ray(const glm::vec3& origin, const glm::vec3& direction);

      /**
      * @brief Creates a ray that originates at "from" and has a direction towards "to"
      * @param[in] from (const glm::vec3&) The origin position
      * @param[in] to (const glm::vec3&) The position to look towards,
      * determining the direction of the ray
      * @return (sulphur::foundation::Ray) The created ray
      */
      static Ray FromTo(const glm::vec3& from, const glm::vec3& to);

      glm::vec3 origin; //!< The origin of the ray
      glm::vec3 direction; //!< The direction of the ray
    };
  }
}