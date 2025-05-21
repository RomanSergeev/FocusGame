#pragma once
#include <string>
#include "OpenGLhub.h"
#define GL_CHECK(stmt) do { stmt; checkGLError(#stmt, __FILE__, __LINE__); } while (0)

// Usage: GL_CHECK(glFunctionCall(args...));
void checkGLError(const char* stmt, const char* fname, int line);

class GLWindow {
    static const int DEFAULT_WIDTH = 800;
    static const int DEFAULT_HEIGHT = 600;

    GLFWwindow* window;
    GLFWmonitor* monitor;
    int winX, winY, winW = DEFAULT_WIDTH, winH = DEFAULT_HEIGHT;
    bool isFullscreen = false;
public:
    GLWindow(int width, int height, const std::string& title);
    ~GLWindow();

    void pollEvents();
    void swapBuffers();
    bool shouldClose() const;
    GLFWwindow* getHandle() const;
    bool keyPressed(int keyCode) const;

    void processInput(); // handles fullscreen toggle
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};