#include "Cuboid.h"
#include "glm/gtc/matrix_transform.hpp"

/********** External constants **********/

Cuboid::Cuboid() : OpenGLShape(6) {
    pointsPerAttribute = {
        { AttributeType::Position, 3 },
        { AttributeType::Normal  , 3 }
    };

    glm::vec3 A( 1.0f,  1.0f,  1.0f);
    glm::vec3 B( 1.0f,  1.0f, -1.0f);
    glm::vec3 C( 1.0f, -1.0f, -1.0f);
    glm::vec3 D( 1.0f, -1.0f,  1.0f);
    glm::vec3 E(-1.0f,  1.0f,  1.0f);
    glm::vec3 F(-1.0f,  1.0f, -1.0f);
    glm::vec3 G(-1.0f, -1.0f, -1.0f);
    glm::vec3 H(-1.0f, -1.0f,  1.0f);

    addFace(A, C, B);
    addFace(A, D, C);
    addFace(E, G, F);
    addFace(E, H, G);
    addFace(A, E, D);
    addFace(D, E, H);
    addFace(B, F, C);
    addFace(C, F, G);
    addFace(A, B, E);
    addFace(B, F, E);
    addFace(D, C, H);
    addFace(C, G, H);

    baseModel = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.0f, 0.0f));
}

void Cuboid::setUniforms(const Shader& shader) const {
    OpenGLShape::setUniforms(shader);
    shader.setVec3("baseColor", 0.2f, 1.0f, 0.6f);
}