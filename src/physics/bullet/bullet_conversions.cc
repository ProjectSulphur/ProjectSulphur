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
      // Bullet matrix to GLM matrix needs to be transposed, done here by using getColumn
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
    glm::mat4x4 BulletConversions::ToGlm(const btTransform & transform)
    {
      glm::mat4x4 glm_matrix = glm::mat4x4();
      glm::mat3x3 rotation = ToGlm(transform.getBasis());
      glm_matrix[0] = { rotation[0], 0.0f };
      glm_matrix[1] = { rotation[1], 0.0f };
      glm_matrix[2] = { rotation[2], 0.0f };
      glm_matrix[3] = { ToGlm(transform.getOrigin()), 1.0f };

      return glm_matrix;
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
      // GLM matrix to Bullet matrix needs to be transposed, which is done directly here
      return btMatrix3x3(
        mat[0].x, mat[1].x, mat[2].x,
        mat[0].y, mat[1].y, mat[2].y,
        mat[0].z, mat[1].z, mat[2].z);
    }

    //-------------------------------------------------------------------------
    btQuaternion BulletConversions::ToBt(const glm::quat & quat)
    {
      return btQuaternion(quat.x, quat.y, quat.z, quat.w);
    }

    //-------------------------------------------------------------------------
    btTransform BulletConversions::ToBt(const glm::mat4x4 & transform)
    {
      btMatrix3x3 basis = btMatrix3x3();

      // GLM matrix to Bullet matrix needs to be transposed, which is done directly here
      basis[0] = ToBt({ transform[0].x, transform[1].x, transform[2].x });
      basis[1] = ToBt({ transform[0].y, transform[1].y, transform[2].y });
      basis[2] = ToBt({ transform[0].z, transform[1].z, transform[2].z });

      btVector3 origin = ToBt({ transform[3].x, transform[3].y, transform[3].z });

      return btTransform(basis, origin);
    }
  }
}
