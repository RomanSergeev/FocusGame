#include "RenderEnums.h"

std::string getShaderTypeString(ShaderType type) {
    switch (type) {
        case ShaderType::Vertex  : return "Vertex";
        case ShaderType::Fragment: return "Fragment";
        case ShaderType::Geometry: return "Geometry";
        case ShaderType::Program : return "Program";
        default: return "Unknown shader type";
    }
}

glm::vec3 axisToVec3(Axis axis) {
    switch (axis) {
        case Axis::X: return glm::vec3(1, 0, 0);
        case Axis::Y: return glm::vec3(0, 1, 0);
        case Axis::Z: return glm::vec3(0, 0, 1);
    }
    return glm::vec3(0, 0, 0); // fallback
}

GLenum getGLDrawMode(DrawMode mode) {
    GLenum result;
    switch (mode) {
        case DrawMode::Triangles: return GL_TRIANGLES;
        case DrawMode::Lines: return GL_LINES;
        case DrawMode::Points: return GL_POINTS;
    }
    return GL_NONE;
}