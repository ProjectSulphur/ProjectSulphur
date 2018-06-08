#pragma once
#include "canvas_system.h"

namespace sulphur
{
  namespace engine
  {
    /**
    * @struct Sulphur::Engine::ImageUIElementData : public BaseUIElementData
    * @brief The data associated with the ImageUIElementComponent.
    * @author Hilze Vonck
    */
    struct ImageUIElementData : public BaseUIElementData
    {
      /**
      * @brief Base constructor which initializes everything to its default values.
      */
      ImageUIElementData() : BaseUIElementData() {};
      /**
      * @brief Copy constructor which initializes everything to the others values.
      */
      ImageUIElementData(const ImageUIElementData& other) :
        BaseUIElementData(other),
        texture(other.texture),
        color(other.color),
        texture_rect(other.texture_rect)
      {
      }

      TextureHandle texture; // !< The texture that this image uses for rendering.
      foundation::Color color = foundation::Color::kWhite; // !< The color of the image.
      Rect texture_rect = Rect(glm::vec2(0.0f), glm::vec2(1.0f)); // !< The texture rect of this image.
      float rotation = 0.0f; //!< The rotation of the image.
      /**
      * @brief Returns the global bounds of this image.
      * @param[in] clipped (const bool&) Currently not used.
      * @return (Rect) This images global bounds.
      * @see UIElementComponent::GetGlobalBounds.
      */
      virtual Rect GetGlobalBounds(const bool& clipped = true) override;
      /**
      * @brief Returns a mesh based upon this meshes global and clipped bounds.
      * @param[in] clipped_bounds (const Rect&) The bounds of this UI elment.
      * @return (Mesh) This images mesh.
      * @see UIElementComponent::GetMesh.
      * @todo Start using the clipped_bounds as actual clipped bounds. Not just bounds.
      */
      virtual Mesh GetMesh(const Rect& clipped_bounds) override;
      /**
      * @brief Returns the material pass from the base function with the texture set.
      * @return (MaterialPass) The material pass associated with this UI element.
      * @see UIElementComponent::GetMaterial.
      */
      virtual MaterialPass GetMaterial() override;
    };
    
    /**
    * @class Sulphur::Engine::ImageUIElementComponent : public BaseUIElementComponent
    * @brief The handle for an UI image component. Will display a colored and textured rect to the screen.
    */
    class ImageUIElementComponent : public BaseUIElementComponent
    {
    public:
      /**
      * @brief Initializes this canvas component and the base component so this component can be used.
      * @param[in] system (CanvasSystem*) The canvas system that this component is attached to.
      * @param[in] canvas (CanvasComponent&) The canvas component that this component is attached to.
      */
      ImageUIElementComponent(CanvasSystem* system, CanvasComponent& canvas, const Entity& entity) :
        BaseUIElementComponent(system, canvas, entity) {};
      /**
      * @brief Copies component data from one component into the other.
      * @param[in] other (const ImageUIElementComponent&) The component which will be copied into this component.
      * @return (ImageUIElementComponent&) Returns this component after the copying is done.
      */
      ImageUIElementComponent& operator=(const ImageUIElementComponent& other)
      {
        system = other.system;
        canvas = other.canvas;
        return *this;
      }
      /**
      * @brief Sets the color.
      * @param[in] color (const foundation::Color&) The color that this image should be using.
      */
      void SetColor(const foundation::Color& color);
      /**
      * @brief Sets the texture.
      * @param[in] texture (const TextureHandle&) The texture that this image should be using.
      */
      void SetTexture(const TextureHandle& texture);
      /**
      * @brief Sets the texture rect.
      * @param[in] texture_rect (const Rect&) The texture rect that this image should be using.
      */
      void SetTextureRect(const Rect& texture_rect);
      /**
      * @brief Sets the rotation of this UI element. HACK NEEDS TO BE REMOVED.
      * @param[in] rotation (const float&) The new rotation.
      */
      void SetRotation(const float& rotation);
    };

    using ImageUIConstructor = UIElementConstructor<ImageUIElementComponent, ImageUIElementData>;
  }
}