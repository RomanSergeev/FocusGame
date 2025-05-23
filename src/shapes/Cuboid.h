#pragma once
#include "OpenGLShape.h"

class Cuboid : public OpenGLShape {
    public:
        Cuboid();
        ShapeType getType() const override { return ShapeType::Cuboid; }
        void setUniforms(const Shader& shader) const override;
};