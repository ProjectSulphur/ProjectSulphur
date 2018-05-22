#include "engine/assets/mesh.h"

#include <foundation/logging/logger.h>

#include <glm/glm.hpp>
#include <glm/gtx/orthonormalize.inl>

namespace sulphur
{
  namespace engine
  {

    //--------------------------------------------------------------------------------
    Mesh::Mesh() :
      topology_(graphics::TopologyType::kTriangle),
      update_index_(false),
      update_pos_(false),
      update_color_(false),
      update_data_(false),
	    static_mesh_(true),
	    always_on_top_(false)
    {
    }

    //--------------------------------------------------------------------------------
    Mesh::Mesh(const Mesh& mesh):
      indices_(mesh.indices_),
      vertices_(mesh.vertices_),
      uvs_(mesh.uvs_),
      normals_(mesh.normals_),
      tangents_(mesh.tangents_),
      colors_(mesh.colors_),
      topology_(mesh.topology_),
      update_index_(true),
      update_pos_(true),
      update_color_(true),
      update_data_(true),
      static_mesh_(mesh.static_mesh_),
      always_on_top_(mesh.always_on_top_)
    {
    }

    //--------------------------------------------------------------------------------
    void Mesh::CalculateBounds()
    {}

    //--------------------------------------------------------------------------------
    void Mesh::RecalculateNormals()
    {}

    //--------------------------------------------------------------------------------
    void Mesh::RecalculateTangents()
    {
      if (topology_ != graphics::TopologyType::kTriangle)
      {
        PS_LOG(Error, "Unable to calculate tangents for anything other then triangle meshes");
        return;
      }

      tangents_.resize(GetVertexCount());

      for (uint i = 0; i < GetIndexCount(); i += 3)
      {
        uint i1 = indices_[i];
        uint i2 = indices_[i + 1];
        uint i3 = indices_[i + 2];

        glm::vec3 p1 = vertices_[i1];
        glm::vec3 p2 = vertices_[i2];
        glm::vec3 p3 = vertices_[i3];

        glm::vec2 uv1 = uvs_[i1];
        glm::vec2 uv2 = uvs_[i2];
        glm::vec2 uv3 = uvs_[i3];


        glm::vec3 tangent;
        glm::vec3 edge1 = p2 - p1;
        glm::vec3 edge2 = p3 - p1;
        glm::vec2 edge1uv = uv2 - uv1;
        glm::vec2 edge2uv = uv3 - uv1;

        float t = edge1uv.x * edge2uv.y - edge2uv.x * edge1uv.y;

        if (t != 0.0f)
        {
          float mul = 1.0f / t;
          tangent = (edge2uv.y * edge1 - edge1uv.y * edge2) * mul;
        }

        tangents_[i1] += tangent;
        tangents_[i2] += tangent;
        tangents_[i3] += tangent;
      }

      for (uint i = 0; i < GetVertexCount(); ++i)
      {
        const glm::vec3& normal = normals_[i];
        glm::vec3& tangent = tangents_[i];

        if (tangent != glm::vec3())
          tangent = glm::orthonormalize(normal, tangent);
      }
    }

    //--------------------------------------------------------------------------------
    Mesh& Mesh::AttachMesh(const Mesh& mesh)
    {
      uint old_vertex_count = (uint)vertices_.size();
      size_t index_size = mesh.indices_.size();
      foundation::Vector<uint32_t> temp_indices;
      temp_indices.reserve(index_size);
      for (size_t i = 0; i < index_size; ++i)
        temp_indices.emplace_back(mesh.indices_[i] + old_vertex_count);

      vertices_.insert(vertices_.end(), mesh.vertices_.begin(), mesh.vertices_.end());
      indices_.insert(indices_.end(), temp_indices.begin(), temp_indices.end());
      uvs_.insert(uvs_.end(), mesh.uvs_.begin(), mesh.uvs_.end());
      normals_.insert(normals_.end(), mesh.normals_.begin(), mesh.normals_.end());
      tangents_.insert(tangents_.end(), mesh.tangents_.begin(), mesh.tangents_.end());
      colors_.insert(colors_.end(), mesh.colors_.begin(), mesh.colors_.end());

      update_index_ = true;
      update_pos_ = true;
      update_color_ = true;
      update_data_ = true;

      return *this;
    }

    //--------------------------------------------------------------------------------
    Mesh& Mesh::AttachMesh(const Mesh& mesh, glm::vec3 position, glm::quat rotation, glm::vec3 scale)
    {
      uint old_vertex_count = (uint)vertices_.size();

      size_t vertex_count = mesh.vertices_.size();
      foundation::Vector<glm::vec3> temp_vertices;
      temp_vertices.reserve(vertex_count);

      glm::vec3 transformed;
      for (size_t i = 0; i < vertex_count; ++i)
      {
        transformed = mesh.vertices_[i] * rotation;
        transformed *= scale;
        transformed += position;
        temp_vertices.emplace_back(transformed);
      }

      size_t index_size = mesh.indices_.size();
      foundation::Vector<uint32_t> tempIndices;
      tempIndices.reserve(index_size);
      for (size_t i = 0; i < index_size; ++i)
      {
        tempIndices.emplace_back(mesh.indices_[i] + old_vertex_count);
      }

      size_t normals_count = mesh.normals_.size();
      foundation::Vector<glm::vec3> temp_normals;
      temp_normals.reserve(normals_count);
      for (size_t i = 0; i < normals_count; ++i)
      {
        if (topology_ == graphics::TopologyType::kPoint) // If this mesh is a point list, store this point's rotation in the normal vector
        {
          temp_normals.emplace_back(glm::eulerAngles(rotation));
        }
        else
        {
          temp_normals.emplace_back(rotation * mesh.normals_[i]);
        }
      }

      size_t tangents_count = mesh.tangents_.size();
      foundation::Vector<glm::vec3> temp_tangents;
      temp_tangents.reserve(tangents_count);
      for (size_t i = 0; i < tangents_count; ++i) {
        if (topology_ == graphics::TopologyType::kPoint) // If this mesh is a point list, store this point's scale in the tangent vector
        {
          temp_tangents.emplace_back(scale);
        }
        else
        {
          temp_tangents.emplace_back(rotation * mesh.tangents_[i]);
        }
      }

      vertices_.insert(vertices_.end(), temp_vertices.begin(), temp_vertices.end());
      indices_.insert(indices_.end(), tempIndices.begin(), tempIndices.end());
      uvs_.insert(uvs_.end(), mesh.uvs_.begin(), mesh.uvs_.end());
      normals_.insert(normals_.end(), temp_normals.begin(), temp_normals.end());
      tangents_.insert(tangents_.end(), temp_tangents.begin(), temp_tangents.end());
      colors_.insert(colors_.end(), mesh.colors_.begin(), mesh.colors_.end());

      update_index_ = true;
      update_pos_ = true;
      update_color_ = true;
      update_data_ = true;

      return *this;
    }

    //--------------------------------------------------------------------------------
    void Mesh::Clear(bool update_render_mesh)
    {
      vertices_.clear();
      indices_.clear();
      uvs_.clear();
      normals_.clear();
      tangents_.clear();
      colors_.clear();

      if (update_render_mesh)
      {
        update_index_ = true;
        update_pos_ = true;
        update_color_ = true;
        update_data_ = true;

#ifdef _DEBUG
        if (!IsStatic())
          PS_LOG(Warning, "Trying to delete data from a dynamic mesh! If updated, this will cause errors.\nAre you sure this mesh should be dynamic?\n");
#endif
      }
    }

    //--------------------------------------------------------------------------------
    Mesh& Mesh::Shade(const foundation::Color& color)
    {
      for (size_t i = 0; i < colors_.size(); ++i)
        colors_[i] *= color;

      update_color_ = true;

      return *this;
    }

    //--------------------------------------------------------------------------------
    Mesh& Mesh::SetColor(const foundation::Color& color)
    {
      for (size_t i = 0; i < colors_.size(); ++i)
        colors_[i] = color;

      update_color_ = true;

      return *this;
    }

    //--------------------------------------------------------------------------------
    Mesh& Mesh::TransformMesh(glm::vec3 position, glm::quat rotation, glm::vec3 scale)
    {
      for (auto it = vertices_.begin(), end = vertices_.end(); it != end; ++it)
        *it = ((rotation * (*it)) * scale) + position;

      for (auto it = normals_.begin(), end = normals_.end(); it != end; ++it)
        *it = rotation * (*it);

      for (auto it = tangents_.begin(), end = tangents_.end(); it != end; ++it)
        *it = rotation * (*it);

      update_pos_ = true;
      update_data_ = true;

      return *this;
    }

    //--------------------------------------------------------------------------------
    Mesh& Mesh::FlipNormals()
    {
      for (size_t i = 0; i < normals_.size(); ++i)
        normals_[i] = -normals_[i];

      update_data_ = true;

      return *this;
    }

    //--------------------------------------------------------------------------------
    void Mesh::MarkDynamic(bool dynamic)
    {
      if (static_mesh_ == !dynamic)
        return;

      static_mesh_ = !dynamic;

      // TODO: Release GPU resource(s)
    }

    //--------------------------------------------------------------------------------
    void Mesh::SetTopologyType(graphics::TopologyType type)
    {
      // TODO: Is empty testing and releasing of data?
      topology_ = type;
    }

    //--------------------------------------------------------------------------------
    void Mesh::SetIndices(foundation::Vector<uint32_t>&& i)
    {
      indices_ = i;
      update_index_ = true;
    }

    //--------------------------------------------------------------------------------
    void Mesh::SetVertices(foundation::Vector<glm::vec3>&& v)
    {
      vertices_ = v;
      update_pos_ = true;
    }

    //--------------------------------------------------------------------------------
    void Mesh::SetUVs(foundation::Vector<glm::vec2>&& u)
    {
      uvs_ = u;
      update_pos_ = true;
    }
    
    //--------------------------------------------------------------------------------
    void Mesh::SetNormals(foundation::Vector<glm::vec3>&& n)
    {
      normals_ = n;
      update_data_ = true;
    }

    //--------------------------------------------------------------------------------
    void Mesh::SetTangents(foundation::Vector<glm::vec3>&& t)
    {
      tangents_ = t;
      update_data_ = true;
    }

    //--------------------------------------------------------------------------------
    void Mesh::SetColors(foundation::Vector<foundation::Color>&& v)
    {
      colors_ = v;
      update_color_ = true;
    }

    //--------------------------------------------------------------------------------
    bool Mesh::Validate() const
    {
      if (vertices_.empty() || indices_.empty())
      {
        PS_LOG(Warning, "Trying to render an empty mesh");
        return false;
      }

#ifdef PS_DEBUG_DEBUG
      uint test_size = 3;
      bool test_equal_indices = false;
      switch (topology_)
      {
      case graphics::TopologyType::kLine:
        test_size = 2;
        break;
      case graphics::TopologyType::kPoint:
        test_size = 1;
        test_equal_indices = true;
        break;
      case graphics::TopologyType::kTriangle:
      default:
        break;
      }

      if (indices_.size() % test_size != 0)
      {
        printError("The indices are not triangulated ! (Index count :" + std::to_string(indices_.size()) + ")\n");
        return false;
      }

      if (test_equal_indices && indices_.size() != vertices_.size())
      {
        printError("The size of 'indices': " + std::to_string(indices_.size()) + " does not match the size of 'vertices': " + std::to_string(vertices_.size()) + "\n");
        return false;
      }

      for (int i = 0; i < indices_.size(); ++i)
      {
        if (indices_[i] >= vertices_.size())
        {
          printError("Index is referencing out of bounds vertex! (Index reference: " + std::to_string(indices_[i]) + ", Vertex count :" + std::to_string(vertices_.size()) + ")\n");
          return false;
        }
      }

      if (!uvs_.empty() && uvs_.size() != vertices_.size())
      {
        printError("The size of 'uvs': " + std::to_string(uvs_.size()) + " does not match the size of 'vertices': " + std::to_string(vertices_.size()) + "\n");
        return false;
      }

      if (!normals_.empty() && normals_.size() != vertices_.size())
      {
        printError("The size of 'normals': " + std::to_string(normals_.size()) + " does not match the size of 'vertices': " + std::to_string(vertices_.size()) + "\n");
        return false;
      }

      if (!tangents_.empty() && tangents_.size() != vertices_.size())
      {
        printError("The size of 'tangents': " + std::to_string(tangents_.size()) + " does not match the size of 'vertices': " + std::to_string(vertices_.size()) + "\n");
        return false;
      }

      if (!colors_.empty() && colors_.size() != vertices_.size())
      {
        printError("The size of 'colors': " + std::to_string(colors_.size()) + " does not match the size of 'vertices': " + std::to_string(vertices_.size()) + "\n");
        return false;
      }
#endif // RENDERING_DEBUGGING

      return true;
    }

    //--------------------------------------------------------------------------------
    void Mesh::Optimize()
    {}

    // AWESOME MESH GENERATION CODE: http://jayelinda.com/modelling-by-numbers-part-two-a/

    //--------------------------------------------------------------------------------
    Mesh Mesh::CreateCube()
    {
      Mesh cube;

      foundation::Vector<glm::vec3> vertices;
      foundation::Vector<glm::vec2> uvs;
      foundation::Vector<glm::vec3> normals;
      foundation::Vector<foundation::Color> colors;
      foundation::Vector<uint32_t> indices;

      //FRONT
      vertices.emplace_back(-0.5f, -0.5f, -0.5f); uvs.emplace_back(0.0f, 1.0f);
      vertices.emplace_back(-0.5f, 0.5f, -0.5f); uvs.emplace_back(0.0f, 0.0f);
      vertices.emplace_back(0.5f, 0.5f, -0.5f); uvs.emplace_back(1.0f, 0.0f);
      vertices.emplace_back(0.5f, -0.5f, -0.5f); uvs.emplace_back(1.0f, 1.0f);

      //BACK
      vertices.emplace_back(-0.5f, -0.5f, 0.5f); uvs.emplace_back(1.0f, 1.0f);
      vertices.emplace_back(0.5f, -0.5f, 0.5f); uvs.emplace_back(0.0f, 1.0f);
      vertices.emplace_back(0.5f, 0.5f, 0.5f); uvs.emplace_back(0.0f, 0.0f);
      vertices.emplace_back(-0.5f, 0.5f, 0.5f); uvs.emplace_back(1.0f, 0.0f);

      //LEFT
      vertices.emplace_back(-0.5f, -0.5f, 0.5f); uvs.emplace_back(0.0f, 1.0f);
      vertices.emplace_back(-0.5f, 0.5f, 0.5f); uvs.emplace_back(0.0f, 0.0f);
      vertices.emplace_back(-0.5f, 0.5f, -0.5f); uvs.emplace_back(1.0f, 0.0f);
      vertices.emplace_back(-0.5f, -0.5f, -0.5f); uvs.emplace_back(1.0f, 1.0f);

      //RIGHT
      vertices.emplace_back(0.5f, -0.5f, -0.5f); uvs.emplace_back(0.0f, 1.0f);
      vertices.emplace_back(0.5f, 0.5f, -0.5f); uvs.emplace_back(0.0f, 0.0f);
      vertices.emplace_back(0.5f, 0.5f, 0.5f); uvs.emplace_back(1.0f, 0.0f);
      vertices.emplace_back(0.5f, -0.5f, 0.5f); uvs.emplace_back(1.0f, 1.0f);

      //TOP
      vertices.emplace_back(-0.5f, 0.5f, -0.5f); uvs.emplace_back(0.0f, 1.0f);
      vertices.emplace_back(-0.5f, 0.5f, 0.5f); uvs.emplace_back(0.0f, 0.0f);
      vertices.emplace_back(0.5f, 0.5f, 0.5f); uvs.emplace_back(1.0f, 0.0f);
      vertices.emplace_back(0.5f, 0.5f, -0.5f); uvs.emplace_back(1.0f, 1.0f);

      //BOTTOM
      vertices.emplace_back(-0.5f, -0.5f, -0.5f); uvs.emplace_back(1.0f, 1.0f);
      vertices.emplace_back(0.5f, -0.5f, -0.5f); uvs.emplace_back(0.0f, 1.0f);
      vertices.emplace_back(0.5f, -0.5f, 0.5f); uvs.emplace_back(0.0f, 0.0f);
      vertices.emplace_back(-0.5f, -0.5f, 0.5f); uvs.emplace_back(1.0f, 0.0f);

      //Generate normals
      for (int i = 0; i < 4; ++i)
        normals.emplace_back(glm::vec3(0.0f, 0.0f, -1.0f));
      for (int i = 0; i < 4; ++i)
        normals.emplace_back(glm::vec3(0.0f, 0.0f, 1.0f));
      for (int i = 0; i < 4; ++i)
        normals.emplace_back(glm::vec3(-1.0f, 0.0f, 0.0f));
      for (int i = 0; i < 4; ++i)
        normals.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
      for (int i = 0; i < 4; ++i)
        normals.emplace_back(glm::vec3(0.0f, 1.0f, 0.0f));
      for (int i = 0; i < 4; ++i)
        normals.emplace_back(glm::vec3(0.0f, -1.0f, 0.0f));

      //Generate indices
      uint index = 0;
      uint face_count = (uint)vertices.size() / 4;
      for (uint i = 0; i < face_count; i++)
      {
        indices.emplace_back(index); //0
        indices.emplace_back(++index); //1
        indices.emplace_back(++index); //2
        indices.emplace_back(index); //2
        indices.emplace_back(++index); //3
        indices.emplace_back(index - 3); //0
        index++;
      }

      // Add colors
      for (uint i = 0; i < vertices.size(); ++i)
        colors.emplace_back(foundation::Color(1.0f, 1.0f, 1.0f));

      cube.SetVertices(eastl::move(vertices));
      cube.SetIndices(eastl::move(indices));
      cube.SetUVs(eastl::move(uvs));
      cube.SetNormals(eastl::move(normals));
      cube.SetColors(eastl::move(colors));

      cube.RecalculateTangents();

      return cube;
    }

    //--------------------------------------------------------------------------------
    Mesh Mesh::CreatePoint()
    {
      Mesh point;
      foundation::Vector<glm::vec3> vertices;
      foundation::Vector<glm::vec2> uvs;
      foundation::Vector<glm::vec3> normals;
      foundation::Vector<glm::vec3> tangents;
      foundation::Vector<foundation::Color> colors;
      foundation::Vector<uint32_t> indices;

      vertices.emplace_back(0.0f, 0.0f, 0.0f); uvs.emplace_back(0.0f, 0.0f);
      normals.emplace_back(glm::vec3(0.0f, 0.0f, -1.0f));
      tangents.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
      indices.emplace_back(0);
      colors.emplace_back(foundation::Color(1.0f, 1.0f, 1.0f));

      point.SetVertices(eastl::move(vertices));
      point.SetIndices(eastl::move(indices));
      point.SetUVs(eastl::move(uvs));
      point.SetNormals(eastl::move(normals));
      point.SetTangents(eastl::move(tangents));
      point.SetColors(eastl::move(colors));

      return point;
    }
    
    //--------------------------------------------------------------------------------
    Mesh Mesh::CreateTriangle()
    {
      Mesh triangle;

      foundation::Vector<glm::vec3> vertices;
      foundation::Vector<glm::vec2> uvs;
      foundation::Vector<glm::vec3> normals;
      foundation::Vector<foundation::Color> colors;
      foundation::Vector<uint32_t> indices;

      const float aspectRatio = 1280.0f / 720.0f;

      vertices.emplace_back(0.0f, 0.25f * aspectRatio, 0.0f); uvs.emplace_back(0.5f, 0.0f);
      vertices.emplace_back(0.25f, -0.25f * aspectRatio, 0.0f); uvs.emplace_back(1.0f, 1.0f);
      vertices.emplace_back(-0.25f, -0.25f * aspectRatio, 0.0f); uvs.emplace_back(0.0f, 1.0f);

      for (size_t i = 0; i < vertices.size(); ++i)
        normals.emplace_back(glm::vec3(0.0f, 0.0f, -1.0f));

      //Generate indices
      indices.emplace_back(0);
      indices.emplace_back(1);
      indices.emplace_back(2);

      // Add colors
      colors.emplace_back(foundation::Color::kRed);
      colors.emplace_back(foundation::Color::kGreen);
      colors.emplace_back(foundation::Color::kBlue);

      triangle.SetVertices(eastl::move(vertices));
      triangle.SetIndices(eastl::move(indices));
      triangle.SetUVs(eastl::move(uvs));
      triangle.SetNormals(eastl::move(normals));
      triangle.SetColors(eastl::move(colors));

      triangle.RecalculateTangents();

      return triangle;
    }

    //--------------------------------------------------------------------------------
    Mesh Mesh::CreateQuad()
    {
      Mesh quad;

      foundation::Vector<glm::vec3> vertices;
      foundation::Vector<glm::vec2> uvs;
      foundation::Vector<glm::vec3> normals;
      foundation::Vector<foundation::Color> colors;
      foundation::Vector<uint32_t> indices;

      vertices.emplace_back(0.0f, 0.0f, 1.0f); uvs.emplace_back(0.0f, 0.0f);
      vertices.emplace_back(0.0f, 1.0f, 1.0f); uvs.emplace_back(0.0f, 1.0f);
      vertices.emplace_back(1.0f, 1.0f, 1.0f); uvs.emplace_back(1.0f, 1.0f);
      vertices.emplace_back(1.0f, 0.0f, 1.0f); uvs.emplace_back(1.0f, 0.0f);

      for (int i = 0; i < 4; ++i)
        normals.emplace_back(glm::vec3(0.0f, 0.0f, -1.0f));

      //Generate indices
      indices.emplace_back(0);
      indices.emplace_back(1);
      indices.emplace_back(2);
      indices.emplace_back(2);
      indices.emplace_back(3);
      indices.emplace_back(0);


      // Add colors
      for (size_t i = 0; i < vertices.size(); ++i)
        colors.emplace_back(foundation::Color::kWhite);

      quad.SetVertices(eastl::move(vertices));
      quad.SetIndices(eastl::move(indices));
      quad.SetUVs(eastl::move(uvs));
      quad.SetNormals(eastl::move(normals));
      quad.SetColors(eastl::move(colors));

      quad.RecalculateTangents();

      return quad;
    }

    //--------------------------------------------------------------------------------
    Mesh Mesh::CreatePlane(uint subDivisions)
    {
      Mesh plane;

      foundation::Vector<glm::vec3> vertices;
      foundation::Vector<glm::vec2> uvs;
      foundation::Vector<glm::vec3> normals;
      foundation::Vector<foundation::Color> colors;
      foundation::Vector<uint32_t> indices;

      float size = 1.0f / subDivisions;

      for (uint x = 0; x <= subDivisions; ++x)
      {
        for (uint z = 0; z <= subDivisions; ++z)
        {
          float _x = x * size - 0.5f;
          float _z = z * size - 0.5f;

          vertices.emplace_back(_x, 0.0f, _z);
          uvs.emplace_back(_x, _z);

          // Visualize sub divisions
          //colors.emplace_back(Utils::Color(_x, _z, 0));
        }
      }

      for (size_t i = 0; i < vertices.size(); ++i)
        normals.emplace_back(glm::vec3(0.0f, 1.0f, 0.0f));

      //Generate indices
      for (uint r = 0; r < subDivisions; ++r)
      {
        for (uint c = 0; c < subDivisions; ++c)
        {
          indices.emplace_back(r * (subDivisions + 1) + c);
          indices.emplace_back(r * (subDivisions + 1) + (c + 1));
          indices.emplace_back((r + 1) * (subDivisions + 1) + c);

          indices.emplace_back((r + 1) * (subDivisions + 1) + c);
          indices.emplace_back((r * (subDivisions + 1) + (c + 1)));
          indices.emplace_back((r + 1) * (subDivisions + 1) + (c + 1));
        }
      }

      // Add colors
      for (size_t i = 0; i < vertices.size(); ++i)
        colors.emplace_back(foundation::Color(1.0f, 1.0f, 1.0f));

      plane.SetVertices(eastl::move(vertices));
      plane.SetIndices(eastl::move(indices));
      plane.SetUVs(eastl::move(uvs));
      plane.SetNormals(eastl::move(normals));
      plane.SetColors(eastl::move(colors));

      plane.RecalculateTangents();

      return plane;
    }

    //--------------------------------------------------------------------------------
    Mesh Mesh::CreatePyramid()
    {
      return CreateCone(1.0f, 1.0f, 4);
    }

    //--------------------------------------------------------------------------------
    Mesh Mesh::CreateCone(float height, float bottomRadius, uint sliceCount, uint stackCount)
    {
      return CreateCylinder(height, 0.0f, bottomRadius, sliceCount, stackCount);
    }

    //--------------------------------------------------------------------------------
    Mesh Mesh::CreateCylinder(float height, float top_radius, float bottom_radius, uint slice_count, uint stackCount)
    {
      Mesh cylinder;

      foundation::Vector<glm::vec3> vertices;
      foundation::Vector<glm::vec2> uvs;
      foundation::Vector<glm::vec3> normals;
      foundation::Vector<glm::vec3> tangents;
      foundation::Vector<foundation::Color> colors;
      foundation::Vector<uint32_t> indices;

      float stack_height = height / stackCount;
      float radius_step = (top_radius - bottom_radius) / stackCount;
      uint ring_count = stackCount + 1;
      float d_theta = glm::two_pi<float>() / slice_count;

      for (uint i = 0; i < ring_count; ++i)
      {
        float y = -0.5f*height + i*stack_height;
        float r = bottom_radius + i*radius_step;

        // vertices of ring
        for (uint j = 0; j <= slice_count; ++j)
        {
          float c = cosf(j*d_theta);
          float s = sinf(j*d_theta);
          vertices.emplace_back(glm::vec3(r*c, y, r*s));
          colors.emplace_back(foundation::Color(1.0f, 1.0f, 1.0f));
          uvs.emplace_back(glm::vec2((float)j / slice_count, 1.0f - (float)i / stackCount));

          glm::vec3 T(-s, 0.0f, c);
          float dr = bottom_radius - top_radius;
          glm::vec3 B(dr*c, -height, dr*s);
          glm::vec3 N = glm::normalize(glm::cross(T, B));

          tangents.emplace_back(T);
          normals.emplace_back(N);
        }
      }

      // Add one because we duplicate the first and last vertex per ring
      // since the texture coordinates are different.
      uint ring_vertex_count = slice_count + 1;
      // Compute indices for each stack.
      for (uint i = 0; i < stackCount; ++i)
      {
        for (uint j = 0; j < slice_count; ++j)
        {
          indices.push_back(i*ring_vertex_count + j);
          indices.push_back((i + 1)*ring_vertex_count + j);
          indices.push_back((i + 1)*ring_vertex_count + j + 1);
          indices.push_back(i*ring_vertex_count + j);
          indices.push_back((i + 1)*ring_vertex_count + j + 1);
          indices.push_back(i*ring_vertex_count + j + 1);
        }
      }

      // Build top cap
      uint base_index = (uint)vertices.size();
      float y = 0.5f*height;

      // Duplicate cap ring vertices because the texture coordinates and normals differ.
      for (uint i = 0; i <= slice_count; ++i)
      {
        float x = top_radius*cosf(i*d_theta);
        float z = top_radius*sinf(i*d_theta);
        // Scale down by the height to try and make top cap texture coord area
        // proportional to base.
        float u = x / height + 0.5f;
        float v = z / height + 0.5f;
        vertices.emplace_back(glm::vec3(x, y, z));
        colors.emplace_back(foundation::Color(1.0f, 1.0f, 1.0f));
        normals.emplace_back(glm::vec3(0.0f, 1.0f, 0.0f));
        tangents.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
        uvs.emplace_back(glm::vec2(u, v));
      }
      // Cap center vertex.
      vertices.emplace_back(glm::vec3(0.0f, y, 0.0f));
      colors.emplace_back(foundation::Color(1.0f, 1.0f, 1.0f));
      normals.emplace_back(glm::vec3(0.0f, 1.0f, 0.0f));
      tangents.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
      uvs.emplace_back(glm::vec2(0.5f, 0.5f));

      // Index of center vertex.
      uint center_index = (uint)vertices.size() - 1;
      for (uint i = 0; i < slice_count; ++i)
      {
        indices.push_back(center_index);
        indices.push_back(base_index + i + 1);
        indices.push_back(base_index + i);
      }

      // Build bottom cap
      base_index = (uint)vertices.size();
      y = -0.5f*height;

      for (uint i = 0; i <= slice_count; ++i)
      {
        float x = bottom_radius*cosf(i*d_theta);
        float z = bottom_radius*sinf(i*d_theta);
        // Scale down by the height to try and make top cap texture coord area
        // proportional to base.
        float u = x / height + 0.5f;
        float v = z / height + 0.5f;
        vertices.emplace_back(glm::vec3(x, y, z));
        colors.emplace_back(foundation::Color(1.0f, 1.0f, 1.0f));
        normals.emplace_back(glm::vec3(0.0f, -1.0f, 0.0f));
        tangents.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
        uvs.emplace_back(glm::vec2(u, v));
      }
      // Cap center vertex.
      vertices.emplace_back(glm::vec3(0.0f, y, 0.0f));
      colors.emplace_back(foundation::Color(1.0f, 1.0f, 1.0f));
      normals.emplace_back(glm::vec3(0.0f, -1.0f, 0.0f));
      tangents.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
      uvs.emplace_back(glm::vec2(0.5f, 0.5f));

      // Cache the index of center vertex.
      center_index = (uint)vertices.size() - 1;
      for (uint i = 0; i < slice_count; ++i)
      {
        indices.push_back(center_index);
        indices.push_back(base_index + i);
        indices.push_back(base_index + i + 1);
      }

      cylinder.SetVertices(eastl::move(vertices));
      cylinder.SetIndices(eastl::move(indices));
      cylinder.SetUVs(eastl::move(uvs));
      cylinder.SetNormals(eastl::move(normals));
      cylinder.SetTangents(eastl::move(tangents));
      cylinder.SetColors(eastl::move(colors));
      return cylinder;
    }

    //--------------------------------------------------------------------------------
    Mesh Mesh::CreateCircle(uint segments)
    {
      Mesh circle;

      foundation::Vector<glm::vec3> vertices;
      foundation::Vector<glm::vec2> uvs;
      foundation::Vector<glm::vec3> normals;
      foundation::Vector<glm::vec3> tangents;
      foundation::Vector<foundation::Color> colors;
      foundation::Vector<uint32_t> indices;

      float d_theta = glm::two_pi<float>() / segments;

      for (uint i = 0; i <= segments; ++i)
      {
        float x = cosf(i*d_theta);
        float z = sinf(i*d_theta);

        float u = x + 0.5f;
        float v = z + 0.5f;
        vertices.emplace_back(glm::vec3(x, z, 0.0f));
        colors.emplace_back(foundation::Color(1.0f, 1.0f, 1.0f));
        normals.emplace_back(glm::vec3(0.0f, 1.0f, 0.0f));
        tangents.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
        uvs.emplace_back(glm::vec2(u, v));
      }
      // Center vertex.
      vertices.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f));
      colors.emplace_back(foundation::Color(1.0f, 1.0f, 1.0f));
      normals.emplace_back(glm::vec3(0.0f, 1.0f, 0.0f));
      tangents.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
      uvs.emplace_back(glm::vec2(0.5f, 0.5f));

      uint center_index = (uint)vertices.size() - 1;
      for (uint i = 0; i < segments; ++i)
      {
        indices.push_back(center_index);
        indices.push_back(i + 1);
        indices.push_back(i);
      }

      circle.SetVertices(eastl::move(vertices));
      circle.SetIndices(eastl::move(indices));
      circle.SetUVs(eastl::move(uvs));
      circle.SetNormals(eastl::move(normals));
      circle.SetColors(eastl::move(colors));

      circle.RecalculateTangents();

      return circle;
    }

    //--------------------------------------------------------------------------------
    Mesh Mesh::CreateHexPointy()
    {
      Mesh hex;

      foundation::Vector<glm::vec3> vertices;
      foundation::Vector<glm::vec2> uvs;
      foundation::Vector<glm::vec3> normals;
      foundation::Vector<foundation::Color> colors;
      foundation::Vector<uint32_t> indices;

      vertices.emplace_back(0.0f, 0.0f, 0.0f); uvs.emplace_back(0.5f, 0.5f); // Center
      vertices.emplace_back(-0.433012f, 0.0f, 0.25f); uvs.emplace_back(0.0f, 0.75f); // Top left
      vertices.emplace_back(0.0f, 0.0f, 0.5f); uvs.emplace_back(0.5f, 1.0f); // Top
      vertices.emplace_back(0.433012f, 0.0f, 0.25f); uvs.emplace_back(1.0f, 0.75f); // Top right
      vertices.emplace_back(0.433012f, 0.0f, -0.25f); uvs.emplace_back(1.0f, 0.25f); // Bottom right
      vertices.emplace_back(0.0f, 0.0f, -0.5f); uvs.emplace_back(0.5f, 0.0f); // Bottom
      vertices.emplace_back(-0.433012f, 0.0f, -0.25f); uvs.emplace_back(0.0f, 0.25f); // Bottom left

      for (size_t i = 0; i < vertices.size(); ++i)
        normals.emplace_back(glm::vec3(0.0f, 1.0f, 0.0f));

      //Generate indices
      for (int i = 1; i < 7; ++i)
      {
        indices.emplace_back(0); //0
        indices.emplace_back(i); //1
        indices.emplace_back(i == 6 ? 1 : i + 1); //2

      }

      // Add colors
      for (size_t i = 0; i < vertices.size(); ++i)
        colors.emplace_back(foundation::Color(1.0f, 1.0f, 1.0f));

      hex.SetVertices(eastl::move(vertices));
      hex.SetIndices(eastl::move(indices));
      hex.SetUVs(eastl::move(uvs));
      hex.SetNormals(eastl::move(normals));
      hex.SetColors(eastl::move(colors));

      hex.RecalculateTangents();

      return hex;
    }

    //--------------------------------------------------------------------------------
    Mesh Mesh::CreateHexFlat()
    {
      Mesh hex;

      foundation::Vector<glm::vec3> vertices;
      foundation::Vector<glm::vec2> uvs;
      foundation::Vector<glm::vec3> normals;
      foundation::Vector<foundation::Color> colors;
      foundation::Vector<uint32_t> indices;

      vertices.emplace_back(0.0f, 0.0f, 0.0f); uvs.emplace_back(0.5f, 0.5f); // Center
      vertices.emplace_back(-0.5f, 0.0f, 0.0f); uvs.emplace_back(0.0f, 0.5f); // Left
      vertices.emplace_back(-0.25f, 0.0f, 0.5f); uvs.emplace_back(0.25f, 1.0f); // Top left
      vertices.emplace_back(0.25f, 0.0f, 0.5f); uvs.emplace_back(0.75f, 1.0f); // Top right
      vertices.emplace_back(0.5f, 0.0f, 0.0f); uvs.emplace_back(1.0f, 0.5f); // Right
      vertices.emplace_back(0.25f, 0.0f, -0.5f); uvs.emplace_back(0.75f, 0.0f); // Bottom right
      vertices.emplace_back(-0.25f, 0.0f, -0.5f); uvs.emplace_back(0.25f, 0.0f); // Bottom left

      for (size_t i = 0; i < vertices.size(); ++i)
        normals.emplace_back(glm::vec3(0.0f, 1.0f, 0.0f));

      //Generate indices
      for (uint i = 1; i < 7; ++i)
      {
        indices.emplace_back(0); //0
        indices.emplace_back(i); //1
        indices.emplace_back(i == 6 ? 1 : i + 1); //2

      }

      // Add colors
      for (size_t i = 0; i < vertices.size(); ++i)
        colors.emplace_back(foundation::Color(1.0f, 1.0f, 1.0f));

      hex.SetVertices(eastl::move(vertices));
      hex.SetIndices(eastl::move(indices));
      hex.SetUVs(eastl::move(uvs));
      hex.SetNormals(eastl::move(normals));
      hex.SetColors(eastl::move(colors));

      hex.RecalculateTangents();

      return hex;
    }

    //--------------------------------------------------------------------------------
    Mesh Mesh::CreateSphere(uint slice_count, uint stack_count)
    {
      Mesh sphere;

      foundation::Vector<glm::vec3> vertices;
      foundation::Vector<glm::vec2> uvs;
      foundation::Vector<glm::vec3> normals;
      foundation::Vector<foundation::Color> colors;
      foundation::Vector<uint32_t> indices;

      vertices.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
      normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
      uvs.push_back(glm::vec2(1.0f, 0.0f));
      colors.push_back(foundation::Color::kWhite);

      float phi_step = glm::pi<float>() / stack_count;
      float theta_step = glm::two_pi<float>() / slice_count;

      //! Vertices.

      for (uint i = 1; i <= stack_count - 1; ++i)
      {
        float phi = i * phi_step;

        for (uint j = 0; j <= slice_count; ++j)
        {
          float theta = j * theta_step;

          glm::vec3 p;
          p.x = sinf(phi)*cosf(theta);
          p.y = cosf(phi);
          p.z = sinf(phi)*sinf(theta);

          glm::vec3 n;
          n = glm::normalize(p);

          glm::vec2 uv;
          uv.x = theta / glm::two_pi<float>();
          uv.y = phi / glm::pi<float>();

          vertices.push_back(p);
          normals.push_back(n);
          uvs.push_back(uv);
          colors.push_back(foundation::Color::kWhite);
        }
      }

      vertices.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
      normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
      uvs.push_back(glm::vec2(1.0f, 0.0f));
      colors.push_back(foundation::Color::kWhite);

      //! Indices.

      for (uint i = 1; i <= slice_count; ++i)
      {
        indices.push_back(0);
        indices.push_back(i + 1);
        indices.push_back(i);
      }

      uint base_index = 1;
      uint ring_vertex_count = slice_count + 1;
      for (uint i = 0; i < stack_count - 2; ++i)
      {
        for (uint j = 0; j < slice_count; ++j)
        {
          indices.push_back(base_index + i*ring_vertex_count + j);
          indices.push_back(base_index + i*ring_vertex_count + j + 1);
          indices.push_back(base_index + (i + 1)*ring_vertex_count + j);

          indices.push_back(base_index + (i + 1)*ring_vertex_count + j);
          indices.push_back(base_index + i*ring_vertex_count + j + 1);
          indices.push_back(base_index + (i + 1)*ring_vertex_count + j + 1);
        }
      }

      uint south_pole_index = (uint)vertices.size() - 1;
      base_index = south_pole_index - ring_vertex_count;
      for (uint i = 0; i < slice_count; ++i)
      {
        indices.push_back(south_pole_index);
        indices.push_back(base_index + i);
        indices.push_back(base_index + i + 1);
      }

      //! Combine.

      sphere.SetVertices(eastl::move(vertices));
      sphere.SetIndices(eastl::move(indices));
      sphere.SetUVs(eastl::move(uvs));
      sphere.SetNormals(eastl::move(normals));
      sphere.SetColors(eastl::move(colors));

      sphere.RecalculateTangents();

      return sphere;
    }
  }
}