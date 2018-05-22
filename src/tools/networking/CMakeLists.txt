IMPORT_FOLDER(NetworkingRootSources "")

SET(NetworkingSources
	${NetworkingRootSources}
)

ADD_LIBRARY(sulphur-networking SHARED ${NetworkingSources})

LINK_LIBS(sulphur-networking enet sulphur-foundation)

IF (PS_WIN32)
	LINK_LIBS(sulphur-networking ws2_32.lib winmm.lib)
ELSEIF (PS_PS4)
	LINK_LIBS(sulphur-networking ps4)
ENDIF (PS_WIN32)