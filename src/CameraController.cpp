#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>
#include "CameraController.h"
#include "shapes/ShapeEnums.h"

CameraController::CameraController(const GLWindow& window, const CameraSettings& settings, float radius) :
settings(std::move(settings)),
radius(radius),
targetRadius(radius) {
    GLFWwindow* handle = window.getHandle();
    glfwSetWindowUserPointer(handle, this); // store user pointer
    glfwSetMouseButtonCallback(handle, CameraController::mouseButtonCallback);
    glfwSetCursorPosCallback(handle, CameraController::mousePositionCallback);
    glfwSetScrollCallback(handle, CameraController::mouseScrollCallback);
    glfwSetFramebufferSizeCallback(handle, CameraController::resizeCallback);

    handleWindowResize(window.getWindowWidth(), window.getWindowHeight());
    clampRadius();
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
    if (rotating) {
        float sens = settings.sensitivity;
        float dx = xpos - lastX;
        float dy = ypos - lastY;
        
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
            yaw   += deltaX;
            pitch += deltaY;
        }

        if (settings.doClampYaw) clampYaw();
        if (settings.doClampPitch) clampPitch();
    }

    lastX = xpos;
    lastY = ypos;
}

void CameraController::handleMouseScroll(double xOffset, double yOffset) {
    targetRadius -= yOffset * settings.zoomStep;
    targetRadius = std::clamp(targetRadius, settings.zoomMin, settings.zoomMax);
    if (!settings.smoothZoom) radius = targetRadius;
}

void CameraController::handleWindowResize(int width, int height) {
    glViewport(0, 0, width, height);
    if (width > 0 && height > 0) {
        setAspectRatio((float)width / height);
    }
}

void CameraController::clampYaw() {
    yaw = std::clamp(yaw, -settings.yawMax, settings.yawMax);
}

void CameraController::clampPitch() {
    pitch = std::clamp(pitch, -settings.pitchMax, settings.pitchMax);
}

void CameraController::clampRadius() {
    radius = std::clamp(radius, settings.zoomMin, settings.zoomMax);
}

void CameraController::updateView(float timePassed) {
    if (settings.smoothZoom) radius += (targetRadius - radius) * settings.zoomSmoothFactor;

    if (!settings.smoothRotation) return;

    yaw += yawVelocity;
    pitch += pitchVelocity;

    yawVelocity   *= std::exp(-settings.rotateSlowdown * timePassed);
    pitchVelocity *= std::exp(-settings.rotateSlowdown * timePassed);

    if (std::abs(yawVelocity  ) < settings.rotateEpsilon) yawVelocity   = 0;
    if (std::abs(pitchVelocity) < settings.rotateEpsilon) pitchVelocity = 0;

    if (settings.doClampYaw) clampYaw();
    if (settings.doClampPitch) clampPitch();
}

glm::mat4 CameraController::getView() const {
    glm::vec3 cameraPos = glm::vec3(
        radius * cos(pitch) * sin(yaw),
        radius * sin(pitch),
        radius * cos(pitch) * cos(yaw)
    );

    glm::mat4 view = glm::lookAt(
        cameraPos, // camera position
        SPACE_ORIGIN, // target (origin)
        axisToVec3(Axis::Y) // up direction
    );

    return view;
}

void CameraController::setZoomLimits(float zoomMin, float zoomMax) {
#ifndef NDEBUG
    if (zoomMin < 0) throw std::invalid_argument("CameraController::setZoomLimits parameter zoomMin is negative.");
    if (zoomMax < 0) throw std::invalid_argument("CameraController::setZoomLimits parameter zoomMax is negative.");
    if (zoomMin > zoomMax) throw std::invalid_argument("CameraController::setZoomLimits parameters zoomMin and zoomMax are misaligned.");
#endif
    if (zoomMin < 0) zoomMin = DEFAULT_ZOOM;
    if (zoomMax < 0) zoomMax = DEFAULT_ZOOM;

    if (zoomMin > zoomMax) {
        float temp = zoomMin;
        zoomMin = zoomMax;
        zoomMax = temp;
    }

    settings.zoomMin = zoomMin;
    settings.zoomMax = zoomMax;
    clampRadius();
}

void CameraController::setAspectRatio(float ratio) {
    projectionMatrix = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 100.0f);;
}