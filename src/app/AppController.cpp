#include <iostream>
#include "AppController.h"
#include "shapes/Cuboid.h"

AppController::AppController() :
    window(WIDTH, HEIGHT, "Focus Game"),
    inputHandler(),
    cameraController(window),
    shader(shaderCodeVertices.c_str(), shaderCodeFragments.c_str()) {

    shader.use();
    shader.setVec3(ShaderParams::LIGHT_DIR, -0.3f, -0.2f, -0.7f);

    CameraController::CameraSettings settings;
    settings.smoothRotation = true;
    settings.smoothZoom = true;
    settings.invertedHorizontalMouse = true;
    settings.invertedVerticalMouse = true;
    cameraController.updateSettings(std::move(settings));
    cameraController.setZoomLimits(4.0f, 30.0f);

    setupDefaultBoard();
}

std::unique_ptr<AppController> AppController::create() {
    try {
        return std::unique_ptr<AppController>(new AppController());
    }
    catch (const std::exception& e) {
        std::cerr << "Window initialization failed: " << e.what() << std::endl;
        return nullptr;
    }
}

void AppController::setupDefaultBoard() {
    const char CELLS_X = 8;
    const char CELLS_Y = 8;
    const float CUBE_W = 2.0f;
    const float CUBE_D = 0.3f; // ratio
    const float REMOVE = 2 * (CELLS_X / 2 - 1.5) * (CELLS_X / 2 - 1.5) + 2.5;

    gameBoard.clear();
    for (int i = 0; i < CELLS_X; ++i) {
        for (int j = 0; j < CELLS_Y; ++j) {
            const float DISTANCE = (CELLS_X / 2 - 0.5 - i) * (CELLS_X / 2 - 0.5 - i) + (CELLS_Y / 2 - 0.5 - j) * (CELLS_Y / 2 - 0.5 - j);
            if (DISTANCE >= REMOVE) continue;
            std::unique_ptr<OpenGLShape> cell = std::make_unique<Cuboid>(CUBE_W, CUBE_W, CUBE_W * CUBE_D);
            cell->setupBuffer();
            bool even = (i + j) & 1;
            float rgb = even ? 0.2 : 0.9;
            cell->setColor(rgb, rgb, rgb);
            float x = (i - CELLS_X / 2 + 0.5) * CUBE_W;
            float y = (j - CELLS_Y / 2 + 0.5) * CUBE_W;
            cell->position(x, y, 0);
            gameBoard.push_back(std::move(cell));
        }
    }
}

void AppController::updateTime() {
    float timeNow = window.getCurrentTime();
    timeDelta = timeNow - currentTime;
    currentTime = timeNow;
}

void AppController::handleInputKey() {
    auto action = inputHandler.handleKeyPress(window);
    if (action != InputHandler::InputAction::ToggleFullScreen) {
        toggledFullscreenLastFrame = false;
    } else {
        if (toggledFullscreenLastFrame) return;
        toggledFullscreenLastFrame = true;
        window.toggleFullscreenMode();
    }
}

void AppController::render() {
    cameraController.updateView(timeDelta);
    glm::mat4 view = cameraController.getView();
    shader.setMat4(ShaderParams::VIEW, view);
    glm::mat4 projection = cameraController.getProjectionMatrix();
    shader.setMat4(ShaderParams::PROJECTION, projection);

    window.clearBuffer();
    // rotation:
    // shape->setModel(glm::rotate(shape->getBaseModel(), time, glm::vec3(0.3f, 1.0f, 0.0f)));
    for (const auto& shape : gameBoard) {
        shape->setUniforms(shader);
        shape->draw();
    }

    window.swapBuffers();
    window.pollEvents();
}