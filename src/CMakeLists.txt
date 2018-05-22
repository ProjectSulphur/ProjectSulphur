# Add definitions
ADD_SUBDIRECTORY("cmake")

IF (PS_PS4)
	ADD_SUBDIRECTORY("ps4")
ENDIF (PS_PS4)

# Include all the project related directories for every target
INCLUDE_DIRECTORIES("\\")

IF (PS_BUILD_ENGINE)
	ADD_SUBDIRECTORY("engine")
	ADD_SUBDIRECTORY("graphics")
	ADD_SUBDIRECTORY("physics")
	ADD_SUBDIRECTORY("tools/networking")
ENDIF (PS_BUILD_ENGINE)


ADD_SUBDIRECTORY("foundation")

IF (PS_BUILD_TEST)
	ADD_SUBDIRECTORY("test")
ENDIF (PS_BUILD_TEST)

IF (PS_PS4)
	SET(PS_BUILD_TOOLS OFF CACHE BOOL "Should the tools projects be built?" FORCE)
ENDIF (PS_PS4)

IF (PS_BUILD_TOOLS)
	# Add the toolchain as seperate projects
	ADD_SUBDIRECTORY("tools")
ENDIF (PS_BUILD_TOOLS)

IF (PS_BUILD_PLAYER)
	ADD_SUBDIRECTORY("player")
ENDIF (PS_BUILD_PLAYER)