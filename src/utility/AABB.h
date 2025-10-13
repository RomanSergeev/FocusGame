#pragma once
#include <ostream>
#include "glm/vec3.hpp"

struct AABB { // Axis-Aligned Bounding Box
    glm::vec3 min;
    glm::vec3 max;

    AABB() = default;
    AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

    friend std::ostream& operator << (std::ostream& out, const AABB& box);

    glm::vec3 center() const { return (min + max) * 0.5f; }
    glm::vec3 size() const { return max - min; }

    bool contains(const glm::vec3& point) const;
    bool intersects(const AABB& other) const;
};