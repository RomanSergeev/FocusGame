#include "Triangle.h"

Triangle::Triangle() : OpenGLShape(3) {
    setVertices({
        -0.5f, -0.5f, 0.0f,  // Bottom-left
        0.5f, -0.5f, 0.0f,  // Bottom-right
        0.0f,  0.5f, 0.0f   // Top-center
    });

    setupBuffer();
}