#pragma once
#include <string>
#include "glew/glew.h"
#include "glfw/glfw3.h"
#include "utility/InputStorage.h"
#define GL_CHECK(stmt) do { stmt; checkGLError(#stmt, __FILE__, __LINE__); } while (0)

// Usage: GL_CHECK(glFunctionCall(args...));
void checkGLError(const char* stmt, const char* fname, int line);

class GLWindow {
    static const int DEFAULT_WIDTH = 800;
    static const int DEFAULT_HEIGHT = 600;

    GLFWwindow* window;
    GLFWmonitor* monitor;
    InputStorage inputStorage;
    int winX, winY, winW = DEFAULT_WIDTH, winH = DEFAULT_HEIGHT;
    bool inFullscreen = false;
    
    void loadWindowIcon();
public:
    GLWindow(int width, int height, const std::string& title);
    ~GLWindow();
    
    const InputStorage& getInputStorage() const { return inputStorage; }

    void updateInput();
    void clearBuffer();
    void pollEvents();
    void swapBuffers();
    bool shouldClose() const;
    GLFWwindow* getHandle() const;
    bool isFullscreen() const { return inFullscreen; }
    int getWindowWidth() const { return winW; }
    int getWindowHeight() const { return winH; }
    void goFullscreenMode();
    void goWindowedMode();
    void toggleFullscreenMode();
    float getCurrentTime() const;
};