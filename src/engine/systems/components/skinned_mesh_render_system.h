#pragma once

#include "engine/assets/asset_interfaces.h"
#include "engine/systems/component_system.h"
#include "engine/scripting/scriptable_object.h"
#include "engine/assets/scriptable_asset_system.h"

#include <foundation/containers/vector.h>
#include <foundation/containers/hash_map.h>

#include <glm/mat4x4.hpp>

namespace sulphur
{
  namespace engine
  {
    template<typename T>
    struct Keyframe;

    using FloatKeyframe = Keyframe<float>;
    using Vector2Keyframe = Keyframe<glm::vec2>;
    using Vector3Keyframe = Keyframe<glm::vec3>;
    using Vector4Keyframe = Keyframe<glm::vec4>;
    using QuaternionKeyframe = Keyframe<glm::quat>;

    class Mesh;
    using MeshHandle = AssetHandle<Mesh>;

    class Material;
    using MaterialHandle = AssetHandle<Material>;

    class Skeleton;
    using SkeletonHandle = AssetHandle<Skeleton>;

    class Animation;
    using AnimationHandle = AssetHandle<Animation>;

    class Model;
    using ModelHandle = AssetHandle<Model>;

    class Entity;
    class IRenderer;
    class CameraSystem;
    class TransformSystem;
    class SkinnedMeshRenderSystem;

    /**
    * @class sulphur::engine::SkinnedMeshRenderComponent
    * @brief A component that allows you to add a skinned mesh to an entity
    * @author Riko Ophorst
    */
    SCRIPT_CLASS() class SkinnedMeshRenderComponent : public ComponentHandleBase
    {
    public:
      SCRIPT_NAME(SkinnedMeshRenderComponent);
      SCRIPT_COMPONENT();

      using System = SkinnedMeshRenderSystem; //!< System type define

      /**
      * @brief Default constructor (creates an empty/invalid handle)
      */
      SkinnedMeshRenderComponent();

      /**
      * @brief Constructor for creating a handle from an integral value
      * @param[in] system (sulphur::engine::SkinnedMeshRendererComponent::System&) A reference to the relevant system that owns this component.
      * @param[in] handle (size_t) The integral value to use for constructing the handle.
      */
      SkinnedMeshRenderComponent(System& system, size_t handle);

      /**
      * @brief Sets the Mesh that this SkinnedMeshRenderComponent renders.
      * @param[in] mesh (const sulphur::engine::MeshHandle&) The new Mesh.
      * @remarks This automatically resizes the Materials set stored
      *          inside of this component to be the same size as the
      *          number of submeshes stored in the new mesh. No new
      *          materials are set whatsoever, however if the new
      *          number of submeshes is greater than the old number
      *          of submeshes, this function adds appropriate
      *          default materials in those new places.
      */
      void SetMesh(const MeshHandle& mesh);

      /**
      * @brief Scriptable version of SetMesh.
      * @param[in] mesh (sulphur::engine::ScriptableAsset*) A scriptable asset which type is kMesh.
      * @remarks This function has exactly the same functionality as SetMesh(), except that it also
      *          performs a check on whether the argument is actually a Mesh.
      */
      SCRIPT_FUNC() void SetMesh(ScriptableAsset* mesh);

      /**
      * @brief Sets the Model that this SkinnedMeshRenderComponent renders.
      * @param[in] model (const sulphur::engine::ModelHandle&) The new Model.
      * @remarks This function sets the appropriate mesh handles and material handles on this
      *          component using SetMesh() and SetMaterials().
      */
      void SetModel(const ModelHandle& model);

      /**
      * @brief Scriptable version of SetModel.
      * @param[in] model (sulphur::engine::ScriptableAsset*) A scriptable asset which type is kModel.
      * @remarks This function has exactly the same functionality as SetModel(), except that it also
      *          performs a check on whether the argument is actually a Model.
      */
      SCRIPT_FUNC() void SetModel(ScriptableAsset* model);

      /**
      * @brief Retrieves the Mesh that is set on this SkinnedMeshRenderComponent.
      * @return (const sulphur::engine::MeshHandle&) The Mesh that is set on this SkinnedMeshRenderComponent.
      */
      const MeshHandle& GetMesh() const;

      /**
      * @brief Sets a Material at a specific index on this component.
      * @param[in] material (const sulphur::engine::MaterialHandle&) The new material.
      * @param[in] index (size_t) The index at which the new material should be slotted.
      * @remarks If the Material passed in is invalid, a default 
      *          error Material is set instead. If the index is
      *          invalid (by being larger than the number of 
      *          submeshes in the mesh set on this component)
      *          then no action will happen. If you are passing
      *          in an invalid material, it will be replaced by
      *          a default error material.
      */
      void SetMaterial(const MaterialHandle& material, size_t index = 0);

      /**
      * @brief Retrieves a Material from a specific index from this component.
      * @param[in] index (size_t) The index from which to retrieve the material.
      * @returns The Material that resides at the given index.
      * @remarks If the index is invalid (by being larger than the 
      *          number of submeshes in the mesh set on this
      *          component) this will return an invalid handle.
      */
      MaterialHandle GetMaterial(size_t index = 0) const;

      /**
      * @brief Sets a set of Materials on this component.
      * @param[in] materials (const sulphur::foundation::Vector<MaterialHandle>&) The set of materials.
      * @remarks Every ith material in the set of materials will be
      *          applied to every ith submesh in this component's
      *          assigned Mesh during rendering. If you pass in more
      *          materials than submeshes, that is no problem. If
      *          you pass in less materials than submeshes, a default
      *          material will be added for every missing material.
      *          If you are passing in an invalid material, it will
      *          be replaced by a default error material.
      */
      void SetMaterials(const foundation::Vector<MaterialHandle>& materials);

      /**
      * @brief Retrieves the set of Materials on this component.
      * @returns (const foundation::Vector<MaterialHandle>&) The set of Materials on this component.
      * @remarks Remember that every ith material gets applied to
      *          every ith submesh in this component's Mesh.
      */
      const foundation::Vector<MaterialHandle>& GetMaterials() const;

      /**
      * @brief Sets whether this object should be rendered.
      * @param[in] visible (bool) Whether it is visible or not.
      */
      SCRIPT_FUNC() void SetVisible(bool visible);

      /**
      * @brief Returns whether this object is being rendered or not.
      * @returns (bool) Whether this object is visible or not.
      */
      SCRIPT_FUNC() bool IsVisible() const;

      /**
      * @brief Sets whether this object should cast shadows or not.
      * @param[in] casts_shadows (bool) Whether this object should cast shadows or not.
      */
      SCRIPT_FUNC() void SetCastsShadows(bool casts_shadows);

      /**
      * @brief Returns whether this object is casting shadows or not.
      * @returns (bool) Whether this object is casting shadows or not.
      */
      SCRIPT_FUNC() bool CastsShadows() const;

      /**
      * @brief Sets the animation that is on this component.
      * @param[in] handle (sulphur::engine::AnimationHandle) A handle to an animation.
      * @remarks This does not start playing the animation automatically. To start playing the
      *          animation, call Play() on this component.
      */
      void SetAnimation(AnimationHandle handle);

      /**
      * @brief Scriptable version of SetAnimation().
      * @param[in] animation (sulphur::engine::ScriptableAsset*) A scriptable asset which type is kAnimation.
      * @remarks This function has exactly the same functionality as SetAnimation(), except that it also
      *          performs a check on whether the argument is actually an Animation.
      */
      SCRIPT_FUNC() void SetAnimation(ScriptableAsset* animation);

      /**
      * @brief Retrieves the Animation that is assigned to this component.
      * @returns (sulphur::engine::AnimationHandle) The handle to the animation that is assigned.
      */
      AnimationHandle GetAnimation() const;

      /**
      * @brief Sets the skeleton that is on this component.
      * @param[in] handle (sulphur::engine::AnimationHandle) A handle to an skeleton.
      */
      void SetSkeleton(SkeletonHandle handle);

      /**
      * @brief Scriptable version of SetSkeleton().
      * @param[in] skeleton (sulphur::engine::ScriptableAsset*) A scriptable asset which type is kSkeleton.
      * @remarks This function has exactly the same functionality as SetSkeleton(), except that it also
      *          performs a check on whether the argument is actually a Skeleton.
      */
      SCRIPT_FUNC() void SetSkeleton(ScriptableAsset* skeleton);

      /**
      * @brief Retrieves the Skeleton that is assigned to this component.
      * @returns (sulphur::engine::SkeletonHandle) The handle to the skeleton that is assigned.
      */
      SkeletonHandle GetSkeleton() const;

      /**
      * @brief Starts playing the animation from the start.
      * @remarks This resets the playback time to 0 and then starts playing the animation.
      */
      SCRIPT_FUNC() void Play();

      /**
      * @brief Stops playing the animation.
      * @remarks This stops playing the animation and also sets the playback time to 0.
      */
      SCRIPT_FUNC() void Stop();

      /**
      * @brief This pauses the playback of the animation.
      * @remarks This does not affect playback time.
      */
      SCRIPT_FUNC() void Pause();

      /**
      * @brief This resumes the playback of the animation.
      * @remarks This does not affect playback time.
      */
      SCRIPT_FUNC() void Resume();

      /**
      * @brief Sets whether the animation is currently playing.
      * @param[in] playing (bool) Whether the animation should be played back or not.
      * @param[in] reset_playback_time (bool) Whether the playback time should be reset to 0.
      */
      SCRIPT_FUNC() void SetPlaying(bool playing, bool reset_playback_time);

      /**
      * @brief Retrieves whether the animation is being played back.
      * @returns (bool) Whether the animation is being played back.
      */
      SCRIPT_FUNC() bool IsPlaying() const;

      /**
      * @brief Toggles the playback state of the animation.
      * @param[in] reset_playback_time (bool) Whether the playback time should be reset to 0.
      * @returns (bool) The new playback state of the animation.
      */
      SCRIPT_FUNC() bool TogglePlaying(bool reset_playback_time = false);

      /**
      * @brief Sets the playback time of the animation.
      * @param[in] playback_time (float) The new playback time of the animation in seconds.
      */
      SCRIPT_FUNC() void SetPlaybackTime(float playback_time);

      /**
      * @brief Retrieves the current playback time of the animation.
      * @returns (float) The current playback time of the animation.
      */
      SCRIPT_FUNC() float GetPlaybackTime() const;

      /**
      * @brief Sets the playback speed of the animation.
      * @param[in] playback_speed (float) The new playback speed of the animation.
      * @remarks A playback speed of 1.0 = 100% playback speed. 0.5 = 50%. 2.0 = 200%. etc.
      */
      SCRIPT_FUNC() void SetPlaybackSpeed(float playback_speed);

      /**
      * @brief Retrieves the playback speed of the animation.
      * @returns (float) The playback speed of the animation.
      * @remarks A playback speed of 1.0 = 100% playback speed. 0.5 = 50%. 2.0 = 200%. etc.
      */
      SCRIPT_FUNC() float GetPlaybackSpeed() const;

      /**
      * @brief Retrieves the bone matrices as they are currently calculated for the animation.
      * @returns (const sulphur::foundation::Vector<glm::mat4>&) The bone matrices as they are currently calculated for the animation.
      * @remarks These bone matrices get calculated once per frame by the parent SkinnedMeshRenderSystem.
      * @remarks These bone matrices are the ones that are sent to the GPU for processing by the vertex shader.
      * @remarks These bone matrices are transformed appropriately to the current state of the animation.
      */
      const foundation::Vector<glm::mat4>& GetBoneMatrices() const;

    private:
      SkinnedMeshRenderSystem* system_; //!< The system that owns this component
    };

    /**
    * @enum sulphur::engine::SkinnedMeshRenderComponentElements
    */
    enum struct SkinnedMeshRenderComponentElements
    {
      kEntity,
      kMesh,
      kMaterials,
      kVisible,
      kCastsShadows,
      kSkeleton,
      kAnimation,
      kGlobalPlaybackTime,
      kLocalPlaybackTime,
      kIsPlaying,
      kPlaybackSpeed,
      kBoneMatrices
    };

    
    /**
    * @class sulphur::engine::SkinnedMeshRenderSystemData
    * @brief Contains all the data that the SkinnedMeshRenderSystem owns for each component. It
    * outlines exactly what the data is that is stored per component. Each member is a pointer to an
    * array of structures.
    * @author Riko Ophorst
    */
    class SkinnedMeshRenderSystemData
    {
    public:
      /**
      * @brief Constructor of the data that initializes the system data by
      * passing it the list of pointers. It uses the first element for this and 
      * assumes that the others follow.
      */
      SkinnedMeshRenderSystemData() :
        data(reinterpret_cast<void**>(&entity))
      {
      }

      Entity* entity; //!< Array of entities attached to each component.
      MeshHandle* mesh; //!< Array of meshes to be rendered per component.
      foundation::Vector<MaterialHandle>* materials; // Array of material handle arrays per component.
      bool* visible; //!< Array of visibility flags per component.
      bool* casts_shadows; //!< Array of shadow casting flags per component.
      SkeletonHandle* skeleton; //!< Array of skeletons per component.
      AnimationHandle* animation; //!< Array of animations per component.
      float* global_playback_time_in_seconds; //!< Array of unbound playback times in seconds per component.
      float* local_playback_time_in_ticks; //!< Array of playback times in ticks bound in the range of [0 - animation duration] per component.
      bool* is_playing; //!< Array of playing flags per component.
      float* playback_speed; //!< Array of playback speed multipliers per components.
      foundation::Vector<glm::mat4>* bone_matrices; //!< Array of transform bone matrices arrays per component.

      /** 
      * @brief Short-hand for the system data of this component. Allows easy access of data that is 
      * contained by the system per component.
      * @remarks Note that the template arguments are exactly the same as the types of data stored
      * in the actual system data.
      */
      using ComponentSystemData = SystemData<
        Entity, 
        MeshHandle, 
        foundation::Vector<MaterialHandle>,
        bool, 
        bool,
        SkeletonHandle,
        AnimationHandle,
        float,
        float,
        bool,
        float,
        foundation::Vector<glm::mat4>
      >;

      ComponentSystemData data; //!< System data of the component.
    };

    /**
    * @class sulphur::engine::SkinnedMeshRenderSystem
    * @brief The ComponentSystem that manages all SkinnedMeshes in the World. Sends all the
    * necessary instructions to the IRenderer so that SkinnedMeshes get rendered appropriately.
    * @see sulphur::engine::SkinnedMeshRenderComponent
    * @author Riko Ophorst
    */
    class SkinnedMeshRenderSystem : 
      public IComponentSystem
    {
    public:
      /** Default constructor */
      SkinnedMeshRenderSystem();

      /**
      * @internal
      * @see sulphur::engine::IComponentSystem::OnInitialize
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;

      /**
      * @internal
      * @see sulphur::engine::IComponentSystem::OnTerminate
      */
      void OnTerminate() override;

      /**
      * @see sulphur::engine::IComponentSystem::Create
      */
      template<typename ComponentT>
      ComponentT Create(Entity& entity) { return Create(entity); };

      /**
      * @brief Destroys the mesh renderer component
      * @param[in] handle (ComponentHandleBase) The component to destroy
      * @see sulphur::engine::IComponentSystem::Destroy
      */
      void Destroy(ComponentHandleBase handle) override;

      /**
      * @brief Create a new SkinnedMeshRenderComponent for this entity and also creates a 
      * TransformComponent if it wasn't attached yet.
      * @param[in] entity (sulphur::engine::Entity) The entity to create this component for.
      * @see sulphur::engine::IComponentSystem::Create
      */
      SkinnedMeshRenderComponent Create(Entity& entity);

      /**
      * @brief Calculates the bone matrix of a given node index based on the current animation and skeleton.
      * @param[in] component_index (unsigned int) The index of the component for which the bones should be calculated.
      * @param[in] node_index (unsigned int) The index of the node for which you want to calculate the bone matrix.
      * @param[in] parent_transform (const glm::mat4&) The transform of the parent node.
      * @remarks This is a recursive function that traverses the entire hierarchy of nodes until there are no
      *          more child nodes left.
      * @remarks Recommended usage is to call this function with the index of the root node and the transform
      *          of the entity the eventual bone matrices will be applied on.
      * @remarks This function automatically updates the bone_matrices_ array for the given component.
      */
      void CalculateBoneTransform(
        unsigned int component_index,
        unsigned int node_index,
        const glm::mat4& parent_transform
      );

      /**
      * @brief Retrieves the index of an animation channel based on a node's name.
      * @param[in] component_index (unsigned int) The index of the component from where the animation channel should be searched for.
      * @param[in] node_name (const sulphur::foundation::String&) The name of the node for which you want the index of the animation channel.
      * @param[out] out_channel_index (unsigned int*) The index of the animation channel will be outputted in this argument.
      * @remarks If there is no animation channel that can be found for a given node name, out_channel_index
      *          will not be modified at all.
      */
      void GetChannelByNodeName(
        unsigned int component_index,
        const foundation::String& node_name,
        unsigned int* out_channel_index) const;

      /**
      * @brief Processes a set of Vector3Keyframes based on a given playback time in ticks, returning the exact
      *        value of the animation sequence at the given playback time.
      * @param[in] playback_time (float) The playback time of the animation in ticks.
      * @param[in] keyframes (const sulphur::foundation::Vector<sulphur::engine::Vector3Keyframe>&) The keyframes of the animation sequence that should be processed.
      * @remarks If the number of keyframes passed in is 0, the return value will be vec3(1, 1, 1).
      * @remarks If the number of keyframes passed in is 1, the return value will be keyframes[0].value.
      */
      glm::vec3 ProcessKeyframes(
        float playback_time,
        const foundation::Vector<Vector3Keyframe>& keyframes) const;

      /**
      * @brief Processes a set of QuaternionKeyframes based on a given playback time in ticks, returning the exact
      *        value of the animation sequence at the given playback time.
      * @param[in] playback_time (float) The playback time of the animation in ticks.
      * @param[in] keyframes (const sulphur::foundation::Vector<sulphur::engine::QuaternionKeyframe>&) The keyframes of the animation sequence that should be processed.
      * @remarks If the number of keyframes passed in is 0, the return value will be quat(1, 0, 0, 0).
      * @remarks If the number of keyframes passed in is 1, the return value will be keyframes[0].value.
      */
      glm::quat ProcessKeyframes(
        float playback_time,
        const foundation::Vector<QuaternionKeyframe>& keyframes) const;
      
    private:
      /**
      * @brief Updates all the AnimationStates in the SkinnedMeshRenderSystem.
      */
      void UpdateAnimationStates();

      /**
      * @brief Renders all the SkinnedMeshRenderComponents in the World to the screen.
      */
      void RenderMeshes();

    public:
      /**
      * @brief Retrieves the underlying component data held inside of the system.
      * @returns (sulphur::engine::SkinnedMeshRenderSystemData&) The underlying component data held inside of the system.
      */
      SkinnedMeshRenderSystemData& data();

    private:
      CameraSystem* camera_system_;       //!< Keep a pointer to the CameraSystem.
      TransformSystem* tranform_system_;  //!< Keep a pointer to the TransformSystem.
      IRenderer* renderer_;               //!< Keep a pointer to the IRenderer.

      SkinnedMeshRenderSystemData component_data_; //!< An instance of the container that stores per-component data
    };
  }
}