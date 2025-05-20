#pragma once
#include "glm/glm.hpp"

extern const glm::vec3 SPACE_ORIGIN;

enum class ShapeType {
    UNKNOWN,
    Triangle,
    Tetrahedron,
    Cuboid
};

enum class AttributeType {
    Position,
    Normal,
    Color
};

enum class Axis {
    X,
    Y,
    Z
};

glm::vec3 axisToVec3(Axis axis);