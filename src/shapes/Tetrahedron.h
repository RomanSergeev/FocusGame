#pragma once
#include "OpenGLShape.h"

class Tetrahedron : public OpenGLShape {
    public:
        Tetrahedron();
        ShapeType getType() const override { return ShapeType::Tetrahedron; }
        void setUniforms(const Shader& shader) const override;
};