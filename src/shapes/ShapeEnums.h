#pragma once
#include "glm/glm.hpp"
#include "glew/glew.h"

extern const glm::vec3 SPACE_ORIGIN;

enum class ShapeType {
    UNKNOWN,
    Triangle,
    Tetrahedron,
    Cuboid,
    Line
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

enum class DrawMode {
    Triangles,
    Lines,
    Points
};

enum class ShaderType {
    Vertex,
    Fragment,
    Geometry
};

glm::vec3 axisToVec3(Axis axis);
GLenum getGLDrawMode(DrawMode mode);