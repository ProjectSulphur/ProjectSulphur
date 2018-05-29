#pragma once
#include "engine/core/world.h"

#include "engine/systems/component_system.h"

#include "engine/assets/texture.h"
#include "engine/assets/mesh.h"
#include "engine/assets/render_target.h"
#include "engine/core/entity_system.h"

#include <foundation/utils/color.h>

#include <glm/glm.hpp>

// Forward declarations.
namespace sulphur
{
  namespace engine
  {
    class Entity;
    class CanvasSystem;
    class CanvasComponent;
    class CameraSystem;
    class TransformSystem;
    class IRenderer;
    class ImageUIElementComponent;
    class CameraComponent;

#ifdef PS_WIN32
    class Win32Window;
    using Window = Win32Window;
#endif
  }
}

namespace sulphur
{
  namespace engine
  {
    /**
    * @struct sulphur::engine::Rect
    * @brief The rect class is a helper class for the UI system that defines the bounds of an element.
    * @remarks Can be clipped using the included clip function.
    * @author Hilze Vonck
    */
    class Rect
    {
    public:
      /**
      * @brief Initializes rect with a size and position of zero.
      */
      Rect() : position_(0.0f), size_(0.0f) {};
      /**
      * @brief Initializes rect with a provided size and position.
      * @param[in] position (glm::vec2) The position of the rect.
      * @param[in] size (glm::vec2) The size of the rect.
      */
      Rect(glm::vec2 position, glm::vec2 size) :
        position_(position), size_(size) {};
      /**
      * @brief Initializes rect with a provided size and position.
      * @param[in] x (float) The x position of the rect.
      * @param[in] y (float) The y position of the rect.
      * @param[in] w (float) The width of the rect.
      * @param[in] h (float) The height of the rect.
      */
      Rect(float x, float y, float w, float h) :
        position_(x, y), size_(w, h) {};
      /**
      * @brief Creates a rect from a provided min and max.
      * @param[in] min (glm::vec2) The bottom left corner of the rect.
      * @param[in] max (glm::vec2) The top right corner of the rect.
      * @return (Rect) The rectangle created in this function.
      */
      static Rect FromMinMax(glm::vec2 min, glm::vec2 max)
      {
        return Rect(min, max - min);
      }
      /**
      * @brief Returns the position of this rect.
      * @return (glm::vec2) The position of this rect.
      */
      glm::vec2 get_position() const
      {
        return position_;
      }
      /**
      * @brief Returns the size of this rect.
      * @return (glm::vec2) The size of this rect.
      */
      glm::vec2 get_size() const
      {
        return size_;
      }
      /**
      * @brief Returns the bottom left corner of this rect.
      * @return (glm::vec2) The bottom left corner of this rect.
      */
      glm::vec2 Min() const
      {
        return position_;
      }
      /**
      * @brief Returns the top right corner of this rect.
      * @return (glm::vec2) The top right corner of this rect.
      */
      glm::vec2 Max() const
      {
        return position_ + size_;
      }
      /**
      * @brief Returns a rect which is the overlapping portion of this and other rect.
      * @param[in] other (const Rect&) The rect to clip with.
      * @return (Rect) The clipped rect.
      */
      Rect Clip(const Rect& other)
      {
        return Clip(*this, other);
      }
      /**
      * @brief Returns a rect which is the overlapping portion of two rects.
      * @param[in] lhs (const Rect&) The first rect to clip with.
      * @param[in] rhs (const Rect&) The second rect to clip with.
      * @return (Rect) The clipped rect.
      */
      static Rect Clip(const Rect& lhs, const Rect& rhs)
      {
        glm::vec2 min = glm::max(lhs.Min(), rhs.Min());
        glm::vec2 max = glm::min(lhs.Max(), rhs.Max());
        return FromMinMax(min, max);
      }

    private:
      glm::vec2 position_; //<! The rects position
      glm::vec2 size_; //<! The rects size
    };

    /**
    * @struct sulphur::engine::BaseUIElementData
    * @brief This class is a base class that all UI elements should inherit from.
    * @author Hilze Vonck
    */
    struct BaseUIElementData
    {
      /**
      * @brief Constructor that initializes all variables to default.
      */
      BaseUIElementData() = default;
      /**
      * @brief Constructor that copies over the values from another instance.
      */
      BaseUIElementData(const BaseUIElementData& other) :
        element_id(other.element_id),
        depth(other.depth),
        meshes(other.meshes),
        mesh_index(other.mesh_index)
      {
      }
      /**
      * @brief The virtual constructor which will make it so the constructor
      * of inherited classes will be called.
      */
      virtual ~BaseUIElementData() {};

      Entity element_id; // <! The element which this data belongs to.
      float depth = 0.0f; // <! The depth of the UI element.
      foundation::Vector<MeshHandle> meshes; // <! The meshes associated with this UI element.
      uint8_t mesh_index = 0; // <! The current mesh that is being rendered.

      // TODO (Hilze): Find out if there is a better way than this.
      /**
      * @brief Get the global bounds of this object.
      * @param[in] clipped (const bool&) Wether or not the bounds should be clipped.
      * @return (Rect) The bounds of this UI element.
      */
      virtual Rect GetGlobalBounds(const bool& clipped = true);
      /**
      * @brief The mesh that this mesh should be rendered with.
      * @return (Mesh) The mesh associated with this UI element.
      */
      virtual Mesh GetMesh(const Rect& clipped_bounds);
      /**
      * @brief The material that this mesh should be rendered with.
      * @return (MaterialPass) The material associated with this UI element.
      */
      virtual MaterialPass GetMaterial();
      /**
      * @brief Returns the depth of this UI element.
      * @return (float) The depth of this UI element.
      */
      virtual float GetDepth();
    };

    /**
    * @class sulphur::engine::BaseUIElementComponent : ComponentHandleBase
    * @brief The base component for UI elements that all UI elements should inherit from.
    * @author Hilze Vonck
    */
    class BaseUIElementComponent : public ComponentHandleBase
    {
    public:
      using System = CanvasSystem; // !< The base system associated with this component.

    public:
      /**
      * @brief Constructor which initializes the associated system and component.
      * @param[in] system (CanvasSystem*) The canvas system that this component exists in.
      * @param[in] canvas (CanvasComponent&) The canvas that this component exists in.
      */
      BaseUIElementComponent(CanvasSystem* system, CanvasComponent& canvas);
      /**
      * @brief Sets the depth of this UI element.
      * @param[in] depth (const float&) The depth this UI element should be at.
      */
      void SetDepth(const float& depth);
      /**
      * @brief Gets the depth of this UI element.
      * @return (float) The depth this UI element is at.
      */
      float GetDepth();

    protected:
      friend class CanvasComponent;
      /**
      * @brief Gets the canvas data associated with this component.
      * @return (BaseUIElementData*) The data. Pointer for inheritance.
      */
      BaseUIElementData* GetData();

      CanvasComponent& canvas; // !< The canvas that this UI element lives on.
      System* system; // !< The canvas system that this UI elements canvas lives on.

    };

    /**
    * @struct sulphur::engine::UIElementConstructor
    * @brief A helper class for the construction of UI elements.
    * The only way to create UI elements is with the usage of this struct.
    * @tparam C (typename) The component that needs to be constructed.
    * @tparam D (typename) The data structure associated with the component.
    */
    template<typename C, typename D>
    struct UIElementConstructor
    {
      using Component = C; // !< The component.
      using Data = D; // !< The data.
    };

    /**
    * @struct sulphur::engine::CanvasComponent : public ComponentHandleBase
    * @brief The canvas component which will make it easy for you to create and manage UI elements.
    * @author Hilze Vonck
    */
    class CanvasComponent : public ComponentHandleBase
    {
    public:
      using System = CanvasSystem;
      
    public:
      /**
      * @brief Default constructor which will initialize everything to default.
      */
      CanvasComponent();
      /**
      * @brief Constructor which will set the system and the handle.
      * @param[in] system (System&) The canvas system that this component lives in.
      * @param[in] handle (size_t) The handle of this component.
      */
      CanvasComponent(System& system, size_t handle);
      /**
      * @brief Constructor which will set the system, the handle and the camera system.
      * @param[in] camera_system (CameraSystem*) The camera system for some reason.
      * @param[in] system (System&) The canvas system that this component lives in.
      * @param[in] handle (size_t) The handle of this component.
      */
      CanvasComponent(CameraSystem* camera_system, System& system, size_t handle);
      /**
      * @brief Destroys an UI element.
      * @param[in] handle (BaseUIElementComponent) The UI element that needs to be destroyed.
      */
      void Destroy(BaseUIElementComponent handle);
      /**
      * @brief Renders this canvas to the screen.
      * @param[in] renderer (IRenderer&) The renderer.
      */
      void OnRender(IRenderer& renderer);
      /**
      * @brief Destroys an UI element.
      * @tparam C (typename) The component that needs to be constructed.
      * @tparam D (typename) The data structure associated with the component.
      * @param[in] handle (UIElementConstructor < C, D >) The UI element that needs to be constructed.
      * C stands for the component and D for the data structure. This UIElementConstructor is supposed
      * to make constructing UI elements easier.
      * @param[in] entity (Entity&) The entity that this new UI element needs to be attached to.
      * @return (C) The constructed component.
      */
      template<typename C, typename D>
      C Create(UIElementConstructor<C, D>, Entity& entity);

    protected:
      friend class BaseUIElementComponent;
      /**
      * @brief Returns the data associated with an UI element.
      * @param[in] handle (BaseUIElementComponent) The component of which the data needs to be recieved.
      * @return (BaseUIElementData*) The associated data.
      */
      BaseUIElementData* GetData(BaseUIElementComponent handle);

    private:
      /**
      * @brief Constructs an UI element handle and data.
      * @tparam H (typename) The handle which needs to be created.
      * @tparam D (typename) The data structure that needs to be created.
      * @param[in] entity (Entity&) The entity that this UI element needs to be attached to.
      */
      template<typename H, typename D>
      H Create(Entity& entity);
      /**
      * @brief Returns a vector of meshes that can then be used for rendering.
      * @remark All meshes are marked as dynamic.
      * It makes sure that each mesh has its own unique ID.
      * @return (foundation::Vector < MeshHandle > ) The constructed meshes.
      */
      foundation::Vector<MeshHandle> GetMeshes();
      /**
      * @brief Initializes a component. Is a separate function since it is required in a templated
      * function but this way all of the required includes can happen in the .cc file.
      * @param[in] entity (Entity&) The Entity that is associated with the component.
      * @param[in] component (BaseUIElementComponent&) The component of which the data needs to be setup.
      * @return (BaseUIElementData*) The associated data.
      */
      void SetupComponent(Entity& entity, BaseUIElementComponent& component);

      System* system_; // !< The canvas system that this canvas is attached to.
      CameraSystem* camera_system_; // !< The camera system required for rendering.
    };

    /**
    * @enum sulphur::engine::CanvasDataElements
    * @brief All of the different elements that the canvas data is made out of.
    * required by the new entity component system.
    * @author Hilze Vonck
    */
    enum struct CanvasDataElements
    {
      kElements, // !< The UI elements.
      kBaseToElement, // !< A map converting handles to entities.
      kMeshes, // !< The meshes that this canvas can use to render.
      kMeshIndex, // !< The current mesh that is being renderered.
      kRenderTarget, // !< The render target that this UI system will render to.
      kCanvasId // !< The entity that this canvas is attached to.
    };

    /**
    * @struct Sulphur::Engine::CanvasData
    * @brief A structure containing all of the data associated with a canvas.
    */
    struct CanvasData
    {
      using UIElementMap = foundation::Map<Entity, BaseUIElementData*>;
      using BaseToEntity = foundation::Map<BaseUIElementComponent, Entity>;
      using MeshVector   = foundation::Vector<MeshHandle>;
      using ComponentSystemData = SystemData<
        UIElementMap,
        BaseToEntity,
        MeshVector,
        size_t,
        RenderTarget,
        Entity>;
      /**
      * @brief Sets up the data with this element.
      */
      CanvasData() :
        data((void**)&elements)
      {
      }

      UIElementMap elements; // !< The UI elements contained on this canvas.
      BaseToEntity base_to_entity; // !< The map converting components to entities.
      MeshVector   meshes; // !< The meshes that this canvas can use for rendering.
      size_t       mesh_index; // !< The index of the mesh that is currently being used for rendering.
      RenderTarget render_target; // !< The render target that this canvas will render to.
      Entity       canvas_id; // !< The entity that this canvas is attached to.

      ComponentSystemData data; // !< The data structure required by the entity component system.
    };

    /**
    * @struct Sulphur::Engine::CanvasDataRef
    * @brief A structure which acts like a reference structure for the CanvasData struct.
    * Required since it is inpossible to get a reference to the CanvasData struct from the ECS.
    * @author Hilze Vonck
    */
    struct CanvasDataRef
    {
      /**
      * @brief Constructor which takes references to all elements within the CanvasData struct.
      * Required since it is inpossible to get a reference to the CanvasData struct from the ECS.
      * @param[in] elements (CanvasData::UIElementMap&) Reference to UI elements map.
      * @param[in] base_to_entity (CanvasData::BaseToEntity&) Reference to base to entity map.
      * @param[in] meshes (CanvasData::MeshVector&) Reference to base to mesh vector.
      * @param[in] mesh_index (size_t&) Reference to base to mesh index.
      * @param[in] render_target (RenderTarget&) Reference to base to render target.
      * @param[in] canvas_id (Entity&) Reference to base to canvas id.
      */
      CanvasDataRef(
        CanvasData::UIElementMap& elements,
        CanvasData::BaseToEntity& base_to_entity,
        CanvasData::MeshVector& meshes,
        size_t& mesh_index,
        RenderTarget& render_target,
        Entity& canvas_id
      ) :
        elements(elements),
        base_to_entity(base_to_entity),
        meshes(meshes),
        mesh_index(mesh_index),
        render_target(render_target),
        canvas_id(canvas_id)
      {}
    
      CanvasData::UIElementMap& elements; // !< See Sulphur::Engine::CanvasData::elements
      CanvasData::BaseToEntity& base_to_entity; // !< See Sulphur::Engine::CanvasData::base_to_entity
      CanvasData::MeshVector&   meshes; // !< See Sulphur::Engine::CanvasData::meshes
      size_t& mesh_index; // !< See Sulphur::Engine::CanvasData::mesh_index
      RenderTarget& render_target; // !< See Sulphur::Engine::CanvasData::render_target
      Entity& canvas_id; // !< See Sulphur::Engine::CanvasData::canvas_id
    };

    /**
    * @class sulphur::engine::CanvasSystem : public IComponentSystem< CanvasComponent, CanvasData >
    * @brief The system which handles the creation, destruction and rendering of canvases.
    * @author Hilze Vonck
    */
    class CanvasSystem : public IComponentSystem
    {
    private:
      friend class CanvasComponent;

    public:
      /**
      * @brief Constructor which initializes the component system.
      */
      CanvasSystem();
      /**
      * @brief Destructor which destroys all of the associated canvases.
      */
      void OnTerminate() override;
      /**
      * @brief Initializes the canvas system.
      * @param[in] app (Application&) The application which is currently running.
      * @param[in] job_graph (foundation::JobGraph&) The job graph to which this system needs to register itself.
      */
      void OnInitialize(Application& app, foundation::JobGraph& job_graph) override;
      /**
      * @see sulphur::engine::IComponentSystem::Create
      */
      template<typename ComponentT>
      ComponentT Create(Entity& entity) { return Create(entity); };
      /**
      * @brief Destroys a canvas component and all of its UI elements.
      * @param[in] handle (CanvasComponent) The canvas component that needs to be destroyed.
      */
      void Destroy(ComponentHandleBase handle) override;
      /**
      * @brief Creates a canvas component.
      * @param[in] entity (Entity&) The entity that this canvas should be associated with.
      * @return (CanvasComponent) The canvas that is now associated with the passed in entity.
      */
      CanvasComponent Create(Entity& entity);
      /**
      * @brief Helper function for setting the camera.
      * @param[in] camera (CameraComponent&) The camera component that needs to be set.
      */
      void SetCamera(CameraComponent& camera);

    protected:
      /**
      * @brief Get the canvas data associated with a canvas using the CanvasDataRef structure.
      * @param[in] handle (CanvasComponent&) The canvas component of which you want the data.
      * @return (CanvasDataRef) The canvas data that is associated with the passed in component.
      */
      CanvasDataRef GetData(CanvasComponent handle);

    private:
      /**
      * @brief Renders all of the canvases.
      */
      void Render();
      /**
      * @brief Returns a vector of mesh handles which all have their own unique ID.
      * @return (foundation::Vector < MeshHandle >) The vector of mesh handles.
      */
      foundation::Vector<MeshHandle> GetMeshes();

    private:
      CameraSystem* camera_system_; // !< The camera system that the canvas components will use to get the main camera and set their own.
      TransformSystem* transform_system_; // !< The transform system required by the UI elements to find out where they are.
      Window* window_; // !< The active window during intialization. Is used to find out if a canvas has the right size.
      IRenderer* renderer_; // !< The active renderer during initialization. Will be used for rendering.
      size_t current_mesh_count_ = 0; // !< The current amount of created meshes using GetMeshes().

      CanvasData component_data_; //!< An instance of the container that stores per-component data

    public:
      static const size_t kMaxMeshCount = 10u; // !< The maximum amount of meshes that each GetMeshes() function call can return.
    };

    template<typename C, typename D>
    inline C CanvasComponent::Create(UIElementConstructor<C, D>, Entity& entity)
    {
      return Create<C, D>(entity);
    }

    template<typename C, typename D>
    inline C CanvasComponent::Create(Entity& entity)
    {
      CanvasDataRef canvas_data = system_->GetData(*this);
      C component(system_, *this);
      SetupComponent(entity, component);

      D* data = foundation::Memory::Construct<D>();
      data->element_id = entity;
      data->meshes = GetMeshes();

      canvas_data.elements.insert(eastl::make_pair(entity, data));
      canvas_data.base_to_entity.insert(eastl::make_pair(component, entity));

      return component;
    }
  }
}