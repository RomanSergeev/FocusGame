#pragma once
#include <utility>
#include "glm/glm.hpp"

void clampValue(float& value, float from, float to);
void clampValue(float& value, const std::pair<float, float>& pair);

struct AABB { // Axis-Aligned Bounding Box
    glm::vec3 min;
    glm::vec3 max;

    AABB() = default;
    AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

    glm::vec3 center() const { return (min + max) * 0.5f; }
    glm::vec3 size() const { return max - min; }

    bool contains(const glm::vec3& point) const;
    bool intersects(const AABB& other) const;
};