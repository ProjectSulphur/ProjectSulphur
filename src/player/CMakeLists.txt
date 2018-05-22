IMPORT_FOLDER(PlayerRootSources "")

SET(PlayerSources
	${PlayerRootSources}
)

ADD_EXECUTABLE(sulphur-player ${PlayerSources})
LINK_LIBS(sulphur-player sulphur-foundation sulphur-engine sulphur-networking sulphur-graphics)