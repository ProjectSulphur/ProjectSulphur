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

    //-------------------------------------------------------------------------
    Mesh ImageUIElementData::GetMesh(const Rect & clipped_bounds)
    {
      // TODO (Hilze): Do something with the clipped bounds.

      glm::vec2 min = clipped_bounds.Min();
      glm::vec2 max = clipped_bounds.Max();

      glm::vec2 tex_min = texture_rect.Min();
      glm::vec2 tex_max = texture_rect.Max();

      Mesh mesh;

      mesh.SetVertices({
        glm::vec3(min.x, max.y, 0.5f),
        glm::vec3(min.x, min.y, 0.5f),
        glm::vec3(max.x, min.y, 0.5f),
        glm::vec3(max.x, max.y, 0.5f)
      });
      mesh.SetNormals({
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)
      });
      mesh.SetUVs({
        glm::vec2(tex_min.x, tex_max.y),
        glm::vec2(tex_min.x, tex_min.y),
        glm::vec2(tex_max.x, tex_min.y),
        glm::vec2(tex_max.x, tex_max.y)
      });

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
  }
}