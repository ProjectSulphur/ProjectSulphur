#pragma once
// ImGui Win32 + DirectX11 binding
// In this binding, ImTextureID is used to store a 'ID3D11ShaderResourceView*' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#include <engine/graphics/imgui/imgui.h>

struct ID3D11Device;
struct ID3D11DeviceContext;

IMGUI_API bool        ImGui_ImplDX11_Init(void* hwnd, ID3D11Device* device, ID3D11DeviceContext* device_context);
IMGUI_API void        ImGui_Impl_Shutdown();
IMGUI_API void        ImGui_Impl_NewFrame();

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_API void        ImGui_Impl_InvalidateDeviceObjects();
IMGUI_API bool        ImGui_Impl_CreateDeviceObjects();

// Handler for Win32 messages, update mouse/keyboard data.
// You may or not need this for your implementation, but it can serve as reference for handling inputs.
// Commented out to avoid dragging dependencies on <windows.h> types. You can copy the extern declaration in your code.

#ifdef PS_WIN32

#ifdef _WIN64
IMGUI_API __int64 ImGui_Impl_WndProcHandler(void* hWnd, unsigned int message, unsigned __int64 wParam, __int64 lParam);
#else
IMGUI_API long ImGui_Impl_WndProcHandler(void* hWnd, unsigned int message, uint wParam, long lParam);
#endif

#endif // PS_WIN32