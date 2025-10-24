#pragma once
#include <unordered_map>
#include <string>
#include "glm/vec3.hpp"
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

enum class HighlightState {
    None,
    Hovered,
    Selected,
    CanAct,
    CannotAct
};

const std::unordered_map<HighlightState, glm::vec3> highlightColors = {
    { HighlightState::None     , glm::vec3(1) },
    { HighlightState::Hovered  , glm::vec3(1.0 , 1.0 , 0.5 ) },
    { HighlightState::Selected , glm::vec3(1.0 , 0.75, 0.0 ) },
    { HighlightState::CanAct   , glm::vec3(0.25, 1.0 , 0.25) },
    { HighlightState::CannotAct, glm::vec3(1.0 , 0.25, 0.25) }
};

std::string getShaderTypeString(ShaderType type);
glm::vec3 axisToVec3(Axis axis);
GLenum getGLDrawMode(DrawMode mode);