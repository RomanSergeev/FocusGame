#include "Tetrahedron.h"

constexpr float SQ3 = 0.577350269f;

Tetrahedron::Tetrahedron() : OpenGLShape(6) {
    pointsPerAttribute = {3, 3};

    vertices = {
        // Face 1 (front)     // normal vector (normalized)
         1.0f,  1.0f,  1.0f, -SQ3,  SQ3,  SQ3, // A
        -1.0f, -1.0f,  1.0f, -SQ3,  SQ3,  SQ3, // B
        -1.0f,  1.0f, -1.0f, -SQ3,  SQ3,  SQ3, // C

        // Face 2 (right)
         1.0f,  1.0f,  1.0f,  SQ3,  SQ3, -SQ3, // A
        -1.0f,  1.0f, -1.0f,  SQ3,  SQ3, -SQ3, // C
         1.0f, -1.0f, -1.0f,  SQ3,  SQ3, -SQ3, // D

        // Face 3 (left)
         1.0f,  1.0f,  1.0f, -SQ3,  SQ3, -SQ3, // A
         1.0f, -1.0f, -1.0f, -SQ3,  SQ3, -SQ3, // D
        -1.0f, -1.0f,  1.0f, -SQ3,  SQ3, -SQ3, // B

        // Face 4 (bottom)
        -1.0f, -1.0f,  1.0f,  SQ3,  SQ3,  SQ3, // B
         1.0f, -1.0f, -1.0f,  SQ3,  SQ3,  SQ3, // D
        -1.0f,  1.0f, -1.0f,  SQ3,  SQ3,  SQ3  // C
    };

    shaderVertices = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;

        out vec3 FragPos;
        out vec3 Normal;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0)); // world space position
            Normal = mat3(transpose(inverse(model))) * aNormal; // normal in world space
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";

    shaderFragments = R"(
        #version 330 core
        out vec4 FragColor;

        in vec3 FragPos;
        in vec3 Normal;

        uniform vec3 lightDir;
        uniform vec3 baseColor;

        void main() {
            float brightness = max(dot(normalize(Normal), normalize(-lightDir)), 0.0);
            vec3 color = baseColor * brightness;
            FragColor = vec4(color, 1.0);
        }
    )";
}