#include <algorithm>
#include <cmath>
#include "CameraController.h"
#include "shapes/ShapeEnums.h"

CameraController::CameraController(const GLWindow& window, float radius) : radius(radius), targetRadius(radius) {
    GLFWwindow* handle = window.getHandle();
    glfwSetWindowUserPointer(handle, this); // store user pointer
    glfwSetMouseButtonCallback(handle, CameraController::mouseButtonCallback);
    glfwSetCursorPosCallback(handle, CameraController::mousePositionCallback);
    glfwSetScrollCallback(handle, CameraController::mouseScrollCallback);
}

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

        yawVelocity = dx * sens;
        pitchVelocity = -dy * sens;

        if (!smoothRotation) {
            yaw   += dx * sens;
            pitch -= dy * sens;
        }

        clampPitch();
    }

    lastX = xpos;
    lastY = ypos;
}

void CameraController::handleMouseScroll(double xOffset, double yOffset) {
    targetRadius -= yOffset * settings.zoomStep;
    targetRadius = std::clamp(targetRadius, settings.zoomMin, settings.zoomMax);
    if (!smoothZoom) radius = targetRadius;
}

void CameraController::updateView() {
    if (smoothZoom) radius += (targetRadius - radius) * settings.zoomSmooth;

    if (!smoothRotation) return;

    yaw += yawVelocity;
    pitch += pitchVelocity;

    yawVelocity   *= settings.rotateSlowdown;
    pitchVelocity *= settings.rotateSlowdown;

    if (std::abs(yawVelocity  ) < settings.rotateEpsilon) yawVelocity   = 0;
    if (std::abs(pitchVelocity) < settings.rotateEpsilon) pitchVelocity = 0;

    clampPitch();
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

void CameraController::clampPitch() {
    pitch = std::clamp(pitch, -settings.pitchMax, settings.pitchMax);
    /*if (pitch > glm::radians(89.0f)) pitch = glm::radians(89.0f);
    if (pitch < glm::radians(-89.0f)) pitch = glm::radians(-89.0f);*/
}