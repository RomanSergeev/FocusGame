# Focus Game

Added OpenGL dependencies. To build properly (with CMake on Windows), download [GLFW](https://www.glfw.org/download.html) and [GLEW](https://glew.sourceforge.net/) binaries, add their library paths as link_directories. Add their header paths as include_directories (not sure if needed).
Make sure you have glew32.dll next to the executable after the build.
So far, nothing is drawn, it's just a black screen window. This is another increment of a working app.

## TODO

- Move GLWindowFunctions into GLWindow class, add fullscreen, resize
- Move Shader into a class, add custom colors/params to individual shapes
- Reconfigure clang++.cfg to lighten CMakeLists.txt
- Potentially remove some include_directories
- Add basic board drawing