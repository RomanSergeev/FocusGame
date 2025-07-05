#pragma once
#include <string>
#include "glm/glm.hpp"
#include "glew/glew.h"

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
    Geometry,
    Program
};

std::string getShaderTypeString(ShaderType type);
glm::vec3 axisToVec3(Axis axis);
GLenum getGLDrawMode(DrawMode mode);