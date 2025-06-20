#include <iostream>
#include "AppController.h"
#include "shapes/Cuboid.h"
#define EVERY_N_FRAMES_DO(n, code) do { if ((frameCounter % (n)) == 0) code; } while(0)

// these identical callbacks cannot be generalized because of the way GLFW (in C) obtains function pointers
void AppController::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;
    auto* controller = static_cast<AppController*>(glfwGetWindowUserPointer(window));
    if (!controller) return;
    if (action == GLFW_PRESS) controller->cameraController.setRotating(true);
    else if (action == GLFW_RELEASE) controller->cameraController.setRotating(false);
}

void AppController::mousePositionCallback(GLFWwindow* window, double xpos, double ypos) {
    auto* controller = static_cast<AppController*>(glfwGetWindowUserPointer(window));
    int winWidth, winHeight;
    glfwGetFramebufferSize(window, &winWidth, &winHeight);
    if (controller) controller->cameraController.handleMousePosition(winWidth, winHeight, xpos, ypos);
}

void AppController::mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    auto* controller = static_cast<AppController*>(glfwGetWindowUserPointer(window));
    if (controller) controller->cameraController.handleMouseScroll(xOffset, yOffset);
}

void AppController::resizeCallback(GLFWwindow* window, int width, int height) {
    auto* controller = static_cast<AppController*>(glfwGetWindowUserPointer(window));
    glViewport(0, 0, width, height);
    if (controller) controller->cameraController.handleWindowResize(width, height);
}

AppController::AppController() :
    window(WIDTH, HEIGHT, "Focus Game"),
    inputHandler(),
    cameraController(WIDTH, HEIGHT),
    shader(shaderCodeVertices.c_str(), shaderCodeFragments.c_str()) {
        
    GLFWwindow* handle = window.getHandle();
    glfwSetWindowUserPointer(handle, this); // store user pointer
    glfwSetMouseButtonCallback(handle, AppController::mouseButtonCallback);
    glfwSetCursorPosCallback(handle, AppController::mousePositionCallback);
    glfwSetScrollCallback(handle, AppController::mouseScrollCallback);
    glfwSetFramebufferSizeCallback(handle, AppController::resizeCallback);

    shader.use();
    shader.setVec3(ShaderParams::LIGHT_DIR, -0.3f, -0.2f, -0.7f);

    CameraController::CameraSettings settings;
    settings.smoothRotation = false;
    settings.smoothZoom = false;
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

void AppController::TEMPselectBoardIndex(const Ray& ray) {
    float closestDist = FLT_MAX;
    int hitIndex = -1;

    for (int i = 0; i < gameBoard.size(); ++i) {
        float dist;
        if (ray.intersects(gameBoard[i]->getBoundingBox(), dist) /*&& TEMPrayIntersectsShape(ray, gameBoard[i], dist)*/) {
            if (dist < closestDist) {
                closestDist = dist;
                hitIndex = i;
            }
        }
        gameBoard[i]->deselect();
    }

    if (hitIndex >= 0) gameBoard[hitIndex]->select();
}

void AppController::updateTime() {
    ++frameCounter;
    float timeNow = window.getCurrentTime();
    timeDelta = timeNow - currentTime;
    currentTime = timeNow;
}

void AppController::handleInputMouse() {
    Ray ray = cameraController.getMouseRay();
    if (!ray.isActive()) return;

    TEMPselectBoardIndex(ray);
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
    glm::mat4 view = cameraController.getCameraView();
    shader.setMat4(ShaderParams::VIEW, view);
    glm::mat4 projection = cameraController.getProjectionMatrix();
    shader.setMat4(ShaderParams::PROJECTION, projection);

    window.clearBuffer();
    // rotation:
    // shape->setModel(glm::rotate(shape->getBaseModel(), time, glm::vec3(0.3f, 1.0f, 0.0f)));
    for (const auto& shape : gameBoard) {
        shape->setUniforms(shader, currentTime);
        shape->draw();
    }

    window.swapBuffers();
    window.pollEvents();
}