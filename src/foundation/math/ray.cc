#include "foundation/math/ray.h"

namespace sulphur
{
  namespace foundation
  {
    //-------------------------------------------------------------------------
    Ray::Ray() :
      origin(glm::vec3{ 0.0f, 0.0f, 0.0f }),
      direction(glm::vec3{ 0.0f, 1.0f, 0.0f })
    {

    }

    //-------------------------------------------------------------------------
    Ray::Ray(const glm::vec3& o, const glm::vec3& d) :
      origin(o),
      direction(d)
    {

    }

    //-------------------------------------------------------------------------
    Ray Ray::FromTo(const glm::vec3& from, const glm::vec3& to)
    {
      Ray ray;
      ray.origin = from;
      ray.direction = glm::normalize(to - from);

      return ray;
    }
  }
}