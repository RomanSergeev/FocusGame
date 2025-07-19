#include "Cylinder.h"
#include "Constants.h"
#include "shapes/OpenGLShape.h"

Cylinder::Cylinder(float sizex, float sizey, float sizez, unsigned int facets, bool altering) :
    OpenGLShape(6), wx(sizex), wy(sizey), wz(sizez), facets(facets), isAltering(altering) {

    pointsPerAttribute = {
        { AttributeType::Position, 3 },
        { AttributeType::Normal  , 3 }
    };

    float x = wx / 2, y = wy / 2, z = wz / 2;

    std::vector<glm::vec3> ptsBottom, ptsTop;
    float angleShift = altering ? (PI / facets) : 0;
    for (int i = 0; i < facets; ++i) {
        float ab = 2*PI*i / facets, // angle bottom
              at = ab + angleShift; // angle top
        ptsBottom.emplace_back(x * cos(ab), y * sin(ab), -z);
        ptsTop.emplace_back(x * cos(at), y * sin(at), z);
    }

    addFacePoly(ptsBottom);
    addFacePoly(ptsTop);

    for (int i = 0; i < facets; ++i) {
        int j = (i + 1) % facets;
        if (altering) {
            addFace(ptsBottom[i], ptsBottom[j], ptsTop[i]);
            addFace(ptsBottom[j], ptsTop[j], ptsTop[i]);
        } else {
            addFacePoly({ptsBottom[i], ptsBottom[j], ptsTop[j], ptsTop[i]});
        }
    }

    setupBuffer();
}