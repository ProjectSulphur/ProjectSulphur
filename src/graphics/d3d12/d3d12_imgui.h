#pragma once
#include <engine/graphics/imgui/imgui.h>

#include <d3d12.h>

namespace sulphur 
{
  namespace graphics 
  {
    class D3D12Device;

    /**
    * @brief Initialize ImGUI for DirectX 12
    * @param[in] hwnd (void*) A Windows window handle
    * @param[in] device (sulphur::graphics::D3D12Device&) DirectX 12 device wrapper
    * @param[in] command_list (ID3D12GraphicsCommandList*) The command list to populate when recording ImGUI draw commands
    * @return (bool) Was initialization successful?
    */
    IMGUI_API bool        ImGui_ImplDX12_Init(void* hwnd, D3D12Device& device, ID3D12GraphicsCommandList* command_list);

    /**
    * @brief Shuts down the DirectX 12 ImGUI system. Releases graphics resources. (Needs implementation)
    */
    IMGUI_API void        ImGui_Impl_Shutdown();

    /**
    * @brief Prepares for a new ImGUI frame
    */
    IMGUI_API void        ImGui_Impl_NewFrame();

    /**
    * @brief Use if you want to reset your rendering device without losing ImGui state. Currently not implemented for DirectX 12.
    */
    IMGUI_API void        ImGui_Impl_InvalidateDeviceObjects();

    /**
    * @brief Creates the necessary device objects for ImGUI.
    * @return (bool) Was creation successful?
    */
    IMGUI_API bool        ImGui_Impl_CreateDeviceObjects();
  }
}