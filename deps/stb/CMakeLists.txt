SET(StbIncludeDir "${CMAKE_CURRENT_SOURCE_DIR}")

ADD_LIBRARY(stb INTERFACE)
TARGET_INCLUDE_DIRECTORIES(stb INTERFACE ${StbIncludeDir})