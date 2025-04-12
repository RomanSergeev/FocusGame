#include "Triangle.h"

std::vector<float> verticesTriangle = {
    -0.5f, -0.5f, 0.0f,  // Bottom-left
     0.5f, -0.5f, 0.0f,  // Bottom-right
     0.0f,  0.5f, 0.0f   // Top-center
};

const char* vertexShaderTriangle = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    void main() {
        gl_Position = vec4(aPos, 1.0);  // No matrices, directly to clip space
    }
)";

const char* fragmentShaderTriangle = R"(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(1.0, 1.0, 0.0, 1.0);  // Bright yellow
    }
)";