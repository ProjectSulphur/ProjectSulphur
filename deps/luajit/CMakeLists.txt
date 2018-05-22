SET(Path ${CMAKE_CURRENT_SOURCE_DIR})

SET(LuaJITIncludeDirs "${Path}/include")

ADD_LIBRARY(luajit INTERFACE)
TARGET_INCLUDE_DIRECTORIES(luajit INTERFACE ${LuaJITIncludeDirs})

IF (PS_WIN32)
	SET(Libs
		buildvm
		lua51
		luajit
		minilua
	)
	
	FOREACH(Lib ${Libs})
		SET(LuaJITDebugLibraries ${LuaJITDebugLibraries} "${Path}/win32/${Lib}d.lib")
	ENDFOREACH ()
	
	FOREACH(Lib ${Libs})
		SET(LuaJITReleaseLibraries ${LuaJITReleaseLibraries} "${Path}/win32/${Lib}.lib")
	ENDFOREACH ()
	
	TARGET_LINK_LIBRARIES(luajit INTERFACE debug "${LuaJITDebugLibraries}")
	TARGET_LINK_LIBRARIES(luajit INTERFACE optimized "${LuaJITReleaseLibraries}")
ELSEIF (PS_PS4)
	SET(LuaJITLibraries "${Path}/ps4/libluajit.a")
	TARGET_LINK_LIBRARIES(luajit INTERFACE ${LuaJITLibraries})
ENDIF (PS_WIN32)
