#include <algorithm>
#include "Utils.h"

void clampValue(float& value, float from, float to) { value = std::clamp(value, from, to); }
void clampValue(float& value, const std::pair<float, float>& pair) { value = std::clamp(value, pair.first, pair.second); }

std::ostream& operator << (std::ostream& out, const AABB& box) {
    out <<  box.min.x << ' ' << box.min.y << ' ' << box.min.z << '\n' <<
            box.max.x << ' ' << box.max.y << ' ' << box.max.z << "\n\n";
    return out;
}

bool AABB::contains(const glm::vec3& point) const {
    return glm::all(glm::lessThanEqual(min, point)) &&
           glm::all(glm::lessThanEqual(point, max));
}

bool AABB::intersects(const AABB& other) const {
    return (min.x <= other.max.x && max.x >= other.min.x) &&
           (min.y <= other.max.y && max.y >= other.min.y) &&
           (min.z <= other.max.z && max.z >= other.min.z);
}