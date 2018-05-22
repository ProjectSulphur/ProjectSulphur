SET(PS_USE_VULKAN CACHE BOOL "Should Vulkan be built?")

IF (PS_WIN32)
	SET(PS_D3D_VERSION "12" CACHE STRING "The D3D version to use")
	SET_PROPERTY(CACHE PS_D3D_VERSION PROPERTY STRINGS "11" "12" "both" "none")
ELSE ()
	SET(PS_D3D_VERSION "none" CACHE STRING "The D3D version to use")
ENDIF (PS_WIN32)

SET(GraphicsAPISources "")

IF (${PS_FORCE_VULKAN})
	SET(PS_VULKAN TRUE)
	ADD_DEFINITIONS("-DPS_VULKAN")
	
	IMPORT_FOLDER(GraphicsVulkanSources "vulkan")
	SET(GraphicsAPISources ${GraphicsAPISources} ${GraphicsVulkanSources})
ENDIF (${PS_FORCE_VULKAN})

IF ("${PS_D3D_VERSION}" STREQUAL "12" OR "${PS_D3D_VERSION}" STREQUAL "both")
	SET(PS_D3D12 TRUE)
	ADD_DEFINITIONS("-DPS_D3D12")
	
	IMPORT_FOLDER(GraphicsD3D12Sources "d3d12")
	IMPORT_FOLDER(GraphicsD3D12AssetsSources "d3d12/assets")
	IMPORT_FOLDER(GraphicsD3D12UtilsSources "d3d12/utils")
	SET(GraphicsAPISources 
		${GraphicsAPISources} 
		${GraphicsD3D12Sources}
		${GraphicsD3D12AssetsSources}
		${GraphicsD3D12UtilsSources}
	)
ENDIF ()

IF ("${PS_D3D_VERSION}" STREQUAL "11" OR "${PS_D3D_VERSION}" STREQUAL "both")
	SET(PS_D3D11 TRUE)
	ADD_DEFINITIONS("-DPS_D3D11")
	
	IMPORT_FOLDER(GraphicsD3D11Sources "d3d11")
	IMPORT_FOLDER(GraphicsD3D11ImGuiSources "d3d11/imgui")
	SET(GraphicsAPISources
		${GraphicsAPISources}
		${GraphicsD3D11Sources}
		${GraphicsD3D11ImGuiSources}
	)
ENDIF ()

IF (PS_PS4)
	SET(PS_GNM TRUE)
	ADD_DEFINITIONS("-DPS_GNM")
	
	IMPORT_FOLDER(GraphicsGNMSources "gnm")
	SET(GraphicsAPISources ${GraphicsAPISources} ${GraphicsGNMSources})
ENDIF (PS_PS4)

IMPORT_FOLDER(GraphicsRootSources "")
IMPORT_FOLDER(GraphicsPlatformSources "platform")

SET(GraphicsSources
	${GraphicsRootSources}
	${GraphicsAPISources}
	${GraphicsPlatformSources}
)

ADD_LIBRARY(sulphur-graphics STATIC ${GraphicsSources})
LINK_LIBS(sulphur-graphics EASTL glm)

IF (PS_GNM)
	LINK_LIBS(sulphur-graphics ${PS_PS4_LIBS})
ENDIF (PS_GNM)

IF (PS_D3D11 OR PS_D3D12)
	FIND_PACKAGE(DirectX REQUIRED)
	
	IF (PS_D3D11)
		IF (DirectX_D3D11_FOUND)
			LINK_LIBS(sulphur-graphics ${DirectX_D3D11_LIBRARIES})
		ELSE ()
			MESSAGE(FATAL_ERROR "D3D11 was requested, but not found")
		ENDIF ()
		
		# TARGET_INCLUDE_DIRECTORIES(sulphur-graphics PRIVATE ${DirectX_D3D11_INCLUDE_DIR})
	ENDIF (PS_D3D11)
	
	IF (PS_D3D12)
		IF (DirectX_D3D12_FOUND)
			LINK_LIBS(sulphur-graphics ${DirectX_D3D12_LIBRARIES})
		ELSE ()
			MESSAGE(FATAL_ERROR "D3D12 was requested, but not found")
		ENDIF ()
		
		# TARGET_INCLUDE_DIRECTORIES(sulphur-graphics PRIVATE ${DirectX_D3D12_INCLUDE_DIR})
	ENDIF (PS_D3D12)
	
	# TARGET_INCLUDE_DIRECTORIES(sulphur-graphics PRIVATE ${DirectX_DXGI_INCLUDE_DIR})
ENDIF (PS_D3D11 OR PS_D3D12)