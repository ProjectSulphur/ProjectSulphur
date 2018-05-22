#pragma once

#include "imgui.h"
#include "extra_fonts/icons_font_awesome.h"
#include "extra_fonts/font_awesome.h"
#include "extra_fonts/font_roboto.h"

inline void ImguiCustomConfig()
{
	
	ImGuiIO& io = ImGui::GetIO();
	
	// Load Font
	io.Fonts->AddFontFromMemoryCompressedTTF(font_roboto_compressed_data, 
																  font_roboto_compressed_size, 
																  14.0f);

	// Merge in font awesome icons
	/*static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config; 
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_compressed_data, 
		                                                            font_awesome_compressed_size,
																	14.0f, 
																	&icons_config,
																	icons_ranges);*/

	// Change the imgui style
	ImGuiStyle& style = ImGui::GetStyle();

	// Settings
	style.WindowPadding = ImVec2(6.0f, 6.0f);
	style.WindowRounding = 0;
	style.ChildWindowRounding = 0;
	style.FramePadding = ImVec2(4.0f, 3.0f);
	style.FrameRounding = 3;
	style.ItemSpacing = ImVec2(8.0f, 3.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 3.0f);
	style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
	style.IndentSpacing = 21.0f;
	style.ScrollbarSize = 12.0f;
	style.ScrollbarRounding = 0;
	style.GrabMinSize = 8.0f;
	style.GrabRounding = 0.0f;
	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);

	// Colors (first section is a dark theme, the second section is a light theme)

#if 1
	style.Colors[ImGuiCol_Text] = ImVec4(0.98f, 0.98f, 0.99f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.80f, 0.79f, 0.80f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.20f, 0.21f, 0.23f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.20f, 0.21f, 0.23f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.11f, 0.13f, 1.00f); //ImVec4(0.30f, 0.32f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.10f, 0.11f, 0.13f, 1.00f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.12f, 0.14f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.31f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.16f, 0.17f, 0.18f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.16f, 0.17f, 0.18f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.17f, 0.18f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.21f, 0.23f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.26f, 0.28f, 0.30f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.34f, 0.37f, 0.40f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.43f, 0.46f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.10f, 0.12f, 0.14f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.25f, 0.27f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.28f, 0.30f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.43f, 0.46f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.28f, 0.30f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.34f, 0.37f, 0.40f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.43f, 0.46f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.21f, 0.23f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.32f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.39f, 0.41f, 0.45f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.26f, 0.28f, 0.30f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.34f, 0.37f, 0.40f, 1.00f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.43f, 0.46f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.28f, 0.30f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.34f, 0.37f, 0.40f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.43f, 0.46f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.26f, 0.28f, 0.30f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.34f, 0.37f, 0.40f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.43f, 0.46f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.75f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.50f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.75f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.13f, 0.14f, 0.16f, 0.80f);
#else
	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
#endif
}
