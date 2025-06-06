#pragma once
#include "Utils.h"
#include "glm/gtc/constants.hpp"
#include "glm/fwd.hpp"
#include "glm/ext/matrix_transform.hpp"

class Camera {
    static constexpr float PI = glm::pi<float>();
    static constexpr float PI2 = glm::half_pi<float>();
    static constexpr float TWOPI = glm::two_pi<float>();
public:
    static constexpr std::pair<float, float> DEFAULT_ZOOM_LIMITS = { 2.0f, 20.0f };
    static constexpr std::pair<float, float> LIMIT_ZOOM = { 0.01f, 1000.0f };
    static constexpr std::pair<float, float> LIMIT_PITCH = { -PI2 + 0.1f, PI2 - 0.1f };

    Camera();

    float getDistance() const { return distance; }
    float getYaw() const { return yaw; }
    const glm::mat4& getProjectionMatrix() const { return projectionMatrix; }
    void setDistance(float newDistance);
    void setYaw(float newYaw);
    void setPitch(float newPitch);

    void addDistance(float distanceDelta);
    void addYaw(float yawDelta);
    void addPitch(float pitchDelta);
    glm::mat4 getView() const;
    void setAspectRatio(float ratio);
private:
    static float normalizeAngle(float angle);

    float yaw = 0.0f; // horizontal angle (around Y)
    float pitch = 0.0f; // vertical angle (around X)
    float distance; // distance from origin
    glm::mat4 projectionMatrix;
};