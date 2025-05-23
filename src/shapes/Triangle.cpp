#include "Triangle.h"

Triangle::Triangle() : OpenGLShape(3) {
    vertices = {
        -0.5f, -0.5f, 0.0f,  // Bottom-left
        0.5f, -0.5f, 0.0f,  // Bottom-right
        0.0f,  0.5f, 0.0f   // Top-center
    };

    // unused for now
    const char* shaderCodeVertices = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos, 1.0);  // No matrices, directly to clip space
        }
    )";

    // unused for now
    const char* shaderFragments = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 1.0, 0.0, 1.0);  // Bright yellow
        }
    )";
}