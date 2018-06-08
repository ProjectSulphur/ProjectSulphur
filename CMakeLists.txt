# Initial setup values
CMAKE_MINIMUM_REQUIRED(VERSION 3.7.2)
PROJECT("project-sulphur")
CMAKE_POLICY(SET CMP0054 NEW)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/src/cmake/")

# Set the output directory to be the same for all projects, to properly copy shared libraries to where the executable is
SET(PlatformPath "Win64")

IF (PS_PS4)
	SET(PlatformPath "ORBIS")
ENDIF (PS_PS4)

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${PlatformPath})
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${PlatformPath})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PlatformPath})

SET(PS_BUILD_ENGINE ON CACHE BOOL "Should the engine projects be built?")
SET(PS_BUILD_TEST ON CACHE BOOL "Should the test projects be built?")
SET(PS_BUILD_TOOLS ON CACHE BOOL "Should the tools projects be built?")
SET(PS_BUILD_PLAYER ON CACHE BOOL "Should the player be built?")
SET(PS_BUILD_CSHARP OFF CACHE BOOL "Should the C# projects be built?")
SET(PS_ENABLE_PS4_TOOLS OFF CACHE BOOL "")
SET(PS_EDITOR OFF CACHE BOOL "Should the engine be built as an editor?")

SET(PS_USER "test" CACHE STRING "Target test projects for which user?")
SET(UserList angelo benjamin daniel hilze jelle joren kenneth maarten raymi riko rodi stan test timo yana)
SET_PROPERTY(CACHE PS_USER PROPERTY STRINGS all ${UserList})

# Set physics implementation
SET(PS_PHYSICS "physx" CACHE STRING "What physics SDK to use?")
SET_PROPERTY(CACHE PS_PHYSICS PROPERTY STRINGS "physx" "bullet" "sulphur")

IF ("${PS_PHYSICS}" STREQUAL "bullet")
	SET(PS_BULLET TRUE)
	ADD_DEFINITIONS("-DPS_BULLET")
ELSEIF ("${PS_PHYSICS}" STREQUAL "physx")
	SET(PS_PHYSX TRUE)
	ADD_DEFINITIONS("-DPS_PHYSX")
ELSEIF ("${PS_PHYSICS}" STREQUAL "sulphur")
	SET(PS_SULPHURPHYSICS TRUE)
	ADD_DEFINITIONS("-DPS_SULPHURPHYSICS")
ELSE ()
	MESSAGE(FATAL_ERROR "A physics SDK needs to be defined.")
ENDIF()

# Pick the appropriate platform
IF (NOT PS_PS4)
	IF (WIN32)
		MESSAGE(STATUS "Generating a Win32 build")
		SET(PS_WIN32 ON)
		ADD_DEFINITIONS("-DPS_WIN32")
	ENDIF (WIN32)
ELSE ()
	MESSAGE(STATUS "Generating a PlayStation 4 build")
	ADD_DEFINITIONS("-DPS_PS4")
ENDIF (NOT PS_PS4)

IF (PS_EDITOR)
	ADD_DEFINITIONS("-DPS_EDITOR")
ENDIF (PS_EDITOR)

# Enable project folder structure for Visual Studio
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
SET_PROPERTY(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake-targets")

IF (MSVC)
  SET(CMAKE_CXX_FLAGS_DEBUG "/EHsc /RTC1 /MTd /ZI /Od /D_DEBUG")
  SET(CMAKE_CXX_FLAGS_RELEASE "/EHsc /MT /O2 /DNDEBUG")
  
  STRING(REGEX REPLACE "/MDd" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
  STRING(REGEX REPLACE "/MD" "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
  
  SET (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
  SET (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")

  SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG /NODEFAULTLIB:\"MSVCRTD\"")
  SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/DEBUG /NODEFAULTLIB:\"MSVCRTD\"")
  SET(CMAKE_STATIC_LINKER_FLAGS_DEBUG "/NODEFAULTLIB:\"MSVCRTD\"")

  SET(CMAKE_EXE_LINKER_FLAGS "/INCREMENTAL")
  SET(CMAKE_SHARED_LINKER_FLAGS "")
  SET(CMAKE_STATIC_LINKER_FLAGS "")
  
  SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "/NODEFAULTLIB:\"MSVCRT\"")
  SET(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/NODEFAULTLIB:\"MSVCRT\"")
  SET(CMAKE_STATIC_LINKER_FLAGS_RELEASE "/NODEFAULTLIB:\"MSVCRT\"")

  STRING(REGEX REPLACE "/W[01234]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX /W4 /wd4201 /wd4464 /wd4505 /MP")
  
  STRING(REGEX REPLACE "/W[01234]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /WX /W4 /wd4201 /wd4464 /wd4505 /MP")
ENDIF ()

# Add dependecies
ADD_SUBDIRECTORY("deps")

# Add source
ADD_SUBDIRECTORY("src")

# Add C# projects
IF (PS_WIN32 AND PS_BUILD_CSHARP)
	ADD_CSPROJ(sulphur-generator "${PROJECT_SOURCE_DIR}/external/sulphur-generator/sulphur-generator/sulphur-generator.csproj")
	
	IF (PS_EDITOR)
		ADD_CSPROJ(sulphur-editor "${PROJECT_SOURCE_DIR}/external/sulphur-editor/sulphur-editor/sulphur-editor.csproj")
		ADD_DEPENDENCIES(sulphur-editor sulphur-builder-shared)
		ADD_DEPENDENCIES(sulphur-editor sulphur-networking)
	ENDIF (PS_EDITOR)
	
	SET_SOLUTION_FOLDER_GROUPED("csharp-tools"
		sulphur-generator
		sulphur-editor
	)
ENDIF ()

# Create the solution folders
SET_SOLUTION_FOLDER_GROUPED("project-sulphur" 
	sulphur-foundation 
	sulphur-engine 
	sulphur-graphics
	sulphur-physics
	sulphur-player
)
	
SET_SOLUTION_FOLDER_GROUPED("project-sulphur/tools"
	sulphur-networking
	sulphur-builder
	sulphur-builder-shared
)

SET_SOLUTION_FOLDER(enet "deps")

SET_SOLUTION_FOLDER_GROUPED("deps/glm" glm_dummy uninstall)

SET_SOLUTION_FOLDER(glfw "deps/glfw")

SET_SOLUTION_FOLDER_GROUPED("deps/eastl" 
	EASTL 
	EABase_ide
	EAAssert_ide
	EAMain_ide
	EAStdC
	EASTLTest
	EATest
	EAThread_ide)

SET_SOLUTION_FOLDER_GROUPED("deps/bullet3" Bullet2FileLoader 
	Bullet3Collision Bullet3Common Bullet3Dynamics
	Bullet3Geometry Bullet3OpenCL_clew BulletCollision
	BulletDynamics BulletInverseDynamics BulletSoftBody
	LinearMath
)

SET_SOLUTION_FOLDER(lz4_static "deps/lz4")

IF (PS_BUILD_TOOLS)
	SET_SOLUTION_FOLDER_GROUPED("deps/assimp"
		assimp 
		UpdateAssimpLibsDebugSymbolsAndDLLs
		zlibstatic
		IrrXML
	)

	SET_SOLUTION_FOLDER_GROUPED("deps/glslang"
		glslang glslang-default-resource-limits OGLCompiler OSDependent
		SPIRV SPVRemapper HLSL spirv-remap glslangValidator Continuous Experimental Nightly
		NightlyMemoryCheck
	)

	SET_SOLUTION_FOLDER_GROUPED("deps/spirvcross"
		spirv-cross spirv-cross-core spirv-cross-cpp spirv-cross-glsl
		spirv-cross-hlsl spirv-cross-msl
	)
	
	SET_SOLUTION_FOLDER_GROUPED("deps/nvtt"
		bc6h bc7 butteraugli cubemaptest EtcLib filtertest imperativeapi nvassemble nvcompress nvcore
		nvddsinfo nvdecompress nv-gnome-thumbnailer nvhdrtest nvimage nvimgdiff nvmath nvtestsuite nvthread
		nvtt nvzoom posh process_alpha_map rg_etc1 squish
	)
ENDIF (PS_BUILD_TOOLS)
