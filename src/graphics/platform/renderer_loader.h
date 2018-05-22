#pragma once
#include <engine/graphics/irenderer.h>

#include <foundation/memory/memory.h>
#include <foundation/containers/vector.h>

namespace sulphur
{
  namespace graphics
  {
    /**
    * @enum sulphur::graphics::RendererType
    * @brief Describes the renderer API used
    */
    enum struct RendererType {
      kD3D11,
      kD3D12,
      kVulkan,
      kGNM
    };

    class D3D11Renderer;
    class D3D12Renderer;
    class VulkanRenderer;
    class GNMRenderer;

    /**
    * @class RendererLoader
    * @brief This class is used for loading and creating the enabled renderers
    * @author Jelle de Haan
    */
    class RendererLoader
    {
    public:
      /**
      * @brief Create a renderer of type sulphur::graphics::RendererType
      * @param[in] type (sulphur::graphics::RendererType) The API type of this renderer
      * @return (sulphur::foundation::SharedPointer <sulphur::engine::IRenderer>) A shared pointer to the renderer or a nullptr if the renderer could not be created
      */
      static foundation::SharedPointer<engine::IRenderer> CreateRenderer(RendererType type);
      
      /**
      * @brief Creates a renderer of type sulphur::graphics::RendererType::kD3D11
      * @return (sulphur::foundation::SharedPointer <sulphur::engine::IRenderer>) A shared pointer to the renderer or a nullptr if the renderer could not be created
      */
      static foundation::SharedPointer<engine::IRenderer> CreateD3D11Renderer();
      
      /**
      * @brief Creates a renderer of type sulphur::graphics::RendererType::kD3D12
      * @return (sulphur::foundation::SharedPointer <sulphur::engine::IRenderer>) A shared pointer to the renderer or a nullptr if the renderer could not be created
      */
      static foundation::SharedPointer<engine::IRenderer> CreateD3D12Renderer();
      
      /**
      * @brief Creates a renderer of type sulphur::graphics::RendererType::kVulkan
      * @return (sulphur::foundation::SharedPointer <sulphur::engine::IRenderer>) A shared pointer to the renderer or a nullptr if the renderer could not be created
      */
      static foundation::SharedPointer<engine::IRenderer> CreateVulkanRenderer();
      
      /**
      * @brief Creates a renderer of type sulphur::graphics::RendererType::kGNM
      * @return (sulphur::foundation::SharedPointer <sulphur::engine::IRenderer>) A shared pointer to the renderer or a nullptr if the renderer could not be created
      */
      static foundation::SharedPointer<engine::IRenderer> CreateGNMRenderer();

      /**
      * @brief Creates and returns a list of all renderer types supported on the current device
      * @return (sulphur::foundation::Vector <sulphur::graphics::RendererType>) The list of supported renderer types
      */
      static foundation::Vector<RendererType> GetSupportedRenderers();

      /**
      * @brief Returns true is DirectX 11 is supported on the current device
      * @return (bool) Is DirectX 11 supported?
      */
      static bool IsD3D11Supported();

      /**
      * @brief Returns true is DirectX 12 is supported on the current device
      * @return (bool) Is DirectX 12 supported?
      */
      static bool IsD3D12Supported();

      /**
      * @brief Returns true is Vulkan is supported on the current device
      * @return (bool) Is Vulkan supported?
      */
      static bool IsVulkanSupported();

      /**
      * @brief Returns true is GNM is supported on the current device
      * @return (bool) Is GNM supported?
      */
      static bool IsGNMSupported();
    };
  }
}