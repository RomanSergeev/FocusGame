#include "Cuboid.h"
#include "glm/gtc/matrix_transform.hpp"

/********** External constants **********/

Cuboid::Cuboid(float sizex, float sizey, float sizez) : OpenGLShape(6), wx(sizex), wy(sizey), wz(sizez) {
    pointsPerAttribute = {
        { AttributeType::Position, 3 },
        { AttributeType::Normal  , 3 }
    };

    float x = wx / 2, y = wy / 2, z = wz / 2;

    glm::vec3 A( x,  y,  z);
    glm::vec3 B( x,  y, -z);
    glm::vec3 C( x, -y, -z);
    glm::vec3 D( x, -y,  z);
    glm::vec3 E(-x,  y,  z);
    glm::vec3 F(-x,  y, -z);
    glm::vec3 G(-x, -y, -z);
    glm::vec3 H(-x, -y,  z);

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
}