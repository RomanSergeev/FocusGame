#pragma once
#include "OpenGLShape.h"

class Triangle : public OpenGLShape {
public:
    Triangle();
    ShapeType getType() const override { return ShapeType::Triangle; }
    DrawMode getDrawMode() const override { return DrawMode::Triangles; }
};