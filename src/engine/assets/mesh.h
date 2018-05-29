#pragma once
#include "engine/assets/asset_interfaces.h"

#include <graphics/platform/pipeline_state.h>

#include <foundation/utils/color.h>
#include <foundation/containers/vector.h>
#include <foundation/utils/shapes.h>

namespace sulphur 
{
  namespace engine 
  {
    /**
    * @struct SubMeshOffset
    * @brief A struct to find the offset for a sub-mesh inside of a 'Mesh'
    * @author Jelle de Haan
    */
    struct SubMeshOffset
    {
      uint32_t offset;
      uint32_t size;
    };

    /**
    * @class Mesh
    * @brief A collection of data that forms a model, used for rendering
    * @author Jelle de Haan
    */
    class Mesh
    {
    public:
      /**
      * @brief Constructs an empty mesh
      */
      Mesh();

      /**
      * @brief Creates a copy of the mesh
      * @param[in] mesh (const Mesh&) The mesh to create a copy of
      */
      Mesh(const Mesh& mesh);

      /**
      * @brief Assigns a copy of the mesh to this mesh
      * @param[in] mesh (const Mesh&) The mesh to copy
      */
      Mesh& operator=(const Mesh& mesh);

      /**
      * @brief Calculates the bounding box of this mesh. (CURRENTLY DOES NOTHING!)
      */
      void CalculateBounds();

      /**
      * @brief Recalculates the normals, using the mesh's vertex and index data. (CURRENTLY DOES NOTHING!)
      */
      void RecalculateNormals();

      /**
      * @brief Recalculates the tangents, using the mesh's vertex and normal data.
      */
      void RecalculateTangents();

      /**
      * @brief Flips all normals of this mesh.
      * @return This mesh, used for chaining
      */
      Mesh& FlipNormals();

      /**
      * @brief Attaches a copy of the mesh to this mesh.
      * @param[in] mesh (const Mesh&) The mesh to attach
      * @return This mesh, used for chaining
      */
      Mesh& AttachMesh(const Mesh& mesh);

      /**
      * @brief Attaches a copy of the mesh to this mesh.
      * @param[in] mesh (const Mesh&) The mesh to attach
      * @param[in] transform (Transform&) A transform to use on the vertex data before attaching
      * @return this mesh
      */
      Mesh& AttachMesh(const Mesh& mesh,
        glm::vec3 position,
        glm::quat rotation = glm::quat(1, 0, 0, 0),
        glm::vec3 scale = glm::vec3(1, 1, 1));

      /**
      * @brief Validates the data that is currently attached to this mesh.
      * @return (bool) Returns false if the mesh is invalid (will output error messages).
      */
      bool Validate() const;

      /**
      * @brief Optimizes this mesh for cache locality. (CURRENTLY DOES NOTHING!)
      */
      void Optimize();

      /**
      * @brief Clears all the data from the mesh
      * @param[in] update_render_mesh (bool) Should this mesh be updated on the GPU, or can ignore the changes?
      */
      void Clear(bool update_render_mesh = false);

      /**
      * @brief Shades the entire mesh with a specific color.
      * @param[in] color (const foundation::Color&) The color used to multiply with the colors currently on this mesh
      * @return (Mesh) This mesh, used for chaining
      */
      Mesh& Shade(const foundation::Color& color);

      /**
      * @brief Replaces all colors on the entire mesh to a specific color.
      * @param[in] color (const foundation::Color&) The color to replace with
      * @return This mesh, used for chaining
      */
      Mesh& SetColor(const foundation::Color& color);

      /**
      * @brief Transforms all the vertex data of this mesh, please only use to offset a mesh.
      * @param[in] color (const Transform&) The transform used to offset the mesh
      * @return (Mesh) This mesh, used for chaining
      */
      Mesh& TransformMesh(glm::vec3 position,
        glm::quat rotation = glm::quat(1, 0, 0, 0),
        glm::vec3 scale = glm::vec3(1, 1, 1));

      /**
      * @brief Marks this mesh as a dynamic mesh.
      * @param[in] dynamic (bool) 
      * @remarks A dynamic mesh is optimized for frequent edits.
      * @remarks This should ONLY be used if this mesh is going to be edited at a later state.
      * @remarks For example when animating or using it for batching.
      */
      void MarkDynamic(bool dynamic = true);

      /**
      * @brief Modify the topology type of this mesh, to form for ex. a line list.
      * @param[in] type (graphics::TopologyType) The new topology type of the mesh
      */
      void SetTopologyType(graphics::TopologyType type);

      /**
      * @brief Get the current topology type of the mesh
      * @return (graphics::TopologyType) The current topology type
      */
      graphics::TopologyType topology_type() const { return topology_; };

      /**
      * @brief Set the index data
      * @param[in] indices (foundation::Vector<uint32_t>&&) The new indices that define this submesh
      * @param[in] submesh (uint) The index of the submesh to modify
      * @remarks Must match the data count topology type of the mesh
      */
      void SetIndices(foundation::Vector<uint32_t>&& indices, uint submesh = 0);

      /**
      * @brief Set the vertex data
      */
      void SetVertices(foundation::Vector<glm::vec3>&& vertices);

      /**
      * @brief Set the uv data
      * @remarks Must match the vertex count of this mesh
      */
      void SetUVs(foundation::Vector<glm::vec2>&& uvs);

      /**
      * @brief Set the normal data
      * @remarks Must match the vertex count of this mesh
      */
      void SetNormals(foundation::Vector<glm::vec3>&& normals);

      /**
      * @brief Set the tangent data
      * @remarks Must match the vertex count of this mesh
      */
      void SetTangents(foundation::Vector<glm::vec3>&& tangents);

      /**
      * @brief Set the color data
      * @remarks Must match the vertex count of this mesh
      */
      void SetColors(foundation::Vector<foundation::Color>&& colors);

      /**
      * @brief Set the per-vertex bone weights of this mesh.
      * @param[in] bone_weights (sulphur::foundation::Vector<glm::vec4>&&) The new bone weights.
      * @remarks Must match the vertex count of this mesh.
      */
      void SetBoneWeights(foundation::Vector<glm::vec4>&& bone_weights);

      /**
      * @brief Set the per-vertex bone indices of this mesh.
      * @param[in] bone_indices (sulphur::foundation::Vector<glm::vec<4, uint32_t>>&&) The new bone indices.
      * @remarks Must match the vertex count of this mesh.
      */
      void SetBoneIndices(foundation::Vector<glm::vec<4, uint32_t>>&& bone_indices);

      /**
       * @brief Sets the bounding box of the mesh.
       * @param[in] bounding_box (const sulphur::foundation::AABB&) The new bounding box.
       */
      void SetBoundingBox(const foundation::AABB& bounding_box);
      /**
      * @brief Sets the bounding sphere of the mesh.
      * @param[in] bounding_sphere (const sulphur::foundation::Sphere&) The new bounding sphere.
      */
      void SetBoundingSphere(const foundation::Sphere& bounding_sphere);

      /**
      * @brief Returns all indices this mesh is build out of
      * @return (const foundation::Vector<uint32_t>&) The index list of the mesh
      */
      const foundation::Vector<uint32_t>& GetIndices() const { return indices_; };

      /**
      * @brief Returns all indices this mesh is build out of
      * @param[in] submesh (uint) The index of the submesh to get
      * @return (foundation::Vector<uint32_t>) The index list of the mesh
      */
      foundation::Vector<uint32_t> GetIndicesAt(uint submesh = 0) const;

      /**
      * @brief Returns a submesh struct with offset and size into the index buffer
      * @param[in] submesh (uint) The index of the submesh to get
      * @return (const foundation::Vector<uint32_t>&) The index list of the mesh
      */
      const SubMeshOffset& GetSubmesh(uint submesh) const;

      /**
      * @brief Returns the amount of submeshes contained in this mesh
      * @return (size_t) The amount of submeshes
      */
      size_t GetSubmeshCount() const { return submesh_offsets_.size(); };

      /**
      * @brief Returns all vertices this mesh is build out of
      * @return (const foundation::Vector<uint32_t>&) The vertex list of the mesh
      */
      const foundation::Vector<glm::vec3>& GetVertices() const { return vertices_; };

      /**
      * @brief Returns all uvs this mesh is build out of
      * @return (const foundation::Vector<uint32_t>&) The uv list of the mesh
      */
      const foundation::Vector<glm::vec2>& GetUVs() const { return uvs_; };

      /**
      * @brief Returns all normals this mesh is build out of
      * @return (const foundation::Vector<uint32_t>&) The normal list of the mesh
      */
      const foundation::Vector<glm::vec3>& GetNormals() const { return normals_; };

      /**
      * @brief Returns all tangents this mesh is build out of
      * @return (const foundation::Vector<uint32_t>&) The tangent list of the mesh
      */
      const foundation::Vector<glm::vec3>& GetTangents() const { return tangents_; };

      /**
      * @brief Returns all colors this mesh is build out of
      * @return (const foundation::Vector<uint32_t>&) The color list of the mesh
      */
      const foundation::Vector<foundation::Color>& GetColors() const { return colors_; };

      /**
      * @brief Returns all per-vertex bone weights this mesh is build out of
      * @return (const sulphur::foundation::Vector<glm::vec4>&) The bone weights list of the mesh
      */
      const foundation::Vector<glm::vec4>& GetBoneWeights() const { return bone_weights_; };

      /**
      * @brief Returns all per-vertex bone indices this mesh is build out of
      * @return (const sulphur::foundation::Vector<glm::vec<4, uint32_t>>&) The bone indices list of the mesh
      */
      const foundation::Vector<glm::vec<4, uint32_t>>& GetBoneIndices() const { return bone_indices_; }

      /**
       * @brief Getter for the bounding box of the mesh.
       * @return (const sulphur::foundation::AABB&) The bounding box of the mesh.
       */
      const foundation::AABB& bounding_box() const { return bounding_box_; }
      /**
      * @brief Getter for the bounding sphere of the mesh.
      * @return (const sulphur::foundation::Sphere&) The bounding sphere of the mesh.
      */
      const foundation::Sphere& bounding_sphere() const { return bounding_sphere_; }

      /**
      * @brief Combines all change flags to test if anything in the mesh has changed
      * @return (bool) Has anything in the mesh changed
      */
      bool HasChanged() const { return (update_index_ || update_pos_ || update_color_ || update_data_); }

      // temp func. remove before submit
      void set_has_changed(bool value) { update_index_ = update_pos_ = update_color_ = update_data_ = value; }

      /**
      * @internal
      * @brief Returns true if the index data has changed and it should be updated
      * @return (bool) Has the index data changed
      */
      bool ShouldUpdateIndexBuffer() const { return update_index_; };

      /**
      * @internal
      * @brief Returns true if the position data has changed and it should be updated
      * @return (bool) Has the position data changed
      */
      bool ShouldUpdatePositionBuffer() const { return update_pos_; };

      /**
      * @internal
      * @brief Returns true if the color data has changed and it should be updated
      * @return (bool) Has the color data changed
      */
      bool ShouldUpdateColorBuffer() const { return update_color_; };

      /**
      * @internal
      * @brief Returns true if any other mesh data has changed and it should be updated
      * @return (bool) Has any other mesh data changed
      */
      bool ShouldUpdateDataBuffer() const { return update_data_; };

      /**
      * @brief Return true if the data in this mesh will never change
      * @return (bool) Is this mesh's data static
      * @see sulphur::engine::Mesh::MarkDynamic()
      */
      bool IsStatic() const { return static_mesh_; };

      /**
      * @brief Returns true if the index count of this mesh is null
      * @return (bool) Is the mesh empty
      */
      bool IsEmpty() const { return GetIndexCount() == 0; };

      /**
      * @brief Should this mesh always show up on top of all other meshes
      * @return (bool) Is the mesh always on top
      */
      bool IsAlwaysOnTop() const { return always_on_top_; };

      /**
      * @brief Sets whether this mesh should always show up on top of all other meshes
      * @param[in] on_top (bool) true if the mesh should always appear on top
      */
      void SetAlwaysOnTop(bool on_top) { always_on_top_ = on_top; };

      /**
      * @brief Get the current vertex count of this mesh
      * @return (uint) The amount of vertices
      */
      uint GetVertexCount() const { return (uint)vertices_.size(); };

      /**
      * @brief Get the current index count of this mesh
      * @return (uint) The amount of indices
      */
      uint GetIndexCount() const { return (uint)indices_.size(); };

      /**
      * @brief Creates a mesh with a single point
      * @return (Mesh) The point mesh
      */
      static Mesh CreatePoint();

      /**
      * @brief Creates a triangle mesh, facing towards negative z
      * @return (Mesh) The triangle mesh
      */
      static Mesh CreateTriangle();

      /**
      * @brief Creates a quad mesh, facing towards negative z
      * @return (Mesh) The quad mesh
      */
      static Mesh CreateQuad();

      /**
      * @brief Creates a cube mesh
      * @return (Mesh) The cube mesh
      */
      static Mesh CreateCube();

      /**
      * @brief Creates a plane mesh, facing towards positive y
      * @param[in] sub_divisions (uint) The amount of sub division quads on the plane
      * @return (Mesh) The plane mesh
      */
      static Mesh CreatePlane(uint sub_divisions = 1);

      /**
      * @brief Creates a pyramid mesh
      * @return (Mesh) The pyramid mesh
      */
      static Mesh CreatePyramid();

      /**
      * @brief Creates a cone mesh
      * @param[in] height (float) The height of the cone
      * @param[in] bottom_radius (float) The bottom radius of the cone
      * @param[in] slice_count (uint) The amount of sub divisions around the cone (on the X and Z axis) 
      * @param[in] stack_count (uint) The amount sub divisions on the Y axis
      * @return (Mesh) The cone mesh
      */
      static Mesh CreateCone(float height = 1.5f, float bottom_radius = 0.5f, uint slice_count = 24, uint stack_count = 1);

      /**
      * @brief Creates a cylinder mesh
      * @param[in] height (float) The height of the cylinder
      * @param[in] top_radius (float) The top radius of the cylinder
      * @param[in] bottom_radius (float) The bottom radius of the cylinder
      * @param[in] slice_count (uint) The amount of sub divisions around the cylinder (on the X and Z axis)
      * @param[in] stack_count (uint) The amount sub divisions on the Y axis
      * @return (Mesh) The cylinder mesh
      */
      static Mesh CreateCylinder(float height = 1.5f, float top_radius = 0.5f, float bottom_radius = 0.5f, uint slice_count = 24, uint stack_count = 1);

      /**
      * @brief Creates a circle mesh, facing towards negative z
      * @param[in] segments (uint) The amount of sub divisions around the circle (on the X and Y axis)
      * @return (Mesh) The circle mesh
      */
      static Mesh CreateCircle(uint segments = 24);

      /**
      * @brief Creates a line circle, facing towards negative z
      * @param[in] segments (uint) The amount of sub divisions around the circle (on the X and Y axis)
      * @return (Mesh) The line circle mesh
      */
      static Mesh CreateLineCircle(uint segments = 24);

      /**
      * @brief Creates a hex mesh, with a pointy top
      * @return (Mesh) The hex mesh
      */
      static Mesh CreateHexPointy();

      /**
      * @brief Creates a hex mesh, with a flat top
      * @return (Mesh) The hex mesh
      */
      static Mesh CreateHexFlat();

      /**
      * @brief Creates a sphere mesh
      * @param[in] slice_count (uint) The amount of sub divisions around the sphere (on the X and Y axis)
      * @param[in] stack_count (uint) The amount sub divisions on the Y axis
      * @return (Mesh) The sphere mesh
      */
      static Mesh CreateSphere(uint slice_count = 24, uint stack_count = 24);



    private:
      foundation::Vector<uint32_t> indices_;                      //!< Set of indices for this Mesh.
      foundation::Vector<glm::vec3> vertices_;                    //!< Set of positions for this Mesh.
      foundation::Vector<glm::vec2> uvs_;                         //!< Set of UV coordinates for this Mesh.
      foundation::Vector<glm::vec3> normals_;                     //!< Set of normals for this Mesh.
      foundation::Vector<glm::vec3> tangents_;                    //!< Set of tangents for this Mesh.
      foundation::Vector<foundation::Color> colors_;              //!< Set of colors for this Mesh.
      foundation::Vector<glm::vec4> bone_weights_;                //!< Set of bone weights for this Mesh.
      foundation::Vector<glm::vec<4, uint32_t>> bone_indices_;    //!< Set of bone indices for this Mesh.

      /**
      * Offsets to be used by the renderer. Each offset is a subsection of the triangles in this
      * Mesh. Each different offset therefore represents only a part of the entire Mesh. With these
      * different parts of the mesh, it is possible to apply a variable number of materials to this
      * Mesh. Therefore, each offset == one material. The number of materials applied to this Mesh 
      * must therefore be exactly equal to the number of individual submesh offsets in the Mesh.
      * The order of the materials versus the order of submesh offsets is also relevant. The first
      * material gets applied to the first submesh offset, the second material gets applied to the
      * second submesh offset, etc. 
      */   
      foundation::Vector<SubMeshOffset> submesh_offsets_;

      graphics::TopologyType topology_; //!< The topology type of this mesh.

      bool update_index_;
      bool update_pos_;
      bool update_color_;
      bool update_data_;
      bool static_mesh_;
      bool always_on_top_;

      foundation::AABB bounding_box_;     //!< The bounding box of the mesh
      foundation::Sphere bounding_sphere_;//!< The bounding sphere of the mesh

      //Functions:
      // Recalculate normals

      // Triangle strip optimization: http://www.codercorner.com/Strips.htm
    };

    using MeshHandle = AssetHandle<Mesh>; //!< Asset handle to a mesh
  }
}
