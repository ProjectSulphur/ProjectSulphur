#include "physics/bullet/bullet_conversions.h"

namespace sulphur
{
  namespace physics
  {
    //-------------------------------------------------------------------------
    glm::vec3 BulletConversions::ToGlm(const btVector3 & v)
    {
      return glm::vec3(v.x(), v.y(), v.z());
    }

    //-------------------------------------------------------------------------
    glm::vec4 BulletConversions::ToGlm(const btVector4 & v)
    {
      return glm::vec4(v.x(), v.y(), v.z(), v.w());
    }

    //-------------------------------------------------------------------------
    glm::mat3x3 BulletConversions::ToGlm(const btMatrix3x3 & mat)
    {
      return glm::mat3x3(
        ToGlm(mat.getColumn(0)),
        ToGlm(mat.getColumn(1)),
        ToGlm(mat.getColumn(2)));
    }

    //-------------------------------------------------------------------------
    glm::quat BulletConversions::ToGlm(const btQuaternion & quat)
    {
      return glm::quat(quat.w(), quat.x(), quat.y(), quat.z());
    }
    //-------------------------------------------------------------------------
    btVector3 BulletConversions::ToBt(const glm::vec3 & v)
    {
      return btVector3(v.x, v.y, v.z);
    }

    //-------------------------------------------------------------------------
    btVector4 BulletConversions::ToBt(const glm::vec4 & v)
    {
      return btVector4(v.x, v.y, v.z, v.w);
    }

    //-------------------------------------------------------------------------
    btMatrix3x3 BulletConversions::ToBt(const glm::mat3x3 & mat)
    {
      return btMatrix3x3(
        mat[0][1], mat[0][2], mat[0][3],
        mat[1][1], mat[1][2], mat[1][3],
        mat[2][1], mat[2][2], mat[2][3]);
    }

    //-------------------------------------------------------------------------
    btQuaternion BulletConversions::ToBt(const glm::quat & quat)
    {
      return btQuaternion(quat.x, quat.y, quat.z, quat.w);
    }
  }
}
