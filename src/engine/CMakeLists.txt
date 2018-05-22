IMPORT_FOLDER(EngineRootSources "")
IMPORT_FOLDER(EngineApplicationSources "application")
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
IMPORT_FOLDER(EngineScriptingWrapperSources "scripting/wrappers")
IMPORT_FOLDER(EngineScriptableValuesSources "scripting/scriptable_values")
IMPORT_FOLDER(EngineSystemSources "systems")
IMPORT_FOLDER(EngineComponentSources "systems/components")
IMPORT_FOLDER(EngineUtilitiesSources "utilities")
IMPORT_PLATFORM_FOLDERS(EnginePlatformSources "")

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
	${EnginePhysicsSources}
	${EngineScriptingSources}
	${EngineScriptingWrapperSources}
	${EngineScriptableValuesSources}
	${EngineSystemSources}
	${EngineComponentSources}
	${EngineUtilitiesSources}
	${EnginePlatformSources}
)

ADD_LIBRARY(sulphur-engine STATIC ${EngineSources})
LINK_LIBS(sulphur-engine EASTL glm luajit sulphur-physics)

IF (PS_WIN32)
	LINK_LIBS(sulphur-engine glfw)
ENDIF (PS_WIN32)

IF (PS_PS4)
	LINK_LIBS(sulphur-engine ps4)
ENDIF (PS_PS4)
