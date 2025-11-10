#pragma once
#include <memory>
#include "controller/CameraController.h"
#include "controller/GameSession.h"
#include "InputHandler.h"
#include "shapes/Line.h"
#include "utility/Utils.h"
#include "view/GLWindow.h"

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
    std::unique_ptr<GameSession> gameSession;
    AppKey key;

    float currentTime;
    float timeDelta;
    float TEMPhitDistance = 0;
    int frameCounter = 0;
    bool drawCameraRay = true;
    Line rayLine;

    AppController();

    void registerCallbacks();
    void updateRayLine();
public:
    static std::unique_ptr<AppController> create();
    void updateTime();
    void handleInputMouse();
    void handleInputKey();
    void render();
    bool shouldClose() const { return window.shouldClose(); }
};