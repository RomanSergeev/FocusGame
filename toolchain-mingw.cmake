# Compiler locations
set(CLANG_ROOT "E:/Programs/CLang/bin")
set(MINGW_ROOT "E:/Programs/CMinGW")
set(MINGW_LIBS "${MINGW_ROOT}/lib/gcc/x86_64-w64-mingw32/14.2.0")
set(MINGW_CPP  "${MINGW_ROOT}/include/c++/14.2.0")

# Force Clang to use lld.exe instead of lld-link.exe
set(CMAKE_LINKER "${CLANG_ROOT}/ld.lld.exe" CACHE STRING "Force use of ld.lld.exe" FORCE)

# Set path to the compiler
set(CMAKE_C_COMPILER "${CLANG_ROOT}/clang.exe" CACHE STRING "C compiler path" FORCE)
set(CMAKE_CXX_COMPILER "${CLANG_ROOT}/clang++.exe" CACHE STRING "C++ compiler path" FORCE)

# NEW iostream issue: flags
# this setup is extremely tight on all the library paths. Might move all the settings to clang++.cfg, but keep it here for one commit for reference.
set(CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS} -nostdinc \
-isystem${MINGW_ROOT}/include \
-isystem${MINGW_ROOT}/x86_64-w64-mingw32/include")

set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS} -nostdinc++ \
-isystem${MINGW_CPP} \
-isystem${MINGW_CPP}/x86_64-w64-mingw32 \
-isystem${MINGW_ROOT}/x86_64-w64-mingw32/include")

# Target triple (force MinGW GNU ABI)
set(CMAKE_C_COMPILER_TARGET "x86_64-w64-mingw32")
set(CMAKE_CXX_COMPILER_TARGET "x86_64-w64-mingw32")

# Set proper linker flags - won't work (will be overridden) without specifying the target compiler mode first
set(CMAKE_EXE_LINKER_FLAGS_INIT "-fuse-ld=${CMAKE_LINKER}" CACHE STRING "" FORCE)
#set(CMAKE_MODULE_LINKER_FLAGS_INIT "-fuse-ld=${CMAKE_LINKER}" CACHE STRING "" FORCE)
#set(CMAKE_SHARED_LINKER_FLAGS_INIT "-fuse-ld=${CMAKE_LINKER}" CACHE STRING "" FORCE)

# Override CMake's default linking behavior
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> -o <TARGET> <OBJECTS> ${CMAKE_EXE_LINKER_FLAGS} <LINK_LIBRARIES>")
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> -o <TARGET> <OBJECTS> ${CMAKE_EXE_LINKER_FLAGS} <LINK_LIBRARIES>")

# debugging messages to check if the compilers are aligned (in GNU, not MSVC mode)
# message(STATUS "CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
# message(STATUS "CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
# message(STATUS "CXX_COMPILER_TARGET: ${CMAKE_CXX_COMPILER_TARGET}")
# message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")