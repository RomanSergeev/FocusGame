#pragma once
#include <memory>
#include <vector>
#include "InputHandler.h"
#include "controller/CameraController.h"
#include "graphics/Shader.h"
#include "shapes/OpenGLShape.h"

class AppController {
    static const int WIDTH = 800;
    static const int HEIGHT = 600;

    GLWindow window;
    InputHandler inputHandler;
    CameraController cameraController;
    Shader shader;

    bool toggledFullscreenLastFrame = false;
    float currentTime;
    float timeDelta;

    std::vector<std::unique_ptr<OpenGLShape>> gameBoard; // TODO move to model, replace setupDefaultBoard with appropriate setup

    AppController();

    void setupDefaultBoard();
public:
    static std::unique_ptr<AppController> create();
    void updateTime();
    void handleInputKey();
    void render();
    bool shouldClose() const { return window.shouldClose(); }
};