IF (MSVC)
	SET(Flags
		CMAKE_CXX_FLAGS
		CMAKE_C_FLAGS
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_RELEASE
	)
	
	FOREACH (Flag ${Flags})
		STRING(REGEX REPLACE "/W[01234]" "" ${Flag} "${${Flag}}")
		SET(${Flag} "${${Flag}} /W0")
	ENDFOREACH ()
ENDIF (MSVC)

ADD_SUBDIRECTORY("enet")
TARGET_INCLUDE_DIRECTORIES(enet PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/enet")

SET(EASTL_BUILD_TESTS ON CACHE BOOL "" FORCE)
ADD_SUBDIRECTORY("eastl")

TARGET_INCLUDE_DIRECTORIES(EASTL PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/eastl/include")

IF(PS_BUILD_TOOLS)
	SET(SKIP_GLSLANG_INSTALL TRUE CACHE BOOL "" FORCE)
	ADD_SUBDIRECTORY("glslang")
	TARGET_INCLUDE_DIRECTORIES(glslang PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/glslang/glslang")

	ADD_SUBDIRECTORY("spirvcross")

	SET(AssimpDisabled
	3DS 3D 3MF AC ASE ASSBIN ASSXML B3D BLEND BVH COB COLLADA CSM DXF
	HMP IFC IRRMESH IRR LWO LWS MD2 MD3 MD5 MDC MDL MS3D NDO NFF
	NONFREE_C4D OFF OGRE OPENGEX PLY Q3BSP Q3D RAW SIB SMD STL TERRAGEN XGL X
	AMF MMD X3D)


	FOREACH (ToDisable ${AssimpDisabled})
		SET(ASSIMP_BUILD_${ToDisable}_IMPORTER FALSE CACHE BOOL "Disabled Assimp importer" FORCE)
	ENDFOREACH ()

	SET(BUILD_SHARED_LIBS FALSE CACHE BOOL "Disabled Assimp shared library" FORCE)
	SET(ASSIMP_BUILD_ASSIMP_TOOLS FALSE CACHE BOOL "Disabled Assimp tools project" FORCE)
	SET(ASSIMP_BUILD_TESTS FALSE CACHE BOOL "Disabled Assimp tests" FORCE)
	SET(ASSIMP_INSTALL_PDB FALSE CACHE BOOL "Disabled Assimp PDB" FORCE)

	ADD_SUBDIRECTORY("assimp-4.1.0")
	TARGET_INCLUDE_DIRECTORIES(assimp PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/assimp-4.1.0/include")
	TARGET_INCLUDE_DIRECTORIES(assimp PUBLIC "${PROJECT_BINARY_DIR}/deps/assimp-4.1.0/include")

	SET(BUILD_TESTING FALSE CACHE BOOL "" FORCE)
	ADD_SUBDIRECTORY("nvtt")
	TARGET_INCLUDE_DIRECTORIES(nvtt PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/nvtt/src")
	TARGET_INCLUDE_DIRECTORIES(nvtt PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/nvtt/extern/poshlib")
	TARGET_INCLUDE_DIRECTORIES(nvtt PUBLIC "${PROJECT_BINARY_DIR}/deps/nvtt/src")
ENDIF (PS_BUILD_TOOLS)

SET(GLM_INSTALL_ENABLE OFF CACHE BOOL "Enable GLM install target")
ADD_SUBDIRECTORY("glm")

IF (NOT PS_PS4)
	SET(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "" FORCE)
ENDIF (NOT PS_PS4)

ADD_SUBDIRECTORY("stb")

IF (PS_BUILD_ENGINE OR PS_BUILD_TOOLS)
    ADD_SUBDIRECTORY("luajit")
	ADD_SUBDIRECTORY("fmod")
	ADD_SUBDIRECTORY("lz4")
	TARGET_INCLUDE_DIRECTORIES(lz4_static PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lz4/lib")
	IF (PS_PHYSX)
		MESSAGE(STATUS "Adding PhysX to the project")
		ADD_SUBDIRECTORY("physx")
	ELSEIF (PS_BULLET)
		MESSAGE(STATUS "Adding bullet3 to the project")
		SET(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
		SET(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
		SET(BUILD_DOCS OFF CACHE BOOL "" FORCE)
		SET(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "" FORCE)
		SET(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
		SET(BUILD_PYBULLET OFF CACHE BOOL "" FORCE)
		SET(BUILD_TESTING OFF CACHE BOOL "" FORCE)
		SET(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
		ADD_SUBDIRECTORY("bullet3")
	ENDIF (PS_PHYSX)
ENDIF ()
