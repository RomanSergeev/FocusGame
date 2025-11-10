#include <stdexcept>
#include <windows.h>
#include "GLWindow.h"
#include "glfw3.h"
#include "utility/Logger.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

constexpr int IDR_ICON_PNG = 101; // see resources/resources.rc (constants must be equal)

void checkGLError(const char* stmt, const char* fname, int line) {
    GLenum err = glGetError();
    while (err != GL_NO_ERROR) {
        Logger::logs("OpenGL error ", err, " at ", fname, ":", line, " - for ", stmt);
        err = glGetError();
    }
}

void GLWindow::loadWindowIcon() {
    HRSRC hRes = FindResource(nullptr, MAKEINTRESOURCE(IDR_ICON_PNG), "PNG");
    HGLOBAL hData = LoadResource(nullptr, hRes);
    void* pData = LockResource(hData);
    DWORD size = SizeofResource(nullptr, hRes);
    int width, height, channels;
    unsigned char* imageData = stbi_load_from_memory(
        static_cast<unsigned char*>(pData),
        size,
        &width, &height, &channels,
        4 // force RGBA
    );
    GLFWimage image;
    image.width = width;
    image.height = height;
    image.pixels = imageData;

    glfwSetWindowIcon(window, 1, &image);
    stbi_image_free(imageData);
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
    
    loadWindowIcon();

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

    inputStorage.registerKeyGroup("Alt", { GLFW_KEY_LEFT_ALT, GLFW_KEY_RIGHT_ALT } );
    inputStorage.registerKeyGroup("Ctrl", { GLFW_KEY_LEFT_CONTROL, GLFW_KEY_RIGHT_CONTROL } );
    inputStorage.registerKeyGroup("Shift", { GLFW_KEY_LEFT_SHIFT, GLFW_KEY_RIGHT_SHIFT } );
    inputStorage.registerKeyGroup("Enter", { GLFW_KEY_ENTER, GLFW_KEY_KP_ENTER } );
    inputStorage.registerKey(GLFW_KEY_R);
}

GLWindow::~GLWindow() {
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}

void GLWindow::updateInput() {
    inputStorage.update(window);
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

void GLWindow::goFullscreenMode() {
    if (inFullscreen) return;
    inFullscreen = true;
    glfwGetWindowPos(window, &winX, &winY);
    glfwGetWindowSize(window, &winW, &winH);
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    if (exclusiveFullscreen) {
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    } else {
        glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
        glfwSetWindowPos(window, 0, 0);
        glfwSetWindowSize(window, mode->width, mode->height);
    }
}

void GLWindow::goWindowedMode() {
    if (!inFullscreen) return;
    inFullscreen = false;
    if (!exclusiveFullscreen) glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
    glfwSetWindowMonitor(window, nullptr, winX, winY, winW, winH, 0);
}

void GLWindow::toggleFullscreenMode() {
    if (inFullscreen) goWindowedMode();
    else goFullscreenMode();
}

float GLWindow::getCurrentTime() const {
    return glfwGetTime();
}