#include "ui_elements.h"

namespace sulphur
{
  namespace engine
  {
    //-------------------------------------------------------------------------
    Rect ImageUIElementData::GetGlobalBounds(const bool & clipped)
    {
      return BaseUIElementData::GetGlobalBounds(clipped);
    }

    glm::vec3 RotateVector(glm::vec3 v, float angle)
    {
      // use local variables to find transformed components
      float x = cosf(angle) * v.x - sinf(angle) * v.y;
      float y = sinf(angle) * v.x + cosf(angle) * v.y;
      // store results thru the pointer
      return glm::vec3(x, y, v.z);
    }

    //-------------------------------------------------------------------------
    Mesh ImageUIElementData::GetMesh(const Rect & clipped_bounds)
    {
      // TODO (Hilze): Do something with the clipped bounds.

      glm::vec2 min = clipped_bounds.Min();
      glm::vec2 max = clipped_bounds.Max();
      //glm::vec2 center = (min + max) * 0.5f;

      glm::vec2 tex_min = texture_rect.Min();
      glm::vec2 tex_max = texture_rect.Max();

      Mesh mesh;

      foundation::Vector<glm::vec3> vertices = {
        glm::vec3(min.x, max.y, 0.5f),
          glm::vec3(min.x, min.y, 0.5f),
          glm::vec3(max.x, min.y, 0.5f),
          glm::vec3(max.x, max.y, 0.5f)
      };

      /*for (glm::vec3& v : vertices)
      {
        v = RotateVector(v - glm::vec3(center.x, center.y, 0.0f), rotation) + glm::vec3(center.x, center.y, 0.0f);
      }*/

      mesh.SetVertices(eastl::move(vertices));
      mesh.SetNormals({
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)
      });
      foundation::Vector<glm::vec2> uvs = {
        glm::vec2(tex_min.x, tex_max.y),
        glm::vec2(tex_min.x, tex_min.y),
        glm::vec2(tex_max.x, tex_min.y),
        glm::vec2(tex_max.x, tex_max.y)
      };

      for (glm::vec2& v : uvs)
      {
        glm::vec3 t = (RotateVector(glm::vec3(v.x, v.y, 0.0f) * 2.0f - 1.0f, rotation) + 1.0f) * 0.5f;
        v = glm::vec2(t.x, t.y);
      }

      mesh.SetUVs(eastl::move(uvs));

      mesh.SetIndices({
        0, 2, 1,
        0, 3, 2,
      });

      //glm::vec3 scale = glm::vec3(max - min, 1.0f);
      //glm::vec3 center = glm::vec3((min + max) / 2.0f, 1.0f);

      return mesh;// .CreateQuad().TransformMesh(center, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), scale);
    }

    //-------------------------------------------------------------------------
    MaterialPass ImageUIElementData::GetMaterial()
    {
      MaterialPass pass = BaseUIElementData::GetMaterial();
      if (texture)
      {
        pass.SetTexture(0, texture);
      }
      return pass;
    }

    //-------------------------------------------------------------------------
    void ImageUIElementComponent::SetColor(const foundation::Color & color)
    {
      ImageUIElementData* data = static_cast<ImageUIElementData*>(GetData());
      data->color = color;
    }

    //-------------------------------------------------------------------------
    void ImageUIElementComponent::SetTexture(const TextureHandle & texture)
    {
      ImageUIElementData* data = static_cast<ImageUIElementData*>(GetData());
      data->texture = texture;
    }

    //-------------------------------------------------------------------------
    void ImageUIElementComponent::SetTextureRect(const Rect & texture_rect)
    {
      ImageUIElementData* data = static_cast<ImageUIElementData*>(GetData());
      data->texture_rect = texture_rect;
    }
    void ImageUIElementComponent::SetRotation(const float& rotation)
    {
      ImageUIElementData* data = static_cast<ImageUIElementData*>(GetData());
      data->rotation = rotation;
    }
  }
}