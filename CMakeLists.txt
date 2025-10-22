cmake_minimum_required(VERSION 3.20)
project(FocusGame LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set correct compiler
set(CMAKE_C_COMPILER "${CLANG_ROOT}/clang.exe" CACHE STRING "C compiler path" FORCE)
set(CMAKE_CXX_COMPILER "${CLANG_ROOT}/clang++.exe" CACHE STRING "C++ compiler path" FORCE)

# NEW iostream issue: flags
# this setup is extremely tight on all the library paths. Might move all the settings to clang++.cfg, but keep it here for one commit for reference.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostdinc -isystem${MINGW_ROOT}/include -isystem${MINGW_ROOT}/x86_64-w64-mingw32/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdinc++ -isystem${MINGW_CPP} -isystem${MINGW_CPP}/x86_64-w64-mingw32 -isystem${MINGW_ROOT}/x86_64-w64-mingw32/include")

# Set proper linker flags
set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=${CMAKE_LINKER}" CACHE STRING "" FORCE)
set(CMAKE_MODULE_LINKER_FLAGS "-fuse-ld=${CMAKE_LINKER}" CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS "-fuse-ld=${CMAKE_LINKER}" CACHE STRING "" FORCE)

# Override CMake's default linking behavior
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> -o <TARGET> <OBJECTS> ${CMAKE_EXE_LINKER_FLAGS} <LINK_LIBRARIES>")
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> -o <TARGET> <OBJECTS> ${CMAKE_EXE_LINKER_FLAGS} <LINK_LIBRARIES>")

set(CMAKE_BUILD_TYPE Debug)

# NEW iostream issue: explicit paths
include_directories(
    ${MINGW_ROOT}/include
    ${MINGW_ROOT}/x86_64-w64-mingw32/include
    ${MINGW_LIBS}/include
    ${MINGW_LIBS}/include-fixed
    include/glfw
    include/glew
)

# inih extension for reading ini files:
add_library(inih
    include/ini/ini.c
    include/ini/INIReader.cpp
)
target_include_directories(inih PUBLIC include/ini)

# OpenGL links:
link_directories(
    lib/glfw-mingw-w64
    lib/glew
)

# Add source files
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")
add_executable(FocusGame ${SRC_FILES})

# Change created executable location
set_target_properties(FocusGame PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../app"
)

# To avoid warnings and usage of relative paths (../) - warnings still remain
target_include_directories(FocusGame
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(
    FocusGame
    glfw3
    glew32
    opengl32
    inih
)
