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

    setupBuffer();
}