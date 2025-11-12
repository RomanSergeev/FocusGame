cmake_minimum_required(VERSION 3.20)
project(FocusGame LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
# set(CMAKE_CXX_LINKER_FLAGS_RELEASE "-s")

# Bake MinGW necessary dlls (libstdc++, libgcc_s_seh, libwinpthread) into the executable
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-static -static-libgcc -static-libstdc++")
# Bake Threads::Threads later into the target specifically (may fail on previous step)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

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

if (WIN32)
    set(APP_ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.rc")
endif()

# Add source files recursively - still need to clean rebuild if any new cpp are added
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")
add_executable(FocusGame
    ${SRC_FILES}
    ${APP_ICON_RESOURCE}
)

if (WIN32)
    option(SHOW_CONSOLE "Show console window at startup" OFF)
    if(NOT SHOW_CONSOLE)
        set_target_properties(FocusGame PROPERTIES WIN32_EXECUTABLE TRUE)
    endif()
endif()

# Change created executable location
set_target_properties(FocusGame PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/app"
)

# To avoid warnings and usage of relative paths (../)
target_include_directories(FocusGame
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
)

# Reduce the release executable size by symbol stripping (-s)
target_link_options(FocusGame PRIVATE
    $<$<CONFIG:Release>:-s>
)

target_link_libraries(FocusGame
    Threads::Threads
    glfw3
    glew32
    opengl32
    inih
)
