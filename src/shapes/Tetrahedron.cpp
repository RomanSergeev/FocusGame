#include "Tetrahedron.h"

constexpr float SQ3 = 0.577350269f;

Tetrahedron::Tetrahedron() : OpenGLShape(3) {
    vertices = {
        // Face 1 (front)     // normal vector (normalized)
         1.0f,  1.0f,  1.0f,//  SQ3, -SQ3, -SQ3, // A
        -1.0f, -1.0f,  1.0f,//  SQ3, -SQ3, -SQ3, // B
        -1.0f,  1.0f, -1.0f,//  SQ3, -SQ3, -SQ3, // C

        // Face 2 (right)
         1.0f,  1.0f,  1.0f,// -SQ3, -SQ3,  SQ3, // A
        -1.0f,  1.0f, -1.0f,// -SQ3, -SQ3,  SQ3, // C
         1.0f, -1.0f, -1.0f,// -SQ3, -SQ3,  SQ3, // D

        // Face 3 (left)
         1.0f,  1.0f,  1.0f,// -SQ3,  SQ3, -SQ3, // A
         1.0f, -1.0f, -1.0f,// -SQ3,  SQ3, -SQ3, // D
        -1.0f, -1.0f,  1.0f,// -SQ3,  SQ3, -SQ3, // B

        // Face 4 (bottom)
        -1.0f, -1.0f,  1.0f,//  SQ3,  SQ3,  SQ3, // B
         1.0f, -1.0f, -1.0f,//  SQ3,  SQ3,  SQ3, // D
        -1.0f,  1.0f, -1.0f //  SQ3,  SQ3,  SQ3  // C
    };

    shaderVertices = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )";

    shaderFragments = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 0.6, 0.2, 1.0); // Orange
        }
    )";
}