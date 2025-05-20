#include "Tetrahedron.h"

Tetrahedron::Tetrahedron() : OpenGLShape(6) {
    pointsPerAttribute = {
        { AttributeType::Position, 3 },
        { AttributeType::Normal  , 3 }
    };

    glm::vec3 A( 1.0f,  1.0f,  1.0f);
    glm::vec3 B(-1.0f, -1.0f,  1.0f);
    glm::vec3 C(-1.0f,  1.0f, -1.0f);
    glm::vec3 D( 1.0f, -1.0f, -1.0f);

    addFace(A, C, B);
    addFace(A, D, C);
    addFace(A, D, B);
    addFace(B, D, C);

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
        uniform vec3 tetraColor;

        void main() {
            float ambient = 0.1;
            float brightness = max(dot(normalize(Normal), normalize(-lightDir)), 0.0);
            vec3 color = tetraColor * (ambient + (1 - ambient) * brightness);
            FragColor = vec4(color, 1.0);
        }
    )";
}