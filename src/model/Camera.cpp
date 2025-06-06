#include <cmath>
#include "Camera.h"
#include "shapes/ShapeEnums.h"
#include "glm/ext/matrix_clip_space.hpp"

float Camera::normalizeAngle(float angle) {
    angle = std::fmod(angle, TWOPI);
    if (angle < -PI) angle += TWOPI;
    if (angle >= PI) angle -= TWOPI;
    return angle;
}

Camera::Camera() {
    distance = (DEFAULT_ZOOM_LIMITS.first + DEFAULT_ZOOM_LIMITS.second) / 2;
    setAspectRatio(1);
}

void Camera::addDistance(float dist) {
    distance += dist;
    clampValue(distance, LIMIT_ZOOM);
}

glm::mat4 Camera::getView() const {
    glm::vec3 cameraPos = glm::vec3(
        distance * cos(pitch) * sin(yaw),
        distance * sin(pitch),
        distance * cos(pitch) * cos(yaw)
    );

    glm::mat4 view = glm::lookAt(
        cameraPos, // camera position
        SPACE_ORIGIN, // target (origin)
        axisToVec3(Axis::Y) // up direction
    );

    return view;
}

void Camera::setAspectRatio(float ratio) {
    projectionMatrix = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 100.0f);
}

void Camera::addYaw(float yawDelta) {
    yaw = normalizeAngle(yaw + yawDelta);
}

void Camera::addPitch(float pitchDelta) {
    pitch += pitchDelta;
    clampValue(pitch, Camera::LIMIT_PITCH);
}