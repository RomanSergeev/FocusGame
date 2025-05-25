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

    clampRadius();
}

// these identical callbacks cannot be generalized because of the way GLFW (in C) obtains function pointers
void CameraController::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    auto* controller = static_cast<CameraController*>(glfwGetWindowUserPointer(window));
    if (controller) controller->handleMouseButton(button, action, mods);
}

void CameraController::mousePositionCallback(GLFWwindow* window, double xpos, double ypos) {
    auto* controller = static_cast<CameraController*>(glfwGetWindowUserPointer(window));
    if (controller) controller->handleMousePosition(xpos, ypos);
}

void CameraController::mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    auto* controller = static_cast<CameraController*>(glfwGetWindowUserPointer(window));
    if (controller) controller->handleMouseScroll(xOffset, yOffset);
}

void CameraController::handleMouseButton(int button, int action, int mods) {
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;
    if (action == GLFW_PRESS) rotating = true;
    else if (action == GLFW_RELEASE) rotating = false;
}

void CameraController::handleMousePosition(double xpos, double ypos) {
    if (rotating) {
        float sens = settings.sensitivity;
        float dx = xpos - lastX;
        float dy = ypos - lastY;
        float invertedMultYaw = settings.invertedHorizontalMouse ? -1 : 1;
        float invertedMultPitch = settings.invertedVerticalMouse ? -1 : 1;

        yawVelocity   = invertedMultYaw   * dx * sens;
        pitchVelocity = invertedMultPitch * dy * sens;

        if (!settings.smoothRotation) {
            yaw   += invertedMultYaw   * dx * sens;
            pitch += invertedMultPitch * dy * sens;
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