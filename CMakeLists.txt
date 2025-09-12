cmake_minimum_required(VERSION 3.20)

# Project name and language settings
project(FocusGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force Clang to use lld.exe instead of lld-link.exe
set(CMAKE_LINKER "E:/Programs/CLang/bin/lld.exe" CACHE STRING "Force use of lld.exe" FORCE)

# Set correct compiler
set(CMAKE_C_COMPILER "E:/Programs/CLang/bin/clang.exe" CACHE STRING "" FORCE)
set(CMAKE_CXX_COMPILER "clang++" CACHE STRING "" FORCE)

# Set proper linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "-fuse-ld=E:/Programs/CLang/bin/ld.lld.exe" CACHE STRING "" FORCE)
set(CMAKE_MODULE_LINKER_FLAGS_INIT "-fuse-ld=E:/Programs/CLang/bin/ld.lld.exe" CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-fuse-ld=E:/Programs/CLang/bin/ld.lld.exe" CACHE STRING "" FORCE)

# Override CMake's default linking behavior
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> -o <TARGET> <OBJECTS> ${CMAKE_EXE_LINKER_FLAGS_INIT} <LINK_LIBRARIES>")
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> -o <TARGET> <OBJECTS> ${CMAKE_EXE_LINKER_FLAGS_INIT} <LINK_LIBRARIES>")

set(CMAKE_BUILD_TYPE Debug)

# NEW iostream issue: explicit paths
include_directories(
    E:/Programs/CMinGW/include
    E:/Programs/CMinGW/x86_64-w64-mingw32/include
    E:/Programs/CMinGW/lib/gcc/x86_64-w64-mingw32/14.2.0/include
    E:/Programs/CMinGW/lib/gcc/x86_64-w64-mingw32/14.2.0/include-fixed
    include/glfw
    include/glew
)

# OpenGL links:
link_directories(
    lib/glfw-mingw-w64
    lib/glew
)

# NEW iostream issue: flags
# this setup is extremely tight on all the library paths. Might move all the settings to clang++.cfg, but keep it here for one commit for reference.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostdinc -isystem E:/Programs/CMinGW/include -isystem E:/Programs/CMinGW/x86_64-w64-mingw32/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdinc++ -isystem E:/Programs/CMinGW/include/c++/14.2.0 -isystem E:/Programs/CMinGW/include/c++/14.2.0/x86_64-w64-mingw32 -isystem E:/Programs/CMinGW/x86_64-w64-mingw32/include")

# Add source files
file(GLOB_RECURSE SRC_FILES "src/*.cpp")
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
)
