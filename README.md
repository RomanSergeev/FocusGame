# Focus Game
This is my pet project I wanted to build for a long time. The original idea is not mine, the game was created by Sid Sackson and published in 1963 (but I remember reading about it being created in 1957). The nature of the game requires it to be rendered in 3D (though it can be done in 2D, but will look clumsy). I want to use this project mostly for my own improvement of developing skills, including: C++ modern standards (C++17 for now), CMake, MVC, networking, testing, git, documenting, cross-platform building.

Made with C++ / OpenGL.

# How To Play?
At game start, several board cells are occupied. Think of checkers there as towers of height 1. A tower can move horizontally/vertically for (not more than its height) cells. When a tower moves onto another tower, it literally gets placed on top of it. Tower's ownership (who can move it) is determined by its top checker. Towers cannot stack up indefinitely - there's a limit (default = 5 checkers). All excess checkers are moved to corresponding trays you can see below the board. If it's enemies' checkers - they stay there til the end. If they're yours - you can use them to place on any free cell or your own tower (not exceeding max height), instead of a regular move. Default limit on reserve placed is 1 checker. A player loses when they cannot make any turn, or place reserve anywhere.

<br>
<br>

## Build notice

OpenGL dependencies: to build properly (with CMake on Windows), download [GLFW](https://www.glfw.org/download.html) and [GLEW](https://glew.sourceforge.net/) binaries, add their library paths as link_directories. Add their header paths as include_directories (not sure if needed).
Make sure you have glew32.dll next to the executable after the build.

<br>

## TODO

- Add view selection support
- Add map files save/load
- Add replay files save/load
- Add user settings sections: game rule defaults (GameRules), camera defaults (CameraSettings), window defaults (GLWindow), key bindings
- ...many more from Backlog.md