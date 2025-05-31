#include <iostream>
#include <sstream>
#include <stdexcept>
#include "GLWindow.h"

void checkGLError(const char* stmt, const char* fname, int line) {
    GLenum err = glGetError();
    while (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error " << err << " at " << fname << ":" << line << " - for " << stmt << std::endl;
        err = glGetError();
    }
}

GLWindow::GLWindow(int width, int height, const std::string& title) {
    // Step 1: Initialize GLFW
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // Step 2: Create a GLFW window
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        throw std::runtime_error("Failed to create window");
        glfwTerminate();
    }
    
    glfwMakeContextCurrent(window); // Set OpenGL context BEFORE initializing GLEW

    glfwSwapInterval(1); // Enable V-Sync - cap the framerate to the monitor current framerate - prevents 100% CPU usage for infinite draw loop

    // Step 3: Initialize GLEW (AFTER context is set)
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
        std::ostringstream oss;
        oss << "GLEW init failed: " << glewGetErrorString(glewStatus);
        glfwDestroyWindow(window);
        glfwTerminate();
        throw std::runtime_error(oss.str());
    }

    glEnable(GL_DEPTH_TEST);

    monitor = glfwGetPrimaryMonitor();
    glfwGetWindowSize(window, &winW, &winH);
    glfwGetWindowPos(window, &winX, &winY);
    glClearColor(0.01f, 0.03f, 0.025f, 1.0f); // Set a dark greenish background
}

GLWindow::~GLWindow() {
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}

void GLWindow::pollEvents() {
    glfwPollEvents();
}

void GLWindow::swapBuffers() {
    glfwSwapBuffers(window);
}

bool GLWindow::shouldClose() const {
    return glfwWindowShouldClose(window);
}

GLFWwindow* GLWindow::getHandle() const {
    return window;
}

bool GLWindow::keyPressed(int keyCode) const {
    return glfwGetKey(window, keyCode) == GLFW_PRESS;
}

void GLWindow::processInput() {
    static bool toggledLastFrame = false;
    
    if ((!keyPressed(GLFW_KEY_ENTER) && !keyPressed(GLFW_KEY_KP_ENTER)) ||
        (!keyPressed(GLFW_KEY_LEFT_ALT) && !keyPressed(GLFW_KEY_RIGHT_ALT))) {
            toggledLastFrame = false;
            return;
        }
    if (toggledLastFrame) return;
    if (isFullscreen) {
        glfwSetWindowMonitor(window, nullptr, winX, winY, winW, winH, 0);
    } else {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwGetWindowPos(window, &winX, &winY);
        glfwGetWindowSize(window, &winW, &winH);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    isFullscreen = !isFullscreen;
    toggledLastFrame = true;
}