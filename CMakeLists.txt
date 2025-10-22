cmake_minimum_required(VERSION 3.20)
project(FocusGame LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

include_directories(
    include/glfw
    include/glew
)

# OpenGL links:
link_directories(
    lib/glfw-mingw-w64
    lib/glew
)

# inih extension for reading ini files:
add_library(inih
    include/ini/ini.c
    include/ini/INIReader.cpp
)
target_include_directories(inih PUBLIC include/ini)

# Add source files recursively - still need to clean rebuild if any new cpp are added
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

target_link_libraries(FocusGame
    glfw3
    glew32
    opengl32
    inih
)
