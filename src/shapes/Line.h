#pragma once
#include "OpenGLShape.h"

class Line : public OpenGLShape {
public:
    Line(const glm::vec3& A, const glm::vec3& B);
    ShapeType getType() const override { return ShapeType::Line; }
    DrawMode getDrawMode() const override { return DrawMode::Lines; }

    void update(const glm::vec3& A, const glm::vec3& B);
};