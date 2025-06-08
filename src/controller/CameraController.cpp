#include <cmath>
#include <memory>
#include <stdexcept>
#include "CameraController.h"
#include "glfw/glfw3.h"

CameraController::CameraSettings::CameraSettings() {
    zoomMin = Camera::DEFAULT_ZOOM_LIMITS.first;
    zoomMax = Camera::DEFAULT_ZOOM_LIMITS.second;
    sanitize();
}

void CameraController::CameraSettings::setZoomLimits(float zoomFrom, float zoomTo) {
    zoomMin = zoomFrom;
    zoomMax = zoomTo;
    sanitize(); // will swap if unordered
}

void CameraController::CameraSettings::sanitize() {
#ifndef NDEBUG
    if (zoomMin < 0) throw std::invalid_argument("CameraController::CameraSettings parameter zoomMin is negative.");
    if (zoomMax < 0) throw std::invalid_argument("CameraController::CameraSettings parameter zoomMax is negative.");
    if (zoomMin > zoomMax) throw std::invalid_argument("CameraController::CameraSettings parameters zoomMin and zoomMax are misaligned.");
#endif
    clampValue(zoomMin, Camera::LIMIT_ZOOM);
    clampValue(zoomMax, Camera::LIMIT_ZOOM);
    if (zoomMin > zoomMax)
        std::swap(zoomMin, zoomMax);
    clampValue(sensitivity, LIMIT_SENSITIVITY);
    clampValue(zoomStep, LIMIT_ZOOM_STEP);
    clampValue(zoomSmoothFactor, LIMIT_ZOOM_SMOOTH);
    clampValue(rotateSlowdown, LIMIT_ROTATE_SLOWDOWN);
}

CameraController::CameraController(const GLWindow& window) :
ray(SPACE_ORIGIN, SPACE_ORIGIN, false)
{
    targetDistance = camera.getDistance();
    GLFWwindow* handle = window.getHandle();
    glfwSetWindowUserPointer(handle, this); // store user pointer
    glfwSetMouseButtonCallback(handle, CameraController::mouseButtonCallback);
    glfwSetCursorPosCallback(handle, CameraController::mousePositionCallback);
    glfwSetScrollCallback(handle, CameraController::mouseScrollCallback);
    glfwSetFramebufferSizeCallback(handle, CameraController::resizeCallback);

    handleWindowResize(window.getWindowWidth(), window.getWindowHeight());
    clampDistance();
}

void CameraController::updateSettings(CameraSettings&& settings) {
    CameraSettings temp = std::move(settings);
    temp.sanitize();
    this->settings = std::move(temp);
    clampDistance();
}

// these identical callbacks cannot be generalized because of the way GLFW (in C) obtains function pointers
void CameraController::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    auto* controller = static_cast<CameraController*>(glfwGetWindowUserPointer(window));
    if (controller) controller->handleMouseButton(button, action, mods);
}

void CameraController::mousePositionCallback(GLFWwindow* window, double xpos, double ypos) {
    auto* controller = static_cast<CameraController*>(glfwGetWindowUserPointer(window));
    if (controller) controller->handleMousePosition(window, xpos, ypos);
}

void CameraController::mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    auto* controller = static_cast<CameraController*>(glfwGetWindowUserPointer(window));
    if (controller) controller->handleMouseScroll(xOffset, yOffset);
}

void CameraController::resizeCallback(GLFWwindow* window, int width, int height) {
    auto* controller = static_cast<CameraController*>(glfwGetWindowUserPointer(window));
    if (controller) controller->handleWindowResize(width, height);
}

void CameraController::handleMouseButton(int button, int action, int mods) {
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;
    if (action == GLFW_PRESS) rotating = true;
    else if (action == GLFW_RELEASE) rotating = false;
}

void CameraController::handleMousePosition(GLFWwindow* window, double xpos, double ypos) {
    updateRayFromCursor(window, xpos, ypos);
    float dx = xpos - lastX;
    float dy = ypos - lastY;
    lastX = xpos;
    lastY = ypos;
    if (!rotating) return;

    float sens = settings.sensitivity;

    const int DEFAULT_WIDTH = 800, DEFAULT_HEIGHT = 600;

    int wWidth, wHeight;
    glfwGetFramebufferSize(window, &wWidth, &wHeight);
    if (wWidth <= 0 || wHeight <= 0) return; // causes a #DIV0 crash when alt-tabbing from fullscreen app
    float scaleX = DEFAULT_WIDTH / (float)wWidth;
    float scaleY = DEFAULT_HEIGHT / (float)wHeight;
    float invertedMultYaw = settings.invertedHorizontalMouse ? -1 : 1;
    float invertedMultPitch = settings.invertedVerticalMouse ? -1 : 1;

    float deltaX = invertedMultYaw   * dx * scaleX * sens;
    float deltaY = invertedMultPitch * dy * scaleY * sens;

    yawVelocity   = deltaX;
    pitchVelocity = deltaY;

    if (!settings.smoothRotation) {
        addYaw(deltaX);
        addPitch(deltaY);
    }
}

void CameraController::handleMouseScroll(double xOffset, double yOffset) {
    targetDistance -= yOffset * settings.zoomStep;
    clampDistance();
    float oldDistance = camera.getDistance();
    if (!settings.smoothZoom) camera.setDistance(targetDistance);
}

void CameraController::handleWindowResize(int width, int height) {
    glViewport(0, 0, width, height);
    if (width > 0 && height > 0) {
        camera.setAspectRatio((float)width / height);
    }
}

void CameraController::updateRayFromCursor(GLFWwindow* window, double xpos, double ypos) {
    // 1. Get window dimensions, deactivate if unfocused
    int winWidth, winHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);
    if (winWidth == 0 || winHeight == 0 ||
        xpos < 0 || ypos < 0 ||
        xpos > winWidth || ypos > winHeight) {
        ray.deactivate();
        return;
    }

    // 2. Convert mouse position from screen space to Normalized Device Coordinates (NDC)
    float x = 2.0f * xpos / winWidth - 1.0f;
    float y = 1.0f - 2.0f * ypos / winHeight;
    float z = 1.0f;

    glm::vec4 rayNDC(x, y, -1.0f, 1.0f); // Normalized Device Coordinates, z = -1 for near plane
    glm::vec4 rayClip(rayNDC);

    // 3. Unproject to eye space
    glm::mat4 invProj = glm::inverse(camera.getProjectionMatrix());
    glm::vec4 rayEye = invProj * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f); // We set z=-1, w=0 for direction

    // 4. Unproject to world space
    glm::mat4 invView = glm::inverse(camera.getView());
    glm::vec4 rayWorld4 = invView * rayEye;
    glm::vec3 rayDir = glm::vec3(rayWorld4);

    // 5. Update ray
    ray.activate();
    ray.setOrigin(camera.getPosition()); // Make sure you expose getPosition() from Camera
    ray.setDirection(rayDir);
}

void CameraController::addYaw(float yawDelta) {
    if (!settings.doClampYaw) {
        camera.addYaw(yawDelta);
        return;
    }
    float oldYaw = camera.getYaw();
    float newYaw = oldYaw + yawDelta;
    clampValue(newYaw, CameraSettings::LIMIT_YAW);
    camera.setYaw(newYaw);
}

void CameraController::addPitch(float pitchDelta) {
    camera.addPitch(pitchDelta);
}

void CameraController::clampDistance() {
    float zoomMin = settings.getZoomMinimal();
    float zoomMax = settings.getZoomMaximal();
    clampValue(targetDistance, zoomMin, zoomMax);
}

void CameraController::updateView(float timePassed) {
    float distance = camera.getDistance();
    if (settings.smoothZoom) camera.addDistance((targetDistance - distance) * settings.zoomSmoothFactor);

    if (!settings.smoothRotation) return;

    addYaw(yawVelocity);
    addPitch(pitchVelocity);

    yawVelocity   *= std::exp(-settings.rotateSlowdown * timePassed);
    pitchVelocity *= std::exp(-settings.rotateSlowdown * timePassed);

    if (std::abs(yawVelocity  ) < CameraSettings::ROTATE_STOP_EPSILON) yawVelocity   = 0;
    if (std::abs(pitchVelocity) < CameraSettings::ROTATE_STOP_EPSILON) pitchVelocity = 0;
}

void CameraController::setZoomLimits(float zoomFrom, float zoomTo) {
    settings.setZoomLimits(zoomFrom, zoomTo);
    clampDistance();
}