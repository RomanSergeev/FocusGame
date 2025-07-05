#include <cmath>
#include "Camera.h"
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

void Camera::setDistance(float newDistance) {
    distance = newDistance;
    clampValue(distance, LIMIT_ZOOM);
}

void Camera::setYaw(float newYaw) {
    yaw = normalizeAngle(newYaw);
}

void Camera::setPitch(float newPitch) {
    pitch = newPitch;
    clampValue(pitch, Camera::LIMIT_PITCH);
}

void Camera::addDistance(float distanceDelta) {
    setDistance(distance + distanceDelta);
}

void Camera::addYaw(float yawDelta) {
    setYaw(yaw + yawDelta);
}

void Camera::addPitch(float pitchDelta) {
    setPitch(pitch + pitchDelta);
}

glm::vec3 Camera::getPosition() const {
    return distance * glm::vec3(
        cos(pitch) * sin(yaw),
        sin(pitch),
        cos(pitch) * cos(yaw)
    ) + origin;
}

glm::mat4 Camera::getView() const {
    glm::vec3 cameraPos = getPosition();

    glm::mat4 view = glm::lookAt(
        cameraPos, // camera position
        origin, // target (origin)
        axisToVec3(Axis::Y) // up direction
    );

    return view;
}

void Camera::setAspectRatio(float ratio) {
    projectionMatrix = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 100.0f);
}