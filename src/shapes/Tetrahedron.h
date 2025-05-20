#pragma once
#include "OpenGLShape.h"

class Tetrahedron : public OpenGLShape {
    public:
        Tetrahedron();
        ShapeType getType() const override { return ShapeType::Tetrahedron; }
};