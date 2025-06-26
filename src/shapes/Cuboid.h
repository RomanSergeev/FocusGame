#pragma once
#include "OpenGLShape.h"

class Cuboid : public OpenGLShape {
    float wx = 1, wy = 1, wz = 1; // width by each axis
public:
    Cuboid(float sizex, float sizey, float sizez);
    ShapeType getType() const override { return ShapeType::Cuboid; }
    DrawMode getDrawMode() const override { return DrawMode::Triangles; }
    //void setUniforms(const Shader& shader) const override;
};