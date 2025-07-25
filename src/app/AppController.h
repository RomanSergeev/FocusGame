#pragma once
#include <memory>
#include <vector>
#include "InputHandler.h"
#include "controller/CameraController.h"
#include "shapes/Cylinder.h"
#include "shapes/Line.h"

class AppController {
    static const int WIDTH = 800;
    static const int HEIGHT = 600;

    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void mousePositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
    static void resizeCallback(GLFWwindow* window, int width, int height);
    static std::vector<Shader> createShaders();

    GLWindow window;
    InputHandler inputHandler;
    CameraController cameraController;
    ShaderCarousel shaders;

    bool toggledFullscreenLastFrame = false;
    float currentTime;
    float timeDelta;
    float TEMPhitDistance = 0;
    int frameCounter = 0;
    bool drawCameraRay = true;
    Line rayLine;
    Cylinder TEMPcylinder;

    std::vector<std::unique_ptr<OpenGLShape>> gameBoard; // TODO move to model, replace setupDefaultBoard with appropriate setup

    AppController();

    void registerCallbacks();
    void setupDefaultBoard();
    void TEMPselectBoardIndex(const Ray& ray);
    void updateRayLine();
public:
    static std::unique_ptr<AppController> create();
    void updateTime();
    void handleInputMouse();
    void handleInputKey();
    void render();
    bool shouldClose() const { return window.shouldClose(); }
};