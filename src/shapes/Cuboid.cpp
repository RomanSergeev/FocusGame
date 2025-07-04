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

    // order of vertices inside lists also matters, if triangles criss-cross on adjacent edges of touching cubes, artifacts will appear 
    addFacePoly({A, D, C, B});
    addFacePoly({E, H, G, F});
    addFacePoly({A, E, H, D});
    addFacePoly({B, F, G, C});
    addFacePoly({B, F, E, A});
    addFacePoly({C, G, H, D});

    setupBuffer();
}