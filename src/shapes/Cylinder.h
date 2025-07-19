#pragma once
#include "OpenGLShape.h"

class Cylinder : public OpenGLShape {
    float wx = 1, wy = 1, wz = 1; // width by each axis
    unsigned int facets = 16;
    bool isAltering = true;
public:
    Cylinder(float sizex, float sizey, float sizez, unsigned int facets = 16, bool altering = true);
    ShapeType getType() const override { return ShapeType::Cylinder; }
    DrawMode getDrawMode() const override { return DrawMode::Triangles; }
    //void setUniforms(const Shader& shader) const override;
};