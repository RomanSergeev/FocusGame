#include <iostream>
#include "AppController.h"
#include "glm/ext/matrix_transform.hpp"
#include "shapes/Cuboid.h"
#include "view/GLWindow.h"
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
    if (!controller) return;
    int winWidth, winHeight;
    glfwGetFramebufferSize(window, &winWidth, &winHeight);
    controller->cameraController.handleMousePosition(winWidth, winHeight, xpos, ypos);
    controller->cameraController.updateRayFromCursor(winWidth, winHeight);
}

void AppController::mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    auto* controller = static_cast<AppController*>(glfwGetWindowUserPointer(window));
    if (!controller) return;
    int winWidth, winHeight;
    glfwGetFramebufferSize(window, &winWidth, &winHeight);
    controller->cameraController.handleMouseScroll(xOffset, yOffset);
    controller->cameraController.updateRayFromCursor(winWidth, winHeight);
}

void AppController::resizeCallback(GLFWwindow* window, int width, int height) {
    auto* controller = static_cast<AppController*>(glfwGetWindowUserPointer(window));
    glViewport(0, 0, width, height);
    if (controller) controller->cameraController.handleWindowResize(width, height);
}

std::vector<Shader> AppController::createShaders() {
    std::vector<Shader> result;
    result.emplace_back(shaderCodeVertices.c_str(), shaderCodeFragments.c_str()); // shader3D
    result.emplace_back(shaderCodeVertices2D.c_str(), shaderCodeFragments2D.c_str()); // shader2D
    return result;
}

AppController::AppController() :
    window(WIDTH, HEIGHT, "Focus Game"),
    inputHandler(),
    cameraController(WIDTH, HEIGHT),
    shaders(createShaders()),
    rayLine(SPACE_ORIGIN, SPACE_ORIGIN),
    TEMPcylinder(1, 1, 1, 32, true) {

    registerCallbacks();

    const Shader& shader = shaders.get();
    shader.setVec3(ShaderParams::LIGHT_DIR, -0.3f, -0.2f, -0.7f);

    //TEMPcylinder.translate(0, 0, 3);
    TEMPcylinder.setColor(0.2, 0.2, 0.2);
    rayLine.setColor(1.0f, 0.0f, 0.0f);

    CameraController::CameraSettings settings;
    settings.smoothRotation = true;
    settings.smoothZoom = true;
    settings.invertedHorizontalMouse = true;
    settings.invertedVerticalMouse = true;
    cameraController.updateSettings(std::move(settings));
    cameraController.setZoomLimits(4.0f, 30.0f);

    setupDefaultBoard();
}

void AppController::registerCallbacks() {
    GLFWwindow* handle = window.getHandle();
    glfwSetWindowUserPointer(handle, this); // store user pointer
    glfwSetMouseButtonCallback(handle, AppController::mouseButtonCallback);
    glfwSetCursorPosCallback(handle, AppController::mousePositionCallback);
    glfwSetScrollCallback(handle, AppController::mouseScrollCallback);
    glfwSetFramebufferSizeCallback(handle, AppController::resizeCallback);
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

bool removeCondition(char sizeX, char sizeY, int i, int j) {
    //return i != 4 || j != 4;
    float distX = sizeX / 2.0 - 0.5 - i;
    float distY = sizeY / 2.0 - 0.5 - j;
    float distance = distX*distX + distY*distY;
    float removeDist = 2 * (sizeX / 2.0 - 1.5) * (sizeX / 2.0 - 1.5) + 2.5;
    return distance >= removeDist;
}

void AppController::setupDefaultBoard() {
    const char CELLS_X = 8;
    const char CELLS_Y = 8;
    const float CUBE_W = 2.0f;
    const float CUBE_D = 0.3f; // ratio

    gameBoard.clear();
    for (int i = 0; i < CELLS_X; ++i) {
        for (int j = 0; j < CELLS_Y; ++j) {
            if (removeCondition(CELLS_X, CELLS_Y, i, j)) continue;
            std::unique_ptr<OpenGLShape> cell = std::make_unique<Cuboid>(CUBE_W, CUBE_W, CUBE_W * CUBE_D);
            bool even = (i + j) & 1;
            float rgb = even ? 0.2 : 0.9;
            cell->setColor(rgb, rgb, rgb);
            float x = (i - CELLS_X / 2.0 + 0.5) * CUBE_W;
            float y = (j - CELLS_Y / 2.0 + 0.5) * CUBE_W;
            cell->translate(x, y, 0);
            gameBoard.push_back(std::move(cell));
        }
    }
}

void AppController::TEMPselectBoardIndex(const Ray& ray) {
    TEMPhitDistance = FLT_MAX;
    int hitIndex = -1;
    float dist;

    if (TEMPcylinder.intersectionTest(ray, dist)) {
        TEMPcylinder.select();
        TEMPhitDistance = dist;
    } else TEMPcylinder.deselect();
    for (int i = 0; i < gameBoard.size(); ++i) {
        gameBoard[i]->deselect();
        if (!gameBoard[i]->intersectionTest(ray, dist)) continue;
        if (dist < TEMPhitDistance) {
            TEMPhitDistance = dist;
            hitIndex = i;
        }
    }

    if (hitIndex >= 0) gameBoard[hitIndex]->select();
}

void AppController::updateRayLine() {
    const Ray& ray = cameraController.getMouseRay();
    if (!ray.isActive()) return;
    glm::vec3 origin = ray.getOrigin();
    glm::vec3 dir = TEMPhitDistance * ray.getDirection();
    rayLine.update(origin + dir * 0.1f, origin + dir);
}

/********** Main pipeline **********/

void AppController::updateTime() {
    ++frameCounter;
    float timeNow = window.getCurrentTime();
    timeDelta = timeNow - currentTime;
    currentTime = timeNow;
}

void AppController::handleInputMouse() {
    const Ray& ray = cameraController.getMouseRay();
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
    glm::mat4 projection = cameraController.getProjectionMatrix();

    const Shader& shader = shaders.get();
    shader.setMat4(ShaderParams::VIEW, view);
    shader.setMat4(ShaderParams::PROJECTION, projection);

    window.clearBuffer();

    for (const auto& shape : gameBoard) {
        shape->setUniforms(shader, currentTime);
        shape->draw();
    }

    // rotation:
    //TEMPcylinder.setModel(glm::rotate(TEMPcylinder.getBaseModel(), currentTime, glm::vec3(0.3f, 1.0f, 0.0f)));
    TEMPcylinder.setUniforms(shader, currentTime);
    TEMPcylinder.draw();

    //EVERY_N_FRAMES_DO(60, {std::cout << "1\n";});
    /*EVERY_N_FRAMES_DO(60, {
        std::cout << "RAY:\n" << rayLine;
        glm::vec3 cp = cameraController.getCameraPosition();
        std::cout << "CAMERA:\n" << cp[0] << ' ' << cp[1] << ' ' << cp[2] << "\n\n";
    });*/

    if (drawCameraRay) {
        const Shader& shader2 = shaders.getNext();
        shader2.setMat4(ShaderParams::VIEW, view);
        shader2.setMat4(ShaderParams::PROJECTION, projection);
        updateRayLine();
        // rayLine.update(SPACE_ORIGIN, {3*cos(currentTime), 3*sin(currentTime), 6}); // just make it spin now

        rayLine.setUniforms(shader2, currentTime); // registers GL error 1282 here, but only once
        rayLine.draw();
        shaders.next();
    }

    window.swapBuffers();
    window.pollEvents();
}