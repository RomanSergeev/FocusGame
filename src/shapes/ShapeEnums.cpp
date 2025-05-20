#include "ShapeEnums.h"

const glm::vec3 SPACE_ORIGIN(0.0f);

glm::vec3 axisToVec3(Axis axis) {
    switch (axis) {
        case Axis::X: return glm::vec3(1, 0, 0);
        case Axis::Y: return glm::vec3(0, 1, 0);
        case Axis::Z: return glm::vec3(0, 0, 1);
    }
    return SPACE_ORIGIN; // fallback
}