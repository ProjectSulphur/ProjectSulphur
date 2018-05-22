IMPORT_FOLDER(BuilderRootSources "" "export.cc" "export.h")
IMPORT_FOLDER(BuilderBaseSources "base")
IMPORT_FOLDER(BuilderPipelinesSources "pipelines")
IMPORT_FOLDER(BuilderSharedSources "shared")
IMPORT_FOLDER(BuilderPlatformSources "platform-specific/win32")

IF (PS_ENABLE_PS4_TOOLS)
        IMPORT_FOLDER(BuilderPS4Sources "platform-specific/ps4")
        SET(BuilderPlatformSources ${BuilderPlatformSources} ${BuilderPS4Sources})
ENDIF (PS_ENABLE_PS4_TOOLS)

SET(BuilderSourcesExec
	${BuilderRootSources}
	${BuilderBaseSources}
	${BuilderPipelinesSources}
        ${BuilderSharedSources}
        ${BuilderPlatformSources}
)

IMPORT_FOLDER(BuilderRootSources "" "main.cc")
IMPORT_FOLDER(BuilderSharedSources "shared")

SET(BuilderSourcesShared
	${BuilderRootSources}
	${BuilderPipelinesSources}
        ${BuilderSharedSources}
        ${BuilderPlatformSources}
)

ADD_EXECUTABLE(sulphur-builder ${BuilderSourcesExec})
ADD_LIBRARY(sulphur-builder-shared SHARED ${BuilderSourcesShared})

FIND_PACKAGE(DirectX REQUIRED)
SET(Targets assimp glslang SPIRV spirv-cross-core spirv-cross-cpp spirv-cross-glsl spirv-cross-hlsl spirv-cross-msl nvtt sulphur-foundation EASTL stb ${DirectX_D3DCOMPILER_LIBRARY})

IF (PS_ENABLE_PS4_TOOLS)
	SET(Targets ${Targets} ps4-tools)
ENDIF (PS_ENABLE_PS4_TOOLS)


LINK_LIBS(sulphur-builder PRIVATE ${Targets})
LINK_LIBS(sulphur-builder-shared PRIVATE ${Targets})