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
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CCW);

    monitor = glfwGetPrimaryMonitor();
    glfwGetWindowSize(window, &winW, &winH);
    glfwGetWindowPos(window, &winX, &winY);
    glClearColor(0.01f, 0.03f, 0.025f, 1.0f); // Set a dark greenish background
    glLineWidth(1.0f); // 1 through 31 is available
}

GLWindow::~GLWindow() {
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}

void GLWindow::clearBuffer() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

bool GLWindow::isPressedAlt() const {
    return isPressedKey(GLFW_KEY_LEFT_ALT) || isPressedKey(GLFW_KEY_RIGHT_ALT);
}

bool GLWindow::isPressedCtrl() const {
    return isPressedKey(GLFW_KEY_LEFT_CONTROL) || isPressedKey(GLFW_KEY_RIGHT_CONTROL);
}

bool GLWindow::isPressedShift() const {
    return isPressedKey(GLFW_KEY_LEFT_SHIFT) || isPressedKey(GLFW_KEY_RIGHT_SHIFT);
}

bool GLWindow::isPressedEnter() const {
    return isPressedKey(GLFW_KEY_ENTER) || isPressedKey(GLFW_KEY_KP_ENTER);
}

bool GLWindow::isPressedKey(int keyCode) const {
    return glfwGetKey(window, keyCode) == GLFW_PRESS;
}

void GLWindow::goFullscreenMode() {
    if (inFullscreen) return;
    inFullscreen = true;
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwGetWindowPos(window, &winX, &winY);
    glfwGetWindowSize(window, &winW, &winH);
    glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
}

void GLWindow::goWindowedMode() {
    if (!inFullscreen) return;
    inFullscreen = false;
    glfwSetWindowMonitor(window, nullptr, winX, winY, winW, winH, 0);
}

void GLWindow::toggleFullscreenMode() {
    if (inFullscreen) goWindowedMode();
    else goFullscreenMode();
}

float GLWindow::getCurrentTime() const {
    return glfwGetTime();
}