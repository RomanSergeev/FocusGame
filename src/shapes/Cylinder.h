#pragma once
#include "OpenGLShape.h"

class Cylinder : public OpenGLShape {
    float wx = 1, wy = 1, wz = 1; // width by each axis
    unsigned int facets = 16;
    bool isAltering = true;
protected:
    bool intersectsMathModel(const Ray& ray, float& distance) const override;
public:
    Cylinder(float sizex, float sizey, float sizez, unsigned int _facets = 16, bool altering = true);
    ShapeType getType() const override { return ShapeType::Cylinder; }
    DrawMode getDrawMode() const override { return DrawMode::Triangles; }
    bool pointOnSurface(const glm::vec3& point) const override;
};