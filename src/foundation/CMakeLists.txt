IMPORT_FOLDER(FoundationRootSources "")
IMPORT_FOLDER(FoundationAuxiliarySources "auxiliary")
IMPORT_FOLDER(FoundationIOSources "io")
IMPORT_FOLDER(FoundationMemorySources "memory")
IMPORT_FOLDER(FoundationMemoryAllocatorsSources "memory/allocators")
IMPORT_FOLDER(FoundationApplicationSources "application")
IMPORT_FOLDER(FoundationLoggingSources "logging")
IMPORT_FOLDER(FoundationThreadSources "thread")
IMPORT_FOLDER(FoundationUtilsSources "utils")
IMPORT_FOLDER(FoundationContainerSources "containers")
IMPORT_FOLDER(FoundationJobSources "job")
IMPORT_FOLDER(FoundationPipelineAssetsSources "pipeline-assets")
IMPORT_FOLDER(FoundationMathSources "math")
IMPORT_PLATFORM_FOLDERS(FoundationPlatformSources "")

SET(FoundationSources
	${FoundationRootSources}
	${FoundationAuxiliarySources}
	${FoundationIOSources}
	${FoundationMemorySources}
	${FoundationMemoryAllocatorsSources}
	${FoundationApplicationSources}
	${FoundationLoggingSources}
	${FoundationThreadSources}
	${FoundationUtilsSources}
	${FoundationContainerSources}
	${FoundationJobSources}
	${FoundationPipelineAssetsSources}
	${FoundationPlatformSources}
	${FoundationMathSources}
)

ADD_LIBRARY(sulphur-foundation STATIC ${FoundationSources})

LINK_LIBS(sulphur-foundation PUBLIC EASTL EAStdC glm lz4_static)
