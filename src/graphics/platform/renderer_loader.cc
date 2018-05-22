#include "renderer_loader.h"

#ifdef PS_WIN32
#ifdef PS_D3D12
#include "graphics/d3d12/d3d12_renderer.h"
#endif
#ifdef PS_D3D11
#include "graphics/d3d11/d3d11_renderer.h"
#endif

#elif defined PS_PS4
#include "graphics/gnm/ps4_renderer.h"
#endif

#include <engine/core/world.h>

#include <assert.h>

namespace sulphur
{
  namespace graphics
  {
    foundation::SharedPointer<engine::IRenderer> RendererLoader::CreateRenderer(RendererType type)
    {
      switch (type)
      {
      case RendererType::kD3D11:
        return CreateD3D11Renderer();
      case RendererType::kD3D12:
        return CreateD3D12Renderer();
      case RendererType::kVulkan:
        return CreateVulkanRenderer();
      case RendererType::kGNM:
        return CreateGNMRenderer();
      default:
        return foundation::SharedPointer<engine::IRenderer>();
      }
    }

    //------------------------------------------------------------------------------------------------------
#ifdef PS_D3D11
    foundation::SharedPointer<engine::IRenderer> RendererLoader::CreateD3D11Renderer()
    {
      return foundation::Memory::ConstructShared<D3D11Renderer>();
    }
#else
    foundation::SharedPointer<engine::IRenderer> RendererLoader::CreateD3D11Renderer()
    {
      return foundation::SharedPointer<engine::IRenderer>();
    }
#endif

    //------------------------------------------------------------------------------------------------------
#ifdef PS_D3D12
    foundation::SharedPointer<engine::IRenderer> RendererLoader::CreateD3D12Renderer()
    {
      return foundation::Memory::ConstructShared<D3D12Renderer>();
    }
#else
    foundation::SharedPointer<engine::IRenderer> RendererLoader::CreateD3D12Renderer()
    {
      return foundation::SharedPointer<engine::IRenderer>();
    }
#endif

    //------------------------------------------------------------------------------------------------------
#ifdef PS_VULKAN
    foundation::SharedPointer<engine::IRenderer> RendererLoader::CreateVulkanRenderer( )
    {
      return foundation::Memory::ConstructShared<VulkanRenderer>();
    }
#else
    foundation::SharedPointer<engine::IRenderer> RendererLoader::CreateVulkanRenderer()
    {
      return foundation::SharedPointer<engine::IRenderer>();
    }
#endif

    //------------------------------------------------------------------------------------------------------
#ifdef PS_PS4
    foundation::SharedPointer<engine::IRenderer> RendererLoader::CreateGNMRenderer( )
    {
      return foundation::Memory::ConstructShared<GNMRenderer>();
    }
#else
    foundation::SharedPointer<engine::IRenderer> RendererLoader::CreateGNMRenderer()
    {
      return foundation::SharedPointer<engine::IRenderer>();
    }
#endif

    //------------------------------------------------------------------------------------------------------
    foundation::Vector<RendererType> RendererLoader::GetSupportedRenderers()
    {
      foundation::Vector<RendererType> renderers;

      if (IsD3D12Supported())
        renderers.push_back(RendererType::kD3D12);

      if (IsVulkanSupported())
        renderers.push_back(RendererType::kVulkan);

      if (IsD3D11Supported())
        renderers.push_back(RendererType::kD3D11);

      if (IsGNMSupported())
        renderers.push_back(RendererType::kGNM);

      return renderers;
    }

    //------------------------------------------------------------------------------------------------------
    bool RendererLoader::IsD3D11Supported()
    {
    #if defined(PS_WIN32) && defined(PS_D3D11)
      return true;
    #else
      return false;
    #endif
    }

    //------------------------------------------------------------------------------------------------------
    bool RendererLoader::IsD3D12Supported()
    {
    #if defined(PS_WIN32) && defined(PS_D3D12)
      return true;
    #else
      return false;
    #endif
    }

    //------------------------------------------------------------------------------------------------------
    bool RendererLoader::IsVulkanSupported()
    {
    #if defined(PS_WIN32) && defined(PS_FORCE_VULKAN)
      return false;
    #else
      return false;
    #endif
    }

    //------------------------------------------------------------------------------------------------------
    bool RendererLoader::IsGNMSupported()
    {
    #ifdef PS_PS4
      return true;
    #else
      return false;
    #endif
    }
    //------------------------------------------------------------------------------------------------------
  }
}
