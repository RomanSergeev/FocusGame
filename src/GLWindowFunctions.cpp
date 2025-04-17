#include <iostream>
#include "GLWindowFunctions.h"

void checkGLError(const char* stmt, const char* fname, int line) {
    GLenum err = glGetError();
    while (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error " << err << " at " << fname << ":" << line << " - for " << stmt << std::endl;
        err = glGetError();
    }
}

int initWindow(GLFWwindow* &window, int width, int height, const char* title) {
    // Step 1: Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Step 2: Create a GLFW window
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window); // Set OpenGL context BEFORE initializing GLEW

    glfwSwapInterval(1); // Enable V-Sync - cap the framerate to the monitor current framerate - prevents 100% CPU usage for infinite draw loop

    // Step 3: Initialize GLEW (AFTER context is set)
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
        std::cerr << "GLEW init failed: " << glewGetErrorString(glewStatus) << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set a dark greenish background
    return 0;
}