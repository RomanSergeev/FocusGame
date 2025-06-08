#pragma once
#include "Utils.h"

class Ray {
    glm::vec3 origin;
    glm::vec3 direction;
    bool active;
public:
    Ray(const glm::vec3& origin, const glm::vec3& direction, bool active) : origin(origin), direction(direction), active(active) {}

    void setOrigin(const glm::vec3& newOrigin) { origin = newOrigin; }
    void setDirection(const glm::vec3& newDirection) { direction = glm::normalize(newDirection); }
    bool isActive() const { return active; }
    void activate() { active = true; }
    void deactivate() { active = false; }

    bool intersects(const AABB& boundingBox, float& distance) const;
    glm::vec3 pointAt(float t) const { return origin + direction * t; }
};