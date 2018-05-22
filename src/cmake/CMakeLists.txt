FUNCTION (ADD_CSPROJ arg1 arg2)

	INCLUDE_EXTERNAL_MSPROJECT(
    ${arg1} "${arg2}"
    TYPE FAE04EC0-301F-11D3-BF4B-00C04F79EFBC
	PLATFORM "Any CPU")
	
ENDFUNCTION(ADD_CSPROJ)

# Imports a folder into the solution
# arg1: The variable to store the found paths in
# arg2: The directory to search in, relative to the current directory
# arg3...n [Optional]: Files to exclude

FUNCTION (IMPORT_FOLDER arg1 arg2)

	IF ("${arg2}" STREQUAL "")
		SET(Path "${CMAKE_CURRENT_SOURCE_DIR}")
	ELSE ()
		SET(Path "${CMAKE_CURRENT_SOURCE_DIR}/${arg2}")
	ENDIF ("${arg2}" STREQUAL "")
	
	FILE(GLOB Sources 
		"${Path}/*.h"
		"${Path}/*.cc"
	)
	
	IF ("${Sources}" STREQUAL "")
		MESSAGE(STATUS "No files found in folder ${Path}")
	ELSE ()
		IF (WIN32)
			IF ("${arg2}" STREQUAL "")
				SET(SourceGroup "\\")
				SET(Prefix "\\")
			ELSE ()
				STRING(REPLACE "/" "\\" Prefix ${arg2})
			ENDIF ("${arg2}" STREQUAL "")
			
			SOURCE_GROUP(${Prefix} FILES ${Sources})
		ENDIF (WIN32)
		
		FOREACH (ToExclude ${ARGN})
			SET(PathExclude "${Path}/${ToExclude}")
			MESSAGE(STATUS "Excluding: ${PathExclude}")
			LIST(REMOVE_ITEM Sources ${PathExclude})
		ENDFOREACH ()
		
		SET(${arg1} ${Sources} PARENT_SCOPE)
	
	ENDIF ("${Sources}" STREQUAL "")
ENDFUNCTION (IMPORT_FOLDER)

# Basically uses IMPORT_FOLDER for "win32" and "ps4" folders based on the platform
# This is relative to the current directory
# arg1: The variable to store the found paths in
# arg2: The directory to look in for the folders
# arg3...n [Optional]: Files to exclude

FUNCTION (IMPORT_PLATFORM_FOLDERS arg1 arg2)
	IF ("${arg2}" STREQUAL "")
		SET(Path "")
	ELSE ()
		SET(Path "${arg2}/")
	ENDIF ("${arg2}" STREQUAL "")

	IF (PS_WIN32)
		IMPORT_FOLDER(Sources "${Path}win32" ${ARGN})
	ELSEIF (PS_PS4)
		IMPORT_FOLDER(Sources "${Path}ps4" ${ARGN})
	ELSE ()
		MESSAGE(FATAL_ERROR "Unknown platform for import")
	ENDIF (PS_WIN32)
	
	SET(${arg1} ${Sources} PARENT_SCOPE)
ENDFUNCTION (IMPORT_PLATFORM_FOLDERS)

# Sets the solution folder for the Win32 platform
# arg1: The target
# arg2: The solution folder 
FUNCTION (SET_SOLUTION_FOLDER arg1 arg2)
	IF (TARGET ${arg1} AND WIN32)
		SET_TARGET_PROPERTIES(${arg1} PROPERTIES FOLDER ${arg2})
	ENDIF (TARGET ${arg1} AND WIN32)
ENDFUNCTION (SET_SOLUTION_FOLDER)

# Sets the solution folder for the Win32 platform for grouped entries
# arg1: The solution folder
# arg2...n: The targets to include in this folder
FUNCTION (SET_SOLUTION_FOLDER_GROUPED arg1)
	FOREACH (Name ${ARGN})
		SET_SOLUTION_FOLDER(${Name} ${arg1})
	ENDFOREACH ()
ENDFUNCTION (SET_SOLUTION_FOLDER_GROUPED)

FUNCTION (LINK_LIBS)
	TARGET_LINK_LIBRARIES(${ARGN})
ENDFUNCTION (LINK_LIBS)

IF (PS_PS4)
	ADD_SUBDIRECTORY("ps4")
ENDIF (PS_PS4)