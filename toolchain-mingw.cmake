# Removed CMAKE_EXE_LINKER_FLAGS_INIT, CMAKE_<CONFIG>_LINKER_FLAGS_INIT "-fuse-ld=fullpath/ld.lld.exe"
# clang++.cfg keeps path to the correct linker (I had to force use of ld.lld.exe)
# wrong target compiler mode treats linker flags differently
# Removed filesystem-specific MinGW standard library paths - now in clang++.cfg

# Make sure CLang/bin is in your PATH
set(CMAKE_C_COMPILER "clang.exe" CACHE FILEPATH "C compiler path" FORCE)
set(CMAKE_CXX_COMPILER "clang++.exe" CACHE FILEPATH "C++ compiler path" FORCE)

# Target triple (force MinGW GNU ABI (Application Binary Interface))
set(CMAKE_C_COMPILER_TARGET "x86_64-w64-mingw32")
set(CMAKE_CXX_COMPILER_TARGET "x86_64-w64-mingw32")

# Override CMake's default linking behavior
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> -o <TARGET> <OBJECTS> ${CMAKE_EXE_LINKER_FLAGS} <LINK_LIBRARIES>")
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> -o <TARGET> <OBJECTS> ${CMAKE_EXE_LINKER_FLAGS} <LINK_LIBRARIES>")

# debugging messages to check if the compilers are aligned (in GNU, not MSVC mode)
# message(STATUS "CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
# message(STATUS "CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
# message(STATUS "CXX_COMPILER_TARGET: ${CMAKE_CXX_COMPILER_TARGET}")
# message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")