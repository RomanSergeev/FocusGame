#pragma once
#include "OpenGLShape.h"

class Tetrahedron : public OpenGLShape {
public:
    Tetrahedron();
    ShapeType getType() const override { return ShapeType::Tetrahedron; }
    DrawMode getDrawMode() const override { return DrawMode::Triangles; }
    //void setUniforms(const Shader& shader) const override;
};