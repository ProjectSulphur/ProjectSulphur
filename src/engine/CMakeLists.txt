IMPORT_FOLDER(EngineRootSources "")
IMPORT_FOLDER(EngineAssetSources "assets")
IMPORT_FOLDER(EngineAudioSources "audio")
IMPORT_FOLDER(EngineCoreSources "core")
IMPORT_FOLDER(EngineComponentSources "component")
IMPORT_FOLDER(EngineGraphicsSources "graphics")
IMPORT_FOLDER(EngineImGuiSources "graphics/imgui")
IMPORT_FOLDER(EngineInputSources "input")
IMPORT_FOLDER(EngineNetworkingSources "networking")
IMPORT_FOLDER(EnginePhysicsSources "physics")
IMPORT_FOLDER(EngineScriptingSources "scripting")
IMPORT_FOLDER(EngineScriptableValuesSources "scripting/scriptable_values")
IMPORT_FOLDER(EngineSystemSources "systems")
IMPORT_FOLDER(EngineComponentSources "systems/components")
IMPORT_FOLDER(EngineRewinderSources "rewinder")
IMPORT_FOLDER(EngineRewinderSystemsSources "rewinder/systems")
IMPORT_FOLDER(EngineUtilitiesSources "utilities")
IMPORT_PLATFORM_FOLDERS(EnginePlatformSources "")

IF (PS_EDITOR)
  IMPORT_FOLDER(EngineApplicationSources "application" "engine_application.cc" "engine_application.h")
  IMPORT_FOLDER(EngineEditorApplicationSources "application/editor")
  IMPORT_FOLDER(EngineNetworkingSources "networking" "engine_messaging_system.cc" "engine_messaging_system.h")
  IMPORT_FOLDER(EngineCoreEditorSources "core/editor")
  IMPORT_FOLDER(EngineSystemEditorSources "systems/editor")
  IMPORT_FOLDER(EngineNetworkingForEditorSources "networking/editor")
  IMPORT_FOLDER(EngineEditorListenerSources "networking/editor/listeners/")
ELSE ()
  IMPORT_FOLDER(EngineApplicationSources "application" "editor_application.cc" "editor_application.h")
  IMPORT_FOLDER(EngineNetworkingSources "networking")
ENDIF ()



SET(EngineSources
	${EngineRootSources}
	${EngineApplicationSources}
	${EngineAssetSources}
	${EngineAudioSources}
	${EngineCoreSources}
	${EngineGraphicsSources}
	${EngineImGuiSources}
	${EngineInputSources}
	${EngineNetworkingSources}
	${EngineNetworkingForEditorSources}
	${EnginePhysicsSources}
	${EngineScriptingSources}
	${EngineScriptableValuesSources}
	${EngineSystemSources}
	${EngineComponentSources}
	${EngineRewinderSources}
	${EngineRewinderSystemsSources}
	${EngineUtilitiesSources}
	${EnginePlatformSources}
	${EngineCoreEditorSources}
	${EngineSystemEditorSources}
	${EngineEditorListenerSources}
	${EngineEditorApplicationSources}
)



ADD_SUBDIRECTORY("lua")

FOREACH (Relative ${LUA_CLASSES})
	IF(${Relative} MATCHES "\\.h")
		SET(InputFile "${CMAKE_CURRENT_SOURCE_DIR}/${Relative}")
		
		MESSAGE(STATUS "Adding custom compilation step for ${InputFile}")
		
		STRING(REGEX REPLACE ".*\\/" "" RelativePath ${InputFile})
		STRING(REGEX REPLACE "\\.h" "" NoExtension ${RelativePath})
		
		SET(OutputFile "${CMAKE_CURRENT_BINARY_DIR}/lua-classes/${NoExtension}.lua.cc")
		
		SET(WorkingDir "${PROJECT_SOURCE_DIR}/src/cmake/lua")
		
		ADD_CUSTOM_COMMAND(
			OUTPUT ${OutputFile}
			COMMAND "${WorkingDir}/lua-parser.exe" -i "${InputFile}" -o "${OutputFile}"
			DEPENDS ${InputFile}
			MAIN_DEPENDENCY ${InputFile}
			WORKING_DIRECTORY "${WorkingDir}"
			COMMENT "Generating Lua bindings for ${InputFile}"
			VERBATIM
		)
	ENDIF ()
ENDFOREACH ()

ADD_LIBRARY(sulphur-engine STATIC ${EngineSources})

LINK_LIBS(sulphur-engine EASTL glm luajit fmod sulphur-physics)

IF (PS_WIN32)
  LINK_LIBS(sulphur-engine Xinput)
ENDIF (PS_WIN32)

TARGET_INCLUDE_DIRECTORIES(sulphur-engine PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

IF (PS_PS4)
	LINK_LIBS(sulphur-engine ps4)
ENDIF (PS_PS4)
