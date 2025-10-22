# Compiler locations
set(CLANG_ROOT "E:/Programs/CLang/bin")
set(MINGW_ROOT "E:/Programs/CMinGW")
set(MINGW_LIBS "${MINGW_ROOT}/lib/gcc/x86_64-w64-mingw32/14.2.0")
set(MINGW_CPP  "${MINGW_ROOT}/include/c++/14.2.0")

# Force Clang to use lld.exe instead of lld-link.exe
set(CMAKE_LINKER "${CLANG_ROOT}/ld.lld.exe" CACHE STRING "Force use of ld.lld.exe" FORCE)