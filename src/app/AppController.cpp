#include "AppController.h"
#include "glfw3.h"
#include "model/BoardEditor.h"
#include "utility/Logger.h"
#include "view/GLWindow.h"
#define EVERY_N_FRAMES_DO(n, code) do { if ((frameCounter % (n)) == 0) code; } while(0)

// these identical callbacks cannot be generalized because of the way GLFW (in C) obtains function pointers
void AppController::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    auto* controller = static_cast<AppController*>(glfwGetWindowUserPointer(window));
    if (!controller) return;
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) controller->cameraController.setRotating(true);
        else if (action == GLFW_RELEASE) controller->cameraController.setRotating(false);
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT) {
        const Ray& ray = controller->cameraController.getMouseRay();
        if (!ray.isActive()) return;
        if (action == GLFW_PRESS) controller->gameSession.handleMouseDown(ray);
        else if (action == GLFW_RELEASE) controller->gameSession.handleMouseUp(ray);
    }
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
    gameSession(BoardEditor::createBoard8x8Focus(), GameView::BoardShapeType::Flat) {

    registerCallbacks();
    Logger::instantiate(key, true);
    // std::cout << Checker::getInstancesCount() << '\n';
    // std::cout << Cell::getInstancesCount() << '\n';

    const Shader& shader = shaders.get();
    shader.setVec3(ShaderParams::LIGHT_DIR, -0.3f, -0.2f, -0.7f);

    rayLine.setColor(1.0f, 0.0f, 0.0f);

    CameraController::CameraSettings settings;
    settings.smoothRotation = true;
    settings.smoothZoom = true;
    settings.invertedHorizontalMouse = true;
    settings.invertedVerticalMouse = true;
    cameraController.updateSettings(std::move(settings));
    cameraController.setZoomLimits(4.0f, 30.0f);
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
        Logger::logs("Window initialization failed: ", e.what());
        return nullptr;
    }
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
    gameSession.hoverShapeByCameraRay(ray);
}

void AppController::handleInputKey() {
    auto action = inputHandler.handleKeyPress(window);
    if (action != InputHandler::InputAction::ToggleFullScreen) {
        toggledFullscreenLastFrame = false;
    } else {
        if (toggledFullscreenLastFrame) return;
        toggledFullscreenLastFrame = true;
        window.toggleFullscreenMode();
        if (window.isFullscreen()) cameraController.setRotationFactor(FULLSCREEN_ROTATION_SLOWDOWN_FACTOR);
        else cameraController.setRotationFactor(1);
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

    gameSession.drawBoard(shader, currentTime);

    // rotation:
    //turnIdentifier.setModel(glm::rotate(turnIdentifier.getBaseModel(), currentTime, glm::vec3(0.3f, 1.0f, 0.0f)));
    //turnIdentifier.setUniforms(shader, currentTime);
    // turnIdentifier.draw();

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