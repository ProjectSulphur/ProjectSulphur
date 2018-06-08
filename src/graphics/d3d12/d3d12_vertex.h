// TODO: Remove this struct when done with testing
#include <glm/glm.hpp>

namespace sulphur
{
  namespace graphics
  {
    /**
    * @struct sulphur::graphics::Vertex
    * @brief D3D12 vertex structure.
    * @author Yana Mateeva
    */
    struct Vertex
    {
      glm::fvec3 position; //!< Vertex position.
      glm::fvec3 normal; //!< Vertex normal.
      glm::fvec3 tangent; //!< Vertex tangent.
      glm::fvec2 uv; //!< Vertex UV coordinates.
      glm::fvec4 color; //!< Vertex color.
      glm::fvec4 bone_weights; //!< Per vertex bone weights.
      glm::uvec4 bone_indices; //!< Per vertex bone indices.
    };
  }
}